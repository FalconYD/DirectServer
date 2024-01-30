#pragma once
#include <string.h>
#include <list>

class ClientSock;

class ServerClass
{
public:
	ServerClass(int nPort = 20000);
	~ServerClass();

	
	bool IsConnected() { return m_bIsConnected; } // returns connection status
	void StartListenClient(); // Listen to client
	void RunListen();
	void RegPrintFunc(pPrintFunction pFunc) { m_fn_WriteLog = std::move(pFunc); };
	void RegInfoFunc(pPrintFunction pFunc) { m_fn_WriteInfoClient = std::move(pFunc); };

	std::list<ClientSock*> GetClientList() { return m_listClients; };

	void RegTask(ClientSock* sock, int nIndex);
	bool GetAliveClient(CMDMSG msg);
	int SendMessagePort(CMDMSG stMsg);

	void WriteLog(char* strLog, ...);
	
	void RemoveClient(ClientSock* clientSock);
	void AddClient(ClientSock* clientSock);

	void SendServerDown();
	
	static unsigned int __stdcall THREAD_SERVER_LISTEN(LPVOID pParam);
private:

	SRWLOCK m_srwClient;

	pPrintFunction m_fn_WriteLog;
	pPrintFunction m_fn_WriteInfoClient;
	bool m_bIsConnected; // true - connected false - not connected
	int m_iServerPort;
	std::list<ClientSock*> m_listClients;
	
	SOCKET m_sockServerListener;

	THREAD_PARAM m_stThreadListen;

	void WriteClient(char* strLog, ...);
};