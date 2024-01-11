/*
	MailSender.cpp
	���α׷� ���� : ���Ͻ��� Sender
					�ڵ��� ��� Ȯ�ο�. �θ� ���μ���
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

	SetCurrentDirectory(_T("C:\\Procademy\\SystemProgramming\\Code\\Part02\\x64\\Debug")); // ���� ���͸� ����
	hMailSlot = CreateFile(
		SLOT_NAME,
		GENERIC_WRITE, // ���� ���
		FILE_SHARE_READ, // �ٸ� ���μ��� �б� ���
		&sa,
		OPEN_EXISTING, // �ִ� �� ���⸸
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
		TRUE, // �ڵ� ���̺� ��� ����
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