/*
	MailSender.cpp
	프로그램 설명 : 메일슬롯 Sender
					핸들의 상속 확인용. 자식 프로세스
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[])
{
	// 64비트에서 핸들은 8바이트
	HANDLE hMailSlot;
	TCHAR message[50];
	DWORD bytesWritten;

	// 핸들을 얻는 코드
	FILE* file = _tfopen(_T("InheritableHandle.txt"), _T("rt"));
	_ftscanf(file, _T("%lld"), &hMailSlot);
	fclose(file);

	_tprintf(_T("Inheritable Handle : %d \n"), hMailSlot);

	// _gettchar();

	while (true)
	{
		_fputts(_T("MY CMD > "), stdout);
		_fgetts(message, sizeof(message) / sizeof(TCHAR), stdin);

		if (!WriteFile(hMailSlot, message, _tcslen(message) * sizeof(TCHAR), &bytesWritten, NULL))
		{
			DWORD err = GetLastError();
			_tprintf(_T("%d"), err);
			_fputts(_T("Unable to write!"), stdout);
			CloseHandle(hMailSlot);
			// _gettchar();
			return 1;
		}

		if (!_tcscmp(message, _T("exit")))
		{
			_fputts(_T("Good Bye!\n"), stdout);
			break;
		}
	}

	CloseHandle(hMailSlot);
	return 0;
}