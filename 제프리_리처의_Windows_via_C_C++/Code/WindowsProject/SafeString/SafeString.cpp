#include <crtdbg.h>
#include <stdio.h>
#include <windows.h>
#include <TCHAR.h>
#include <corecrt_wstring.h>
#include <strsafe.h>

// 에러 기록 함수
void InvalidParameterHandler(PCTSTR expression, PCTSTR func, PCTSTR file, unsigned int line, uintptr_t pReserved)
{
	_tprintf(TEXT("Invalid parameter detected in function %s.\n") TEXT("File : %s \nLine : %d\n"), func, file, line);
	_tprintf(TEXT("Expression : %s\n"), expression);
}

int _tmain()
{
	// ASSERT 다이얼로그 Off
	_CrtSetReportMode(_CRT_ASSERT, 0);
	_invalid_parameter_handler oldHandler;
	_invalid_parameter_handler newHandler;
	newHandler = InvalidParameterHandler;
	oldHandler = _set_invalid_parameter_handler(newHandler);

	TCHAR szBefore[5] = {
		TEXT('B'), TEXT('B'), TEXT('B'), TEXT('B'), TEXT('\0')
	};

	TCHAR szBuffer[10] = {
		TEXT('-'), TEXT('-'), TEXT('-'), TEXT('-'), TEXT('-'),
		TEXT('-'), TEXT('-'), TEXT('-'), TEXT('-'), TEXT('\0')
	};

	TCHAR szAfter[5] = {
		TEXT('A'), TEXT('A'), TEXT('A'), TEXT('A'), TEXT('\0')
	};

	errno_t result = _tcscpy_s(szBuffer, _countof(szBuffer), TEXT("0123456789"));

	return 0;
}