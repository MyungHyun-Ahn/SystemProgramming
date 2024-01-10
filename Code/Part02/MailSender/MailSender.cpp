/*
	MailSender.cpp
	프로그램 설명 : 메일슬롯 Sender
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define SLOT_NAME _T("\\\\.\\mailslot\\mailbox")

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hMailSlot;
	TCHAR message[50];
	DWORD bytesWritten;

	hMailSlot = CreateFile(
		SLOT_NAME,
		GENERIC_WRITE, // 쓰기 모드
		FILE_SHARE_READ, // 다른 프로세스 읽기 허용
		NULL,
		OPEN_EXISTING, // 있는 거 열기만
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hMailSlot == INVALID_HANDLE_VALUE)
	{
		_fputts(_T("Unable to create mailslot! \n"), stdout);
		return 1;
	}

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
			_fputts(_T("Good Bye!"), stdout);
			break;
		}
	}

	CloseHandle(hMailSlot);
	return 0;
}