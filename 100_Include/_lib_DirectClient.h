#pragma once
#ifdef DLLEXPORT
#define MYDLLTYPE extern "C" __declspec(dllexport)
#else
#define MYDLLTYPE extern "C" __declspec(dllimport)
#endif
#include <windows.h>

const int MSG_BUFF_SIZE = 4096; /// Message Buffer Max Size

/**
@struct	Command ����ü
@brief	Command Protocall
@remark	
 - 
@author	�����(Kyeong Kyu - Seon)
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
MYDLLTYPE int _lib_Connect(const char* sIpAddress, int iPort, void* callback);

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
MYDLLTYPE void _lib_Disconnect();

/**
@brief	������ ����
@return	������ 0, ���н� 1
@param	Command Message ����ü
@remark
-
@author	�����(Kyeong Kyu - Seon)
@date	2019/10/8  17:01
*/
MYDLLTYPE int _lib_SendData(CMDMSG cmd);
