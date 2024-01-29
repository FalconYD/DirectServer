#pragma once

#include <WS2tcpip.h>
#include <queue>
#include "define_Protocol.h"

class SocketDefine
{
public:
	SocketDefine()
	{
		sockidx = -1;
		socket = 0;
		InitializeSRWLock(&srwLock);
	}
	~SocketDefine()
	{
	}
private:
	int sockidx;
	SOCKET socket;
	SRWLOCK srwLock;
	std::queue<CMDMSG> msgque;
public:
	bool operator== (const SocketDefine& sock) const { return (sockidx == sock.sockidx);}
	//friend bool operator==(const SocketDefine& x1, const SocketDefine& x2);
	bool CompareSocket(SocketDefine sock) { return (socket == sock.socket); }
	void SetSocket(SOCKET sock)	{ socket = sock;}
	SOCKET GetSocket() const	{ return socket;}
	int GetIndex() const	{ return sockidx;}
	void SetIndex(int nIdx)	{ sockidx = nIdx;}
	char* GetAddressName() 
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

	void PushData(CMDMSG msg)
	{
		AcquireSRWLockExclusive(&srwLock);
		msgque.push(msg);
		ReleaseSRWLockExclusive(&srwLock);
	}

	bool IsDataPopable()
	{
		return msgque.size();
	}

	CMDMSG PopData()
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
};

// bool operator==(const SocketDefine& x1, const SocketDefine& x2) 
//{
//	return (x1.sockidx == x2.sockidx);
//}