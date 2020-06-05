#include "stdafx.h"

#include "ServerClass.h"

ServerClass::ServerClass(int nPort)
{
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
	
	m_SListenClient = socket(AF_INET, SOCK_STREAM, 0);

	if (m_SListenClient == INVALID_SOCKET)
		return;

	if (bind(m_SListenClient, (sockaddr*)&local, sizeof(local)) != 0)
		return;

	if (listen(m_SListenClient, 10) != 0)
		return;

	m_bIsConnected = true;
	return;
}

ServerClass::~ServerClass()
{
	

	DWORD nExitCode = NULL;

	m_stThreadRecv.bThreadFlag = false;
	while (m_stThreadRecv.enState == THREAD_STATUS::THREAD_STAT_RUNNING)
	{
		Sleep(1);
	}
	GetExitCodeThread(m_stThreadRecv.hThread, &nExitCode);
	m_stThreadRecv.hThread = NULL;

	
	m_stThreadListen.bThreadFlag = false;
// 	while (m_stThreadListen.enState == THREAD_STATUS::THREAD_STAT_RUNNING)
// 	{
// 		Sleep(1);
// 	}

	GetExitCodeThread(m_stThreadListen.hThread, &nExitCode);
	TerminateThread(m_stThreadListen.hThread, nExitCode);
	m_stThreadListen.hThread = NULL;

	

	closesocket(m_SListenClient);

	WSACleanup();
}

void ServerClass::StartListenClient()
{
	sockaddr_in from;
	int fromlen = sizeof(from);
	SOCKET sock = accept(m_SListenClient, (struct sockaddr*) & from, &fromlen);
	m_SClient.SetSocket(sock);
	if (m_SClient.GetSocket() != INVALID_SOCKET)
		m_vClientList.push_back(m_SClient);
	else
		return;

	m_stThreadRecv.bThreadFlag = true;
	m_stThreadRecv.cObjectPointer1 = this;
	m_stThreadRecv.cObjectPointer2 = &m_SClient;
	m_stThreadRecv.nScanInterval = 1;
	m_stThreadRecv.enState = THREAD_STATUS::THREAD_STAT_NONE;
	HANDLE hThread = (HANDLE*)_beginthreadex(NULL, 0, THREAD_SERVER_RECV, &m_stThreadRecv, 0, NULL);
	m_stThreadRecv.hThread = hThread;
	CloseHandle(hThread);
}

int ServerClass::SendMessagePort(CMDMSG stMsg)
{
	int iStat = 0;
	std::list<SocketDefine>::iterator itl;

	if (m_vClientList.size() == 0)
		return 0;

	for (itl = m_vClientList.begin(); itl != m_vClientList.end(); itl++)
	{
		if (stMsg.uMsg_Type == MSGTYPE::EMT_TARGET)
		{
			if (itl->GetIndex() == stMsg.uTask_Dst)
			{
				iStat = send(itl->GetSocket(), (char*)&stMsg, sizeof(CMDMSG), 0);
				if (iStat != -1)
					break;
			}
		}
		else
		{
			iStat = send(itl->GetSocket(), (char*)&stMsg, sizeof(CMDMSG), 0);
			if (iStat == -1)
				m_vClientList.remove(*itl);
		}
	}
	if (iStat == -1)
		return 1;

	return 0;
}

int ServerClass::RecClient(SocketDefine sRecSocket)
{
	char temp[sizeof(CMDMSG)] = { 0, };
	
	SOCKET sock = sRecSocket.GetSocket();
	if (sock == INVALID_SOCKET)
		return -1;

	int iStat = recv(sock, temp, sizeof(CMDMSG), 0);
	if (iStat == -1)
	{
		m_vClientList.remove(sRecSocket);
		return 1;
	}
	else
	{
		memcpy(&m_stCmdMsg, temp, sizeof(CMDMSG));
		
		WriteLog("[%d:%d:%d][%d:%d:%d][%d:%d:%d]%d,%d,%d : %s",
			m_stCmdMsg.uMsg_Type, m_stCmdMsg.uTask_Src, m_stCmdMsg.uTask_Dst,
			m_stCmdMsg.uFunID_Src, m_stCmdMsg.uSeqID_Src, m_stCmdMsg.uUnitID_Src,
			m_stCmdMsg.uFunID_Dst, m_stCmdMsg.uSeqID_Dst, m_stCmdMsg.uUnitID_Dst,
			m_stCmdMsg.uRetStatus, m_stCmdMsg.uMsgSize, m_stCmdMsg.cDummy, m_stCmdMsg.cMsgBuf);

		if (m_stCmdMsg.uFunID_Dst == FUNCCMD::EFC_TASK_REG)
			RegTask(sRecSocket, m_stCmdMsg.uTask_Src);
		else
			SendMessagePort(m_stCmdMsg);

		return 0;
	}
	return 0;
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

void ServerClass::RegTask(SocketDefine &sock, int nIndex)
{
	std::list<SocketDefine>::iterator itl;
	int nListIndex = 0;
	for (itl = m_vClientList.begin(); itl != m_vClientList.end(); itl++)
	{
		if (itl->GetIndex() == nIndex)
		{
			WriteLog("Regist Fail : %d socket (Index : %d)", nListIndex, nIndex);
			return;
		}
		nListIndex++;
	}

	nListIndex = 0;
	for (itl = m_vClientList.begin(); itl != m_vClientList.end(); itl++)
	{
		if (itl->CompareSocket(sock))
		{
			itl->SetIndex(nIndex);
			sock.SetIndex(nIndex);
			WriteLog("Regist Success : %d socket (Index : %d)", nListIndex, nIndex);
			break;
		}
		nListIndex++;
	}
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

unsigned int ServerClass::THREAD_SERVER_LISTEN(LPVOID pParam)
{
	THREAD_PARAM* pThreadparam = (THREAD_PARAM*)pParam;
	ServerClass* pDlg = (ServerClass*)pThreadparam->cObjectPointer1;

	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;
	while (pThreadparam->bThreadFlag)
	{
		Sleep(pThreadparam->nScanInterval);
		pThreadparam->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
		pDlg->StartListenClient();
	}
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	return 0;
}

unsigned int ServerClass::THREAD_SERVER_RECV(LPVOID pParam)
{
	THREAD_PARAM* pThreadparam = (THREAD_PARAM*)pParam;
	ServerClass* pDlg = (ServerClass*)pThreadparam->cObjectPointer1;
	SocketDefine Socket = *(SocketDefine*)pThreadparam->cObjectPointer2;

	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;
	while (pThreadparam->bThreadFlag)
	{
		Sleep(pThreadparam->nScanInterval);
		pThreadparam->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
		pDlg->RecClient(Socket);
	}
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	return 0;
}