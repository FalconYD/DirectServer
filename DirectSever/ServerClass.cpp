#include "stdafx.h"

#include "ServerClass.h"

ServerClass::ServerClass(int nPort)
{
	InitializeSRWLock(&m_srwClient);
	//cout << "Starting up TCP Chat server\n";
	m_bIsConnected = false;

	WSADATA wsaData;

	sockaddr_in local;

	int wsaret = WSAStartup(0x101, &wsaData);

	if (wsaret != 0)
	{
		return;
	}

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons((u_short)nPort);
	
	m_sockServerListener = socket(AF_INET, SOCK_STREAM, 0);
	//m_SListenClient = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);

	if (m_sockServerListener == INVALID_SOCKET)
		return;

	if (bind(m_sockServerListener, (sockaddr*)&local, sizeof(local)) != 0)
		return;

	if (listen(m_sockServerListener, 10) != 0)
		return;

	m_bIsConnected = true;
	return;
}

ServerClass::~ServerClass()
{
	DWORD nExitCode = NULL;
	
	m_stThreadListen.bThreadFlag = false;

	GetExitCodeThread(m_stThreadListen.hThread, &nExitCode);
	TerminateThread(m_stThreadListen.hThread, nExitCode);
	m_stThreadListen.hThread = NULL;

	SendServerDown();
	for (auto* client : m_listClients)
	{

		delete(ClientSock*)client;
		client = nullptr;
	}


	closesocket(m_sockServerListener);

	WSACleanup();
}

void ServerClass::StartListenClient()
{
	sockaddr_in from;
	int fromlen = sizeof(from);
	SOCKET sock = accept(m_sockServerListener, (struct sockaddr*) & from, &fromlen);
	
	if (sock != INVALID_SOCKET)
	{
		ClientSock* pSock = new ClientSock(this);
		pSock->SetSocket(sock);
		AddClient(pSock);
		pSock->StartRecv();
	}
	else
		return;
}

void ServerClass::RunListen()
{
	m_stThreadListen.bThreadFlag = true;
	m_stThreadListen.cObjectPointer1 = this;
	m_stThreadListen.nScanInterval = 1;
	m_stThreadListen.enState = THREAD_STATUS::THREAD_STAT_NONE;
	HANDLE hThread = (HANDLE*)_beginthreadex(NULL, 0, THREAD_SERVER_LISTEN, &m_stThreadListen, 0, NULL);
	m_stThreadListen.hThread = hThread;
	CloseHandle(hThread);
}


void ServerClass::WriteLog(char* strLog, ...)
{
	char chLog[LOG_TEXT_MAX_SIZE] = { 0, };
	char chBuff[LOG_TEXT_MAX_SIZE] = { 0, };
	wchar_t wchMsg[LOG_TEXT_MAX_SIZE] = { 0, };
	int nScrollCount = 0;
	va_list ap;
	va_start(ap, strLog);
	vsnprintf_s(chLog, LOG_TEXT_MAX_SIZE, strLog, ap);
	va_end(strLog);

	m_fn_WriteLog(chLog);
}

void ServerClass::WriteClient(char* strLog, ...)
{
	char chLog[LOG_TEXT_MAX_SIZE] = { 0, };
	char chBuff[LOG_TEXT_MAX_SIZE] = { 0, };
	wchar_t wchMsg[LOG_TEXT_MAX_SIZE] = { 0, };
	int nScrollCount = 0;
	va_list ap;
	va_start(ap, strLog);
	vsnprintf_s(chLog, LOG_TEXT_MAX_SIZE, strLog, ap);
	va_end(strLog);

	m_fn_WriteInfoClient(chLog);
}

unsigned int ServerClass::THREAD_SERVER_LISTEN(LPVOID pParam)
{
	THREAD_PARAM* pThreadparam = (THREAD_PARAM*)pParam;
	ServerClass* pDlg = (ServerClass*)pThreadparam->cObjectPointer1;

	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
	while (pThreadparam->bThreadFlag)
	{
		Sleep(pThreadparam->nScanInterval);
		pDlg->StartListenClient();
	}
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	return 0;
}

void ServerClass::AddClient(ClientSock* clientSock)
{
	AcquireSRWLockExclusive(&m_srwClient);
	m_listClients.push_back(clientSock);
	ReleaseSRWLockExclusive(&m_srwClient);
	WriteLog("Create Socket Client.");
}

void ServerClass::RemoveClient(ClientSock* clientSock)
{
	if (m_listClients.size() > 0)
	{
		AcquireSRWLockExclusive(&m_srwClient);
		m_listClients.remove(clientSock);
		ReleaseSRWLockExclusive(&m_srwClient);

		delete(ClientSock*) clientSock;
		clientSock = nullptr;
		WriteLog("Destroy Socket Client.");
	}
}


void ServerClass::RegTask(ClientSock* sock, int nTaskNum)
{
	std::list<ClientSock*>::iterator itl;
	int nListIndex = 0;
	for (itl = m_listClients.begin(); itl != m_listClients.end(); itl++)
	{
		if ((*itl)->GetTaskNum() == nTaskNum)
		{
			WriteLog("Regist Fail : %d socket (Index : %d)", nListIndex, nTaskNum);
			return;
		}
		nListIndex++;
	}

	nListIndex = 0;
	for (itl = m_listClients.begin(); itl != m_listClients.end(); itl++)
	{
		if ((*itl)->GetSocket() == sock->GetSocket())
		{
			(*itl)->SetTaskNum(nTaskNum);
			//sock.SetTaskNum(nTaskNum);
			WriteLog("Regist Success : %d socket (Index : %d)", nListIndex, nTaskNum);
			break;
		}
		nListIndex++;
	}
}


bool ServerClass::GetAliveClient(CMDMSG msg)
{
	bool bRet = false;
	int iStat = 0;
	std::list<ClientSock*>::iterator itl;
	if (m_listClients.size() == 0)
		return 0;

	ALIVESTATE* pState = (ALIVESTATE*)msg.cMsgBuf;

	const int MAXINDEX = MSG_BUFF_SIZE / sizeof(ALIVESTATE);

	int idx;
	// GetData.
	for (itl = m_listClients.begin(); itl != m_listClients.end(); itl++)
	{
		idx = (*itl)->GetTaskNum();
		if (idx > -1 && idx < MAXINDEX)
		{
			pState[idx].tasknum = idx;
			pState[idx].taskstate = 1;
		}
		else
		{
			// idx over or not regist task
			if (idx == -1)
				WriteLog("Unregisted Task : [%d] %s", idx, (*itl)->GetAddressName());

			if (idx > MAXINDEX)
				WriteLog("Over Index : %s", (*itl)->GetAddressName());
		}
	}

	// SendData.
	for (itl = m_listClients.begin(); itl != m_listClients.end(); itl++)
	{
		if ((*itl)->GetTaskNum() == msg.uTask_Src)
		{
			msg.uRetStatus = bRet;
			iStat = send((*itl)->GetSocket(), (char*)&msg, sizeof(CMDMSG), 0);
			break;
		}
	}

	if (iStat == -1)
		bRet = false;

	return bRet;
}


int ServerClass::SendMessagePort(CMDMSG stMsg)
{
	int iStat = 0;
	std::list<ClientSock*>::iterator itl;

	if (m_listClients.size() == 0)
		return 0;

	for (itl = m_listClients.begin(); itl != m_listClients.end(); itl++)
	{
		if (stMsg.uMsg_Type == MSGTYPE::EMT_TARGET)
		{
			if ((*itl)->GetTaskNum() == stMsg.uTask_Dst)
			{
				iStat = send((*itl)->GetSocket(), (char*)&stMsg, sizeof(CMDMSG), 0);
				if (iStat != -1)
					break;
			}
		}
		else
		{
			iStat = send((*itl)->GetSocket(), (char*)&stMsg, sizeof(CMDMSG), 0);
			if (iStat == -1)
			{
				RemoveClient(*itl);
			}
		}
	}
	if (iStat == -1)
		return 1;

	return 0;
}

void ServerClass::SendServerDown()
{
	CMDMSG msg;
	msg.uMsg_Type = MSGTYPE::EMT_SERVERDOWN;
	msg.uRetStatus = 1;

	SendMessagePort(msg);
}