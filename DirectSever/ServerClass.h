#pragma once
#include <string.h>
#include <list>


class ServerClass
{
public:
	ServerClass(int nPort = 20000);
	~ServerClass();

	
	bool IsConnected() { return m_bIsConnected; } // returns connection status
	void StartListenClient(); // Listen to client
	int SendMessagePort(CMDMSG stMsg);
	int RecClient(SocketDefine sRecSocket); // receive message for a particulat socket
	void RunListen();
	void RegPrintFunc(pPrintFunction pFunc) { m_fn_WriteLog = std::move(pFunc); };
	void RegInfoFunc(pPrintFunction pFunc) { m_fn_WriteInfoClient = std::move(pFunc); };
	void RegTask(SocketDefine &sock, int nIndex);
	void MessageProc(SocketDefine sRecSocket);

	std::list<SocketDefine> GetClientList() { return m_vClientList; };

	bool GetAliveClient(CMDMSG msg);
private:
	void WriteLog(char* strLog, ...);
	void WriteClient(char* strLog, ...);
private:
	pPrintFunction m_fn_WriteLog;
	pPrintFunction m_fn_WriteInfoClient;
	bool m_bIsConnected; // true - connected false - not connected
	int m_iServerPort;
	std::list<SocketDefine> m_vClientList;
	SocketDefine m_SClient;
	SOCKET m_SListenClient; // socket listening for client calls
	
	CMDMSG m_stCmdMsg;

	THREAD_PARAM m_stThreadSender;
	THREAD_PARAM m_stThreadRecv;
	THREAD_PARAM m_stThreadListen;

public:
	static unsigned int __stdcall THREAD_SERVER_SENDER(LPVOID pParam);
	static unsigned int __stdcall THREAD_SERVER_RECV(LPVOID pParam);
	static unsigned int __stdcall THREAD_SERVER_LISTEN(LPVOID pParam);
};