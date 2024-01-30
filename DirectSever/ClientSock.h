#pragma once
class ServerClass;

class ClientSock
{

public:
	ClientSock(ServerClass* main, int tasknum = -1);
	~ClientSock();
private:
	int tasknum;

	SOCKET socket;
	SRWLOCK srwLock;
	std::queue<CMDMSG> msgque;

	ServerClass* mc_pServer;

	ST_THREAD_PARAM m_stThreadRecv;
	ST_THREAD_PARAM m_stThreadProc;

	static unsigned int _stdcall THREAD_MSGRECV(LPVOID param);
	static unsigned int _stdcall THREAD_MSGPROC(LPVOID param);

	void StartProc();
	void StopProc();

	void PushData(CMDMSG msg);
	bool IsDataPopable();
	CMDMSG PopData();
public:
	//bool operator== (const ClientSock& sock) const { return (tasknum == sock.tasknum); }
	//friend bool operator==(const SocketDefine& x1, const SocketDefine& x2);
	//bool CompareSocket(ClientSock sock);
	void SetSocket(SOCKET sock);
	SOCKET GetSocket() const;
	int GetTaskNum() const;
	void SetTaskNum(int nTaskNum);
	char* GetAddressName();


	void StartRecv();
	void StopRecv();
};

