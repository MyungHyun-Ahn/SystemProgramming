/*
	MailSender.cpp
	���α׷� ���� : ���Ͻ��� Sender
					�ڵ��� ��� Ȯ�ο�. �ڽ� ���μ���
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[])
{
	// 64��Ʈ���� �ڵ��� 8����Ʈ
	HANDLE hMailSlot;
	TCHAR message[50];
	DWORD bytesWritten;

	// �ڵ��� ��� �ڵ�
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