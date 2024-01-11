/*
	MailSender.cpp
	프로그램 설명 : 메일슬롯 Sender
					핸들의 상속 확인용. 부모 프로세스
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define SLOT_NAME _T("\\\\.\\mailslot\\mailbox")

int _tmain(int argc, TCHAR *argv[])
{

	HANDLE hMailSlot;
	TCHAR message[50];
	DWORD bytesWritten;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	SetCurrentDirectory(_T("C:\\Procademy\\SystemProgramming\\Code\\Part02\\x64\\Debug")); // 현재 디렉터리 설정
	hMailSlot = CreateFile(
		SLOT_NAME,
		GENERIC_WRITE, // 쓰기 모드
		FILE_SHARE_READ, // 다른 프로세스 읽기 허용
		&sa,
		OPEN_EXISTING, // 있는 거 열기만
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hMailSlot == INVALID_HANDLE_VALUE)
	{
		_fputts(_T("Unable to create mailslot! \n"), stdout);
		return 1;
	}

	_tprintf(_T("Inheritable Handle : %d \n"), hMailSlot);

	FILE *file = _tfopen(_T("InheritableHandle.txt"), _T("wt"));
	_ftprintf(file, _T("%d"), hMailSlot);
	fclose(file);

	TCHAR command[] = _T("MailSender2.exe");

	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);

	CreateProcess(
		NULL, command, NULL, NULL,
		TRUE, // 핸들 테이블 상속 결정
		CREATE_NEW_CONSOLE,
		NULL, NULL,
		&si, &pi
	);

	while (true)
	{
		_fputts(_T("MY CMD > "), stdout);
		_fgetts(message, sizeof(message) / sizeof(TCHAR), stdin);

		if (!WriteFile(hMailSlot, message, _tcslen(message) * sizeof(TCHAR), &bytesWritten, NULL))
		{
			_fputts(_T("Unable to write!"), stdout);
			CloseHandle(hMailSlot);
			return 1;
		}

		if (!_tcscmp(message, _T("exit")))
		{
			_fputts(_T("Good Bye!\n"), stdout);
			break;
		}


		DWORD exitCode;
		GetExitCodeProcess(pi.hProcess, &exitCode);

		_tprintf(_T("%d"), exitCode);
	}

	CloseHandle(hMailSlot);
	return 0;
}