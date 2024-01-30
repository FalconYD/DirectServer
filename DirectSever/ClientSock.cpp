#include "stdafx.h"
#include "ServerClass.h"
#include "ClientSock.h"

ClientSock::ClientSock(ServerClass* main, int tsk)
{
	tasknum = -1;
	socket = 0;
	InitializeSRWLock(&srwLock);
	tasknum = tsk;

	mc_pServer = main;
}

ClientSock::~ClientSock()
{
	StopRecv();

	AcquireSRWLockExclusive(&srwLock);
	for (int i = 0; i < msgque.size(); i++)
	{
		msgque.pop();
	}
	ReleaseSRWLockExclusive(&srwLock);
}

//bool ClientSock::CompareSocket(ClientSock sock) 
//{ 
//	return (socket == sock.socket); 
//}

void ClientSock::SetSocket(SOCKET sock) 
{ 
	socket = sock; 
}

SOCKET ClientSock::GetSocket() const 
{ 
	return socket; 
}

int ClientSock::GetTaskNum() const 
{ 
	return tasknum;
}

void ClientSock::SetTaskNum(int nTaskNum) 
{ 
	tasknum = nTaskNum;
}

char* ClientSock::GetAddressName()
{
	SOCKADDR_IN clientAddress;
	int nLen = sizeof(struct sockaddr_in);
	getpeername(socket, (struct sockaddr*)&clientAddress, &nLen);
	char bufIP[32] = { 0, };
	char buf[32] = { 0, };
	inet_ntop(AF_INET, &clientAddress.sin_addr, bufIP, 32);
	sprintf_s(buf, "%s : %d", bufIP, clientAddress.sin_port);
	return buf;
}

void ClientSock::PushData(CMDMSG msg)
{
	AcquireSRWLockExclusive(&srwLock);
	msgque.push(msg);
	ReleaseSRWLockExclusive(&srwLock);
}

bool ClientSock::IsDataPopable()
{
	return msgque.size();
}

CMDMSG ClientSock::PopData()
{
	CMDMSG msg;
	if (msgque.size() > 0)
	{
		AcquireSRWLockExclusive(&srwLock);
		msg = msgque.front();
		msgque.pop();
		ReleaseSRWLockExclusive(&srwLock);
	}
	return msg;
}

void ClientSock::StartRecv()
{
	m_stThreadRecv.bThreadFlag = true;
	m_stThreadRecv.cObjectPointer1 = this;
	m_stThreadRecv.cObjectPointer2 = nullptr;
	m_stThreadRecv.nScanInterval = 1;
	m_stThreadRecv.enState = THREAD_STATUS::THREAD_STAT_NONE;
	m_stThreadRecv.hThread = (HANDLE)_beginthreadex(nullptr, 0, THREAD_MSGRECV, &m_stThreadRecv, 0, nullptr);

}

void ClientSock::StopRecv()
{
	if (m_stThreadRecv.bThreadFlag)
	{
		m_stThreadRecv.bThreadFlag = false;
		WaitForSingleObject(m_stThreadRecv.hThread, INFINITE);
		CloseHandle(m_stThreadRecv.hThread);
		m_stThreadRecv.hThread = nullptr;
	}

}

void ClientSock::StartProc()
{
	m_stThreadProc.bThreadFlag = true;
	m_stThreadProc.cObjectPointer1 = this;
	m_stThreadProc.cObjectPointer2 = nullptr;
	m_stThreadProc.nScanInterval = 1;
	m_stThreadProc.enState = THREAD_STATUS::THREAD_STAT_NONE;
	m_stThreadProc.hThread = (HANDLE)_beginthreadex(nullptr, 0, THREAD_MSGPROC, &m_stThreadRecv, 0, nullptr);
}

void ClientSock::StopProc()
{
	if (m_stThreadProc.bThreadFlag)
	{
		m_stThreadProc.bThreadFlag = false;
		WaitForSingleObject(m_stThreadProc.hThread, INFINITE);
		CloseHandle(m_stThreadProc.hThread);
		m_stThreadProc.hThread = nullptr;
	}
}

unsigned int ClientSock::THREAD_MSGRECV(LPVOID param)
{
	THREAD_PARAM* pParam = reinterpret_cast<THREAD_PARAM*>(param);
	ClientSock* pMain = reinterpret_cast<ClientSock*>(pParam->cObjectPointer1);

	pParam->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;

	SOCKET sock = pMain->GetSocket();

	if (sock == INVALID_SOCKET) // socket error.
		pParam->bThreadFlag = false;
	else
	{
		pParam->bThreadFlag = true;
		pMain->StartProc();
	}

	pParam->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
	CMDMSG temp;
	while (pParam->bThreadFlag)
	{
		Sleep(pParam->nScanInterval);

		int iStat = recv(sock, (char*)&temp, sizeof(CMDMSG), MSG_WAITALL);
		if (iStat == sizeof(CMDMSG))
		{
			pMain->PushData(temp);
		}
		else if (iStat == -1)
		{
			pMain->mc_pServer->RemoveClient(pMain);
			break;
		}
	}
	pParam->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;

	pMain->StopProc();

	_endthreadex(0);
	return 0;
}

unsigned int ClientSock::THREAD_MSGPROC(LPVOID param)
{
	THREAD_PARAM* pParam = reinterpret_cast<THREAD_PARAM*>(param);
	ClientSock* pMain = reinterpret_cast<ClientSock*>(pParam->cObjectPointer1);

	pParam->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;

	SOCKET sock = pMain->GetSocket();

	if (sock == INVALID_SOCKET) pParam->bThreadFlag = false;
	else						pParam->bThreadFlag = true;

	pParam->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
	CMDMSG temp;
	while (pParam->bThreadFlag)
	{
		if (!pMain->IsDataPopable())
		{
			Sleep(pParam->nScanInterval);
			continue;
		}

		temp = pMain->PopData();

		//memcpy(&m_stCmdMsg, &temp, sizeof(CMDMSG));

		pMain->mc_pServer->WriteLog("[%d:%d:%d][%d:%d:%d][%d:%d:%d]%d,%d,%d : %s",
			temp.uMsg_Type, temp.uTask_Src, temp.uTask_Dst,
			temp.uFunID_Src, temp.uSeqID_Src, temp.uUnitID_Src,
			temp.uFunID_Dst, temp.uSeqID_Dst, temp.uUnitID_Dst,
			temp.uRetStatus, temp.uMsgSize, temp.cDummy, temp.cMsgBuf);

		switch ((MSGTYPE)temp.uMsg_Type)
		{
		case MSGTYPE::EMT_TASKREG:
			pMain->mc_pServer->RegTask(pMain, temp.uTask_Src);
			break;
		case MSGTYPE::EMT_TASKALIVE:
			pMain->mc_pServer->GetAliveClient(temp);
			break;
		default:
			pMain->mc_pServer->SendMessagePort(temp);
			break;
		}
	}
	pParam->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	_endthreadex(0);
	return 0;
}