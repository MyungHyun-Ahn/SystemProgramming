/*
	anonymous_pipe
	���α׷� ���� : �̸����� �������� �⺻ ���� �ľ�
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hReadPipe;
	HANDLE hWritePipe;

	TCHAR sendString[] = _T("anonymous pipe");
	TCHAR recvString[100];

	DWORD bytesWritten;
	DWORD bytesRead;

	CreatePipe(&hReadPipe, &hWritePipe, NULL, 0);

	WriteFile(
		hWritePipe, sendString, lstrlen(sendString) * sizeof(TCHAR), &bytesWritten, NULL
	);

	_tprintf(_T("stinrg send : %s \n"), sendString);

	ReadFile(
		hReadPipe, recvString, bytesWritten, &bytesRead, NULL
	);

	recvString[bytesRead / sizeof(TCHAR)] = 0;
	_tprintf(_T("stinrg recv : %s \n"), recvString);

	CloseHandle(hReadPipe);
	CloseHandle(hWritePipe);

	return 0;
}