#pragma once

inline void _in_fn_Uni2Multi(const wchar_t* InputString, char* OutString)
{
	if (wcslen(InputString) > 0 && wcslen(InputString) < LOG_TEXT_MAX_SIZE)
	{
		int len = WideCharToMultiByte(CP_ACP, 0, InputString, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, InputString, -1, OutString, len, NULL, NULL);
	}
	else
	{
		OutputDebugString(_T("InputString Length Error."));
	}
}

inline void _in_fn_Multi2Uni(const char* InputString, wchar_t* OutString)
{
	if (strlen(InputString) > 0 && strlen(InputString) < LOG_TEXT_MAX_SIZE)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, InputString, (int)strlen(InputString), NULL, NULL);
		MultiByteToWideChar(CP_ACP, 0, InputString, (int)strlen(InputString), OutString, len);
	}
	else
	{
		OutputDebugString(_T("InputString Length Error."));
	}
}

typedef std::function<void(char*)> pPrintFunction;