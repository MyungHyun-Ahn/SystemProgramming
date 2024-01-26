#include <stdio.h>
#include <windows.h>
#include <process.h>

const int COUNT = 1000;
int g_nSum = 0;
CRITICAL_SECTION g_cs;

DWORD WINAPI FirstThread(PVOID pvParam)
{
	EnterCriticalSection(&g_cs);
	g_nSum = 0;
	for (int n = 1; n <= COUNT; n++)
	{
		g_nSum += n;
	}

	LeaveCriticalSection(&g_cs);
	return g_nSum;
}


DWORD WINAPI SecondThread(PVOID pvParam)
{
	EnterCriticalSection(&g_cs);
	g_nSum = 0;
	for (int n = 1; n <= COUNT; n++)
	{
		g_nSum += n;
	}

	LeaveCriticalSection(&g_cs);
	return g_nSum;
}

int main()
{
	InitializeCriticalSection(&g_cs);


	while (true)
	{
		HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)&FirstThread, NULL, CREATE_SUSPENDED, NULL);
		if (hThread1 == 0)
			return 1;
		HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)&SecondThread, NULL, CREATE_SUSPENDED, NULL);
		if (hThread2 == 0)
			return 1;

		ResumeThread(hThread1);
		ResumeThread(hThread2);

		WaitForSingleObject(hThread1, INFINITE);
		WaitForSingleObject(hThread2, INFINITE);

		DWORD t1;
		DWORD t2;

		GetExitCodeThread(hThread1, &t1);
		GetExitCodeThread(hThread2, &t2);
		printf("%d, %d\n", t1, t2);
		CloseHandle(hThread1);
		CloseHandle(hThread2);
	}
	DeleteCriticalSection(&g_cs);
	return 0;
}