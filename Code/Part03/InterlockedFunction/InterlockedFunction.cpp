/*
	InterlockedFunction.cpp
	프로그램 설명 : 임계영역의 동기화
*/

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>

#define NUM_OF_GATE 6

LONG gTotalCount = 0;

unsigned int WINAPI ThreadProc(LPVOID lpParam)
{
	for (DWORD i = 0; i < 1000; i++)
	{
		InterlockedIncrement(&gTotalCount);
	}

	return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
	DWORD dwThreadID[NUM_OF_GATE];
	HANDLE hThread[NUM_OF_GATE];

	for (DWORD i = 0; i < NUM_OF_GATE; i++)
	{
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, CREATE_SUSPENDED, (unsigned*)&dwThreadID[i]);

		if (hThread[i] == NULL)
		{
			_tprintf(_T("Thread Creation Fault! \n"));
			return -1;
		}
	}

	for (DWORD i = 0; i < NUM_OF_GATE; i++)
	{
		ResumeThread(hThread[i]);
	}

	WaitForMultipleObjects(NUM_OF_GATE, hThread, TRUE, INFINITE);

	_tprintf(_T("total count : %d \n"), gTotalCount);

	for (DWORD i = 0; i < NUM_OF_GATE; i++)
	{
		CloseHandle(hThread[i]);
	}

	return 0;
}