/*
	EnvChild.cpp
	프로그램 설명 : 환경변수 참조하는 프로세스
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define BUF_SIZE 1024

int _tmain(int argc, TCHAR* argv[])
{
	TCHAR value[BUF_SIZE];

	if (GetEnvironmentVariable(_T("Good"), value, BUF_SIZE) > 0)
		_tprintf(_T("[%s = %s]\n"), _T("Good"), value);

	if (GetEnvironmentVariable(_T("Hey"), value, BUF_SIZE) > 0)
		_tprintf(_T("[%s = %s]\n"), _T("Hey"), value);

	if (GetEnvironmentVariable(_T("Big"), value, BUF_SIZE) > 0)
		_tprintf(_T("[%s = %s]\n"), _T("Big"), value);

	Sleep(10000);

	return 0;
}