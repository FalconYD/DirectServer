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
	//m_SListenClient = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);

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


	m_stThreadSender.bThreadFlag = false;
	while (m_stThreadSender.enState == THREAD_STATUS::THREAD_STAT_RUNNING)
	{
		Sleep(1);
	}
	GetExitCodeThread(m_stThreadSender.hThread, &nExitCode);
	m_stThreadSender.hThread = NULL;

	
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
	{
		m_vClientList.push_back(m_SClient);
	}
	else
		return;

	m_stThreadRecv.bThreadFlag = true;
	m_stThreadRecv.cObjectPointer1 = this;
	m_stThreadRecv.cObjectPointer2 = &m_SClient;
	m_stThreadRecv.nScanInterval = 1;
	m_stThreadRecv.enState = THREAD_STATUS::THREAD_STAT_NONE;
	m_stThreadRecv.hThread = (HANDLE*)_beginthreadex(NULL, 0, THREAD_SERVER_RECV, &m_stThreadRecv, 0, NULL);
	CloseHandle(m_stThreadRecv.hThread);

	m_stThreadSender.bThreadFlag = true;
	m_stThreadSender.cObjectPointer1 = this;
	m_stThreadSender.cObjectPointer2 = &m_SClient;
	m_stThreadSender.nScanInterval = 1;
	m_stThreadSender.enState = THREAD_STATUS::THREAD_STAT_NONE;
	m_stThreadSender.hThread = (HANDLE*)_beginthreadex(NULL, 0, THREAD_SERVER_SENDER, &m_stThreadSender, 0, NULL);
	CloseHandle(m_stThreadSender.hThread);
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
	//char temp[sizeof(CMDMSG)] = { 0, };
	CMDMSG temp;
	
	SOCKET sock = sRecSocket.GetSocket();
	if (sock == INVALID_SOCKET)
		return -1;

	int iStat = recv(sock,(char*)&temp, sizeof(CMDMSG), 0);
	if (iStat == -1)
	{
		if (m_vClientList.size() > 0)
		{
			m_vClientList.remove(sRecSocket);
		}
		return 1;
	}
	else
	{
		for (auto &sock : m_vClientList)
		{
			if (sock.CompareSocket(sRecSocket))
			{
				sock.PushData((CMDMSG)temp);
				break;
			}
		}
		//memcpy(&m_stCmdMsg, temp, sizeof(CMDMSG));
		//
		//WriteLog("[%d:%d:%d][%d:%d:%d][%d:%d:%d]%d,%d,%d : %s",
		//	m_stCmdMsg.uMsg_Type, m_stCmdMsg.uTask_Src, m_stCmdMsg.uTask_Dst,
		//	m_stCmdMsg.uFunID_Src, m_stCmdMsg.uSeqID_Src, m_stCmdMsg.uUnitID_Src,
		//	m_stCmdMsg.uFunID_Dst, m_stCmdMsg.uSeqID_Dst, m_stCmdMsg.uUnitID_Dst,
		//	m_stCmdMsg.uRetStatus, m_stCmdMsg.uMsgSize, m_stCmdMsg.cDummy, m_stCmdMsg.cMsgBuf);
		//
		//switch ((FUNCCMD)m_stCmdMsg.uFunID_Dst)
		//{
		//case FUNCCMD::EFC_TASK_REG:
		//	RegTask(sRecSocket, m_stCmdMsg.uTask_Src);
		//	break;
		//case FUNCCMD::EFC_TASK_ALIVE:
		//	GetAliveClient(m_stCmdMsg);
		//	break;
		//default:
		//	SendMessagePort(m_stCmdMsg);
		//	break;
		//}

		return 0;
	}
	return 0;
}

void ServerClass::MessageProc(SocketDefine sRecSocket)
{
	CMDMSG temp;

	SOCKET sock = sRecSocket.GetSocket();
	if (sock == INVALID_SOCKET)
		return;

	for (auto& sock : m_vClientList)
	{
		if (sock.CompareSocket(sRecSocket))
		{
			if (!sock.IsDataPopable()) break;

			temp = sock.PopData();

			memcpy(&m_stCmdMsg, &temp, sizeof(CMDMSG));
			
			WriteLog("[%d:%d:%d][%d:%d:%d][%d:%d:%d]%d,%d,%d : %s",
				m_stCmdMsg.uMsg_Type, m_stCmdMsg.uTask_Src, m_stCmdMsg.uTask_Dst,
				m_stCmdMsg.uFunID_Src, m_stCmdMsg.uSeqID_Src, m_stCmdMsg.uUnitID_Src,
				m_stCmdMsg.uFunID_Dst, m_stCmdMsg.uSeqID_Dst, m_stCmdMsg.uUnitID_Dst,
				m_stCmdMsg.uRetStatus, m_stCmdMsg.uMsgSize, m_stCmdMsg.cDummy, m_stCmdMsg.cMsgBuf);
			
			switch ((FUNCCMD)m_stCmdMsg.uFunID_Dst)
			{
			case FUNCCMD::EFC_TASK_REG:
				RegTask(sRecSocket, m_stCmdMsg.uTask_Src);
				break;
			case FUNCCMD::EFC_TASK_ALIVE:
				GetAliveClient(m_stCmdMsg);
				break;
			default:
				SendMessagePort(m_stCmdMsg);
				break;
			}

			break;
		}
	}
}

bool ServerClass::GetAliveClient(CMDMSG msg)
{
	bool bRet = false;
	int iStat = 0;
	std::list<SocketDefine>::iterator itl;
	if (m_vClientList.size() == 0)
		return 0;

	for (itl = m_vClientList.begin(); itl != m_vClientList.end(); itl++)
	{
		if (itl->GetIndex() == msg.uTask_Dst)
		{
			//msg.uTask_Dst
			bRet = true;
			break;
		}
	}

	for (itl = m_vClientList.begin(); itl != m_vClientList.end(); itl++)
	{
		if (itl->GetIndex() == msg.uTask_Src)
		{
			msg.uUnitID_Dst = bRet;
			iStat = send(itl->GetSocket(), (char*)&msg, sizeof(CMDMSG), 0);
			break;
		}
	}

	if (iStat == -1)
		bRet = false;

	return bRet;
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

unsigned int ServerClass::THREAD_SERVER_RECV(LPVOID pParam)
{
	THREAD_PARAM* pThreadparam = (THREAD_PARAM*)pParam;
	ServerClass* pDlg = (ServerClass*)pThreadparam->cObjectPointer1;
	SocketDefine Socket = *(SocketDefine*)pThreadparam->cObjectPointer2;

	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
	while (pThreadparam->bThreadFlag)
	{
		Sleep(pThreadparam->nScanInterval);
		pDlg->RecClient(Socket);
	}
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	return 0;
}

unsigned int ServerClass::THREAD_SERVER_SENDER(LPVOID pParam)
{
	THREAD_PARAM* pThreadparam = (THREAD_PARAM*)pParam;
	ServerClass* pDlg = (ServerClass*)pThreadparam->cObjectPointer1;
	SocketDefine Socket = *(SocketDefine*)pThreadparam->cObjectPointer2;

	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
	while (pThreadparam->bThreadFlag)
	{
		Sleep(pThreadparam->nScanInterval);
		//
		pDlg->MessageProc(Socket);
	}
	pThreadparam->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	return 0;
}