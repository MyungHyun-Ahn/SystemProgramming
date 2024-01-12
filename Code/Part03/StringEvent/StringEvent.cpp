#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <process.h>

unsigned int WINAPI OutputThreadFunction(LPVOID lpParam);
unsigned int WINAPI CountThreadFunction(LPVOID lpParam);

TCHAR string[100];
HANDLE hEvent;

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hThreads[2];
	HANDLE dwThreadIDs[2];

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (hEvent == NULL)
		return -1;

	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, OutputThreadFunction, NULL, 0, (unsigned*)&dwThreadIDs[0]);

	if (hThreads[0] == 0)
		return -1;

	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, CountThreadFunction, NULL, 0, (unsigned*)&dwThreadIDs[1]);

	if (hThreads[1] == 0)
		return -1;

	_fputts(_T("Insert string : "), stdout);
	_fgetts(string, 30, stdin);

	SetEvent(hEvent);

	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	CloseHandle(hEvent);
	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);

	return 0;
}

unsigned int WINAPI OutputThreadFunction(LPVOID lpParam)
{
	WaitForSingleObject(hEvent, INFINITE);

	_fputts(_T("Output String : "), stdout);
	_fputts(string, stdout);

	return 0;
}

unsigned int __stdcall CountThreadFunction(LPVOID lpParam)
{
	WaitForSingleObject(hEvent, INFINITE);

	_tprintf(_T("Output String Length : %d \n"), _tcslen(string) - 1);

	return 0;
}
