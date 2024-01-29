#pragma once

typedef struct ST_COMMAND_MESSAGE
{
	unsigned short	uMsg_Type;
	unsigned short	uTask_Src;
	unsigned short	uTask_Dst;

	unsigned short	uFunID_Src;
	unsigned short	uSeqID_Src;
	unsigned short	uUnitID_Src;

	unsigned short	uFunID_Dst;
	unsigned short	uSeqID_Dst;
	unsigned short	uUnitID_Dst;

	unsigned short	uRetStatus;
	unsigned short	uMsgSize;
	unsigned char	cMsgBuf[MSG_BUFF_SIZE];
	unsigned short	cDummy;
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

typedef enum EN_FUNC_CMD
{
	EFC_TASK_REG = 100,
	EFC_TASK_ALIVE
}FUNCCMD;