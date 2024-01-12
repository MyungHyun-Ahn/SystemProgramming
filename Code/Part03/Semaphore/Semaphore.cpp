#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <process.h>


#define NUM_OF_CUSTOMER 50
#define RANGE_MIN 10
#define RANGE_MAX (30 - RANGE_MIN)
#define TABLE_COUNT 3

HANDLE hSemaphore;
DWORD randTimeArr[10];

void TakeMeal(DWORD time)
{
	WaitForSingleObject(hSemaphore, INFINITE);
	_tprintf(_T("Enter Customer %d~ \n"), GetCurrentThreadId());

	_tprintf(_T("Customer %d having launch~ \n"), GetCurrentThreadId());

	Sleep(1000 * time); // 식사 중인 상태를 시뮬레이션 하는 함수

	ReleaseSemaphore(hSemaphore, 1, NULL);
	_tprintf(_T("Out Custom %d~ \n"), GetCurrentThreadId());
}

unsigned int WINAPI ThreadProc(LPVOID lpParam)
{
	TakeMeal((DWORD)lpParam);
	return 0;
}

int _tmain(int argc, TCHAR *argv[])
{
	DWORD dwThreadIDs[NUM_OF_CUSTOMER];
	HANDLE hThreads[NUM_OF_CUSTOMER];

	srand((unsigned)time(NULL));

	// 스레드에게 전달할 random 값 총 50개 생성
	for (int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		randTimeArr[i] = (DWORD)(((double)rand() / (double)RAND_MAX) * RANGE_MAX + RANGE_MIN);
	}

	hSemaphore = CreateSemaphore(NULL, TABLE_COUNT, TABLE_COUNT, NULL);

	if (hSemaphore == NULL)
		_tprintf(_T("CreateSemaphore Error : %d \n"), GetLastError());

	// Customer
	for (int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		hThreads[i] = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void *) randTimeArr[i], CREATE_SUSPENDED, (unsigned *)& dwThreadIDs[i]);

		if (hThreads[i] == NULL)
			return -1;
	}

	for (int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		ResumeThread(hThreads[i]);
	}

	WaitForMultipleObjects(NUM_OF_CUSTOMER, hThreads, TRUE, INFINITE);

	_tprintf(_T("---- END ----\n"));

	for (int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		CloseHandle(hThreads[i]);
	}


	CloseHandle(hSemaphore);

	return 0;
}