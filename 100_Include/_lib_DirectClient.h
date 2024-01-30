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

/**
@brief	Client Alive 체크시 cMsgBuf 데이터 타입.
@remark
 - 해당 구조체 포인터로 캐스팅 해서 사용.
  ALIVESTATE* state = (ALIVESTATE*)cmdmsg.cMsgBuf;
 - 포인터 접근 인덱스는 MSG_BUFF_SIZE / sizeof(ALIVESTATE) 보다 작아야함.
 - 포인터 접근 인덱스는 tasknum.
@author	선경규(Kyeong Kyu - Seon)
@date	2024/1/30  18:20
*/
typedef struct ST_ALIVE_STATE
{
	unsigned short tasknum;
	unsigned short taskstate;
}ALIVESTATE;

/**
@brief	Msg_Type 정의.
@remark
 -
@author	선경규(Kyeong Kyu - Seon)
@date	2024/1/30  15:16
*/
typedef enum EN_MSG_TYPE
{
	EMT_BROADCAST = 0, /// 모든 클라이언트에 전송 코드.
	EMT_TARGET, /// 특정 클라이언트에 전송 코드.
	EMT_TASKREG, /// 클라이언트 번호 등록 코드.
	EMT_TASKALIVE, /// 다른 클라이언트 상태 확인 코드. RetStatus가 1이면 Alive, 0이면 Dead
	EMT_SERVERDOWN		  /// 서버 종료시 전송되는 코드. RetStatus 1을 동시 체크.
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
