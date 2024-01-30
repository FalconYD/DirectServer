#define  DLLEXPORT
#include <string>
#include <process.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")
#include "_lib_DirectClient.h"

/**
@enum	Thread Status	
@brief	Thread의 상태를 나타내는 열거형 데이터
@author	선경규(Kyeong Kyu - Seon)
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
@struct	Thread 관리 구조체
@brief	Thread 관리를 위한 구조체
@author	선경규(Kyeong Kyu - Seon)
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
THREAD_PARAM	g_ThreadParam;			/// Thread 관리를 위한 구조체 선언
bool			g_bIsConnected;			/// true - connected false - not connected
SOCKET			g_sock;					/// Client 소켓
CMDMSG			g_stMsg;				/// 전송 인터페이스 Command Message 구조체

int RecvMessagePort();					
static unsigned int __stdcall THREAD_MESSAGE_RECV(LPVOID pParam);

MYDLLTYPE int _lib_Connect(const char* sIpAddress, int iPort, void* callback)
{
	if (g_bIsConnected)
	{
		//int iState = send(g_sock, "p", 1, 0);
		//return iState == 0 ? 1 : -1;
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
	
	// CallBack 등록
	g_callback = (pCallback)callback;

	g_ThreadParam.bThreadFlag = true;
	g_ThreadParam.nScanInterval = 1;
	g_ThreadParam.enState = THREAD_STATUS::THREAD_STAT_NONE;
	HANDLE hThread = (HANDLE*)_beginthreadex(NULL, 0, THREAD_MESSAGE_RECV, &g_ThreadParam, 0, NULL);
	g_ThreadParam.hThread = hThread;
	CloseHandle(hThread);

	return 0;
}

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

MYDLLTYPE int _lib_SendData(CMDMSG cmd)
{
	int iStat = 0;

	iStat = send(g_sock, (char*)&cmd, sizeof(CMDMSG), 0);
	if (iStat == -1)
		return 1;

	return 0;
}

/**	
@brief	데이터 수신
@return	성공시 0, 실패시 1
@param	void
@remark	
 - 데이터 수신 및 콜백 실행
@author	선경규(Kyeong Kyu - Seon)
@date	2019/10/8  16:53
*/
int RecvMessagePort()
{
	int iStat = 0;

	iStat = recv(g_sock, (char*)&g_stMsg, sizeof(CMDMSG), MSG_WAITALL);
	if (iStat == -1) // fail
		return 1;	

	switch (g_stMsg.uMsg_Type)
	{
	case (int)MSGTYPE::EMT_SERVERDOWN:
		if (g_stMsg.uRetStatus == 1)
			_lib_Disconnect();
		break;
	case (int)MSGTYPE::EMT_TASKALIVE:
		g_stMsg.uRetStatus = 1;
		_lib_SendData(g_stMsg);
		break;
	}

	if (g_callback != nullptr)
	{
		g_callback(g_stMsg);
	}
	return 0;
}

/**	
@brief	Message Recv Thread
@return	
@param	
@remark	
 - Recv가 성공시 계속 동작
 - 실패시 스레드 종료
@author	선경규(Kyeong Kyu - Seon)
@date	2019/10/8  16:48
*/
unsigned int __stdcall THREAD_MESSAGE_RECV(LPVOID pParam)
{
	THREAD_PARAM* stThread = (THREAD_PARAM*)pParam;
	stThread->enState = THREAD_STATUS::THREAD_STAT_ACTIVE;
	stThread->enState = THREAD_STATUS::THREAD_STAT_RUNNING;
	
	while (stThread->bThreadFlag)
	{
		
		if (RecvMessagePort())
			break;
		else
			Sleep(stThread->nScanInterval);
	}

	stThread->enState = THREAD_STATUS::THREAD_STAT_COMPLETE;
	_endthreadex(0);
	return 0;
}