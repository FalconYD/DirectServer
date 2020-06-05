#pragma once

class SocketDefine
{
public:
	SocketDefine()
	{
		sockidx = -1;
		socket = 0;
	}
	~SocketDefine()
	{
	}
private:
	int sockidx;
	SOCKET socket;
public:
	bool operator== (const SocketDefine& sock) const { return (sockidx == sock.sockidx);}
	//friend bool operator==(const SocketDefine& x1, const SocketDefine& x2);
	bool CompareSocket(SocketDefine sock) { return (socket == sock.socket); }
	void SetSocket(SOCKET sock)	{ socket = sock;}
	SOCKET GetSocket() const	{ return socket;}
	int GetIndex() const	{ return sockidx;}
	void SetIndex(int nIdx)	{ sockidx = nIdx;}
};

// bool operator==(const SocketDefine& x1, const SocketDefine& x2) 
//{
//	return (x1.sockidx == x2.sockidx);
//}