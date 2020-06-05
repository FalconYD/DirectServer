#pragma once
#ifdef DLLEXPORT
#define MYDLLTYPE extern "C" __declspec(dllexport)
#else
#define MYDLLTYPE extern "C" __declspec(dllimport)
#endif
#include <windows.h>

const int MSG_BUFF_SIZE = 4096; /// Message Buffer Max Size

/**
@struct	Command 구조체
@brief	Command Protocall
@remark	
 - 
@author	선경규(Kyeong Kyu - Seon)
@date	2019/10/8  17:15
*/
typedef struct ST_COMMAND_MESSAGE
{
	unsigned short	uMsg_Type;				/// Message Type
	unsigned short	uTask_Src;				/// Source Task
	unsigned short	uTask_Dst;				/// Destination Task

	unsigned short	uFunID_Src;				/// Src Function
	unsigned short	uSeqID_Src;				/// Src Sequence
	unsigned short	uUnitID_Src;			/// Src Unit

	unsigned short	uFunID_Dst;				/// Dst Function
	unsigned short	uSeqID_Dst;				/// Dst Sequence
	unsigned short	uUnitID_Dst;			/// Dst Unit

	unsigned short	uRetStatus;				/// Return Status
	unsigned short	uMsgSize;				/// Data Size
	unsigned char	cMsgBuf[MSG_BUFF_SIZE];	/// Data
	unsigned short	cDummy;					/// dummy
	ST_COMMAND_MESSAGE()
	{
		memset(this, 0x00, sizeof(ST_COMMAND_MESSAGE));
		cDummy = '\0';
	}
}CMDMSG;

typedef enum EN_MSG_TYPE
{
	EMT_BROADCAST = 0,
	EMT_TARGET
} MSGTYPE;

/**
@brief	소켓 연결
@return	void
@param	IP String, Port Number, Callback Pointer
@remark
- 윈속 초기화
- 소켓 연결
- 수신 콜백 등록
- 수신 스레드 실행
@author	선경규(Kyeong Kyu - Seon)
@date	2019/10/8  17:03
*/
MYDLLTYPE int _lib_Connect(const char* sIpAddress, int iPort, void* callback);

/**
@brief	 소켓 연결 종료
@return	void
@param	void
@remark
- 수신 쓰레드 종료
- 소켓 종료
@author	선경규(Kyeong Kyu - Seon)
@date	2019/10/8  17:02
*/
MYDLLTYPE void _lib_Disconnect();

/**
@brief	데이터 전송
@return	성공시 0, 실패시 1
@param	Command Message 구조체
@remark
-
@author	선경규(Kyeong Kyu - Seon)
@date	2019/10/8  17:01
*/
MYDLLTYPE int _lib_SendData(CMDMSG cmd);
