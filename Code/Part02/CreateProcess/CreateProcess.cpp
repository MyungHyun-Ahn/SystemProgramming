/*
	CreateProcess.cpp
	프로그램 설명 : 덧셈 프로세스를 생성
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define DIR_LEN MAX_PATH+1

int _tmain(int argc, TCHAR *argv[])
{
	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE;
	si.dwX = 100;
	si.dwY = 200;
	si.dwXSize = 300;
	si.dwYSize = 200;
	si.lpTitle = _T("I am a boy!");
	TCHAR command[] = _T("AdderProcess.exe 10 20");
	TCHAR cDir[DIR_LEN];
	BOOL state;

	GetCurrentDirectory(DIR_LEN, cDir);
	_fputts(cDir, stdout);
	_fputts(_T("\n"), stdout);

	SetCurrentDirectory(_T("C:\\Procademy\\SystemProgramming\\Code\\Part02\\x64\\Debug")); // 현재 디렉터리 설정

	GetCurrentDirectory(DIR_LEN, cDir);
	_fputts(cDir, stdout);
	_fputts(_T("\n"), stdout);

	state = CreateProcess( // 프로세스 생성
		NULL,				// 실행파일 이름
		command,			// main 함수에 전달될 문자열
		NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE,
		NULL, NULL, &si, &pi
	); // CreateProcess

	if (state != 0)
		_fputts(_T("Creation OK! \n"), stdout);
	else
		_fputts(_T("Creation Error! \n"), stdout);

	return 0;
}