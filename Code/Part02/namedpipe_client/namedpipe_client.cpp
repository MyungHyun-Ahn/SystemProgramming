/*
	namedpipe_client.cpp
	프로그램 설명 : 이름있는 파이프 클라이언트
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define BUF_SIZE 1024

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hPipe;
	TCHAR readDataBuf[BUF_SIZE + 1];
	LPTSTR pipeName = _T("\\\\.\\pipe\\simple_pipe");

	while (true)
	{
		hPipe = CreateFile(
			pipeName,
			GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, 0, NULL
		);

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(_T("Could not open pipe \n"));
			return 0;
		}
	}

	DWORD pipeMode = PIPE_READMODE_BYTE | PIPE_WAIT;
	BOOL isSuccess = SetNamedPipeHandleState(hPipe, &pipeMode, NULL, NULL);

	if (!isSuccess)
	{
		_tprintf(_T("SetNamedPipeHandleState failed \n"));
		return 0;
	}

	LPCTSTR fileName = _T("news.txt");
	DWORD bytesWritten = 0;

	isSuccess = WriteFile(hPipe, fileName, (_tcslen(fileName) + 1) * sizeof(TCHAR), &bytesWritten, NULL);

	if (!isSuccess)
	{
		_tprintf(_T("WriteFile failed %d\n"), GetLastError());
		return 0;
	}

	DWORD bytesRead = 0;
	while (true)
	{
		isSuccess = ReadFile(hPipe, readDataBuf, BUF_SIZE * sizeof(TCHAR), &bytesRead, NULL);

		if (!isSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

		readDataBuf[bytesRead] = 0;
		_tprintf(_T("%s \n"), readDataBuf);
	}

	CloseHandle(hPipe);

	return 0;
}