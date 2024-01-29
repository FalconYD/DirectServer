#define  DLLEXPORT
#include <string>
#include <process.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")
#include "_lib_DirectClient.h"

/**
@enum	Thread Status	
@brief	Thread�� ���¸� ��Ÿ���� ������ ������
@author	�����(Kyeong Kyu - Seon)
@date	2019/10/8  17:19
*/
typedef enum EN_THREAD_STATUS
{
	THREAD_STAT_NONE = 0,
	THREAD_STAT_ACTIVE,
	THREAD_STAT_RUNNING,
	THREAD_STAT_COMPLETE
}THREAD_STATUS;

/**
@struct	Thread ���� ����ü
@brief	Thread ������ ���� ����ü
@author	�����(Kyeong Kyu - Seon)
@date	2019/10/8  17:20
*/
typedef struct ST_THREAD_PARAM
{
	void*	cObjectPointer1;
	void*	cObjectPointer2;
	int		nScanInterval;
	int		nThreadIndex;
	bool	bThreadFlag;
	HANDLE  hThread;
	THREAD_STATUS enState;

	ST_THREAD_PARAM()
	{
		cObjectPointer1 = nullptr;
		cObjectPointer2 = nullptr;
		nScanInterval = 300;
		bThreadFlag = false;
		hThread = nullptr;
		enState = THREAD_STAT_NONE;
	}
}THREAD_PARAM;

typedef void(*pCallback)(CMDMSG);		/// Recive Callback Define
pCallback		g_callback = nullptr;	/// Recive CallBack Function Pointer
THREAD_PARAM	g_ThreadParam;			/// Thread ������ ���� ����ü ����
bool			g_bIsConnected;			/// true - connected false - not connected
SOCKET			g_sock;					/// Client ����
CMDMSG			g_stMsg;				/// ���� �������̽� Command Message ����ü

int RecvMessagePort();					
static unsigned int __stdcall THREAD_MESSAGE_RECV(LPVOID pParam);

/**	
@brief	���� ����
@return	void
@param	IP String, Port Number, Callback Pointer
@remark	
 - ���� �ʱ�ȭ
 - ���� ����
 - ���� �ݹ� ���
 - ���� ������ ����
@author	�����(Kyeong Kyu - Seon)
@date	2019/10/8  17:03
*/
MYDLLTYPE int _lib_Connect(const char* sIpAddress, int iPort, void* callback)
{
	if (g_bIsConnected)
	{
		return 1;
	}
	unsigned int addr;
	struct sockaddr_in server;

	WSADATA wsaData;

	//int wsaret = WSAStartup(0x101, &wsaData);
	int wsaret = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (wsaret != 0)
	{
		return -1;
	}

	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET)
		return -1;

	inet_pton(AF_INET, sIpAddress, &addr);

  	server.sin_family = AF_INET;
 	server.sin_port = htons(iPort);
 	server.sin_addr.s_addr = addr;
	if (connect(g_sock, (struct sockaddr*)&server, sizeof(server)))
	{
		closesocket(g_sock);
		return -1;
	}
	g_bIsConnected = true;
	
	// CallBack ���
	g_callback = (pCallback)callback;

	g_ThreadParam.bThreadFlag = true;
	g_ThreadParam.nScanInterval = 1;
	g_ThreadParam.enState = THREAD_STATUS::THREAD_STAT_NONE;
	HANDLE hThread = (HANDLE*)_beginthreadex(NULL, 0, THREAD_MESSAGE_RECV, &g_ThreadParam, 0, NULL);
	g_ThreadParam.hThread = hThread;
	CloseHandle(hThread);

	return 0;
}

/**	
@brief	 ���� ���� ����
@return	void
@param	void
@remark	
 - ���� ������ ����
 - ���� ����
@author	�����(Kyeong Kyu - Seon)
@date	2019/10/8  17:02
*/
MYDLLTYPE void _lib_Disconnect()
{
	DWORD nExitCode = NULL;
	GetExitCodeThread(g_ThreadParam.hThread, &nExitCode);
	TerminateThread(g_ThreadParam.hThread, nExitCode);
	if (g_bIsConnected)
	{
		closesocket(g_sock);
		g_bIsConnected = false;
	}
	return;
}

/**	
@brief	������ ����
@return	������ 0, ���н� 1
@param	Command Message ����ü
@remark	
 - 
@author	�����(Kyeong Kyu - Seon)
@date	2019/10/8  17:01
*/
MYDLLTYPE int _lib_SendData(CMDMSG cmd)
{
	int iStat = 0;

	iStat = send(g_sock, (char*)&cmd, sizeof(CMDMSG), 0);
	if (iStat == -1)
		return 1;

	return 0;
}

/**	
@brief	������ ����
@return	������ 0, ���н� 1
@param	void
@remark	
 - ������ ���� �� �ݹ� ����
@author	�����(Kyeong Kyu - Seon)
@date	2019/10/8  16:53
*/
int RecvMessagePort()
{
	char recvData[sizeof(CMDMSG)];
	int iStat = 0;

	iStat = recv(g_sock, recvData, sizeof(CMDMSG), 0);
	if (iStat == -1)
		return 1;

	if (g_callback != nullptr)
	{
		memcpy(&g_stMsg, recvData, sizeof(CMDMSG));
		g_callback(g_stMsg);
	}
	return 0;
}

/**	
@brief	Message Recv Thread
@return	
@param	
@remark	
 - Recv�� ������ ��� ����
 - ���н� ������ ����
@author	�����(Kyeong Kyu - Seon)
@date	2019/10/8  16:48
*/
unsigned int __stdcall THREAD_MESSAGE_RECV(LPVOID pParam)
{
	THREAD_PARAM* stThread = (THREAD_PARAM*)pParam;
	stThread->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;
	while (stThread->bThreadFlag)
	{
		stThread->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
		Sleep(stThread->nScanInterval);
		if (RecvMessagePort())
			break;
	}
	stThread->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	_endthreadex(0);
	return 0;
}