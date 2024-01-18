#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>


int wmain(int argc, WCHAR *argv[])
{
	int nNumArgs;
	PWSTR *ppArgv = CommandLineToArgvW(GetCommandLineW(), &nNumArgs);

	wprintf(TEXT("Argv[0] = %s\n"), *ppArgv);

	HeapFree(GetProcessHeap(), 0, ppArgv);
	LocalFree(ppArgv); // LocalFree : HeapFree의 래퍼 함수
}