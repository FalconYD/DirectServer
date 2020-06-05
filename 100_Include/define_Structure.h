#pragma once

typedef enum EN_THREAD_STATUS
{
	THREAD_STAT_NONE = 0,
	THREAD_STAT_ACTIVE,
	THREAD_STAT_RUNNING,
	THREAD_STAT_COMPLETE
}THREAD_STATUS;

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
		nThreadIndex = 0;
		bThreadFlag = false;
		hThread = nullptr;
		enState = THREAD_STAT_NONE;
	}
}THREAD_PARAM;
