/*
	CreateToolhelp32Snapshot
	ListProcessInfo.cpp
	프로그램 설명 : 현재 실행 중인 프로세스 정보 출력
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <TlHelp32.h>

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return -1;

	// 프로세스 정보 얻기 위한 구조체
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	bool pfRet = Process32First(hProcessSnap, &pe32);

	if (!pfRet)
	{
		DWORD errCode = GetLastError();
		_tprintf(_T("Process32Firest error! %d\n"), errCode);
		CloseHandle(hProcessSnap);
		return -1;
	}

	do 
	{
		_tprintf(_T("%25s %5d \n"), pe32.szExeFile, pe32.th32ProcessID);

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return 0;
}

