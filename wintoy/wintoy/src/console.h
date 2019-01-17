#pragma once
#include <consoleapi.h>
#include <stdio.h>
#pragma warning(disable: 4996)
#ifdef __cplusplus
extern "C" {
#endif

	void start_console()
	{
		AllocConsole();
		freopen("CONOUT$", "w+t", stdout);
	}

	int output(char const* const _Format, ...)
	{
		int _Result;
		va_list _ArgList;
		__crt_va_start(_ArgList, _Format);
		_Result = _vfprintf_l(stdout, _Format, NULL, _ArgList);
		__crt_va_end(_ArgList);
		return _Result;
	}

	void stop_console()
	{
		FreeConsole();
	}

#ifdef __cplusplus
}
#endif