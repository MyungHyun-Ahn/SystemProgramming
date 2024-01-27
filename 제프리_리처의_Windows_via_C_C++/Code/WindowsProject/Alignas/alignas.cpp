// #define  WINDOWS_IGNORE_PACKING_MISMATCH

#include <stdio.h>
#include <windows.h>
#include <process.h>

// #define INTERLOCKED01
// #define INTERLOCKED02

#pragma pack(push, 1)
struct Test
{
	BYTE arr[62];
	int x;
	unsigned long long g_llTest;
};
#pragma pack(pop)

Test g_Test;
long g_bInterLocked = FALSE;


unsigned __stdcall threadProc1(void *arg)
{
	while (true)
	{
#ifdef INTERLOCKED01
		while (InterlockedCompareExchange(&g_bInterLocked, TRUE, FALSE) == TRUE)
		{
			// Sleep(0);
		}
#endif
		// g_Test.x = 0xCCCCCCCC;
		g_Test.g_llTest = 0xAAAAAAAAAAAAAAAA;

#ifdef INTERLOCKED01
		InterlockedCompareExchange(&g_bInterLocked, FALSE, TRUE);
#endif
	}

	return 0;
}

unsigned __stdcall threadProc2(void *arg)
{
	while (true)
	{
#ifdef INTERLOCKED01
		while (InterlockedCompareExchange(&g_bInterLocked, TRUE, FALSE) == TRUE)
		{
			// Sleep(0);
		}
#endif
		// g_Test.x = 0xDDDDDDDD;
		g_Test.g_llTest = 0xDDDDDDDDDDDDDDDD;

#ifdef INTERLOCKED01
		InterlockedCompareExchange(&g_bInterLocked, FALSE, TRUE);
#endif
	}

	return 0;
}



int main()
{	
	g_Test.arr[0] = 0x11;
	g_Test.arr[61] = 0x22;
	g_Test.x = 0x88888888;
	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, &threadProc1, NULL, CREATE_SUSPENDED, NULL);
	if (hThread1 == 0)
		return 1;
	HANDLE hThread2 = (HANDLE)_beginthreadex(NULL, 0, &threadProc2, NULL, CREATE_SUSPENDED, NULL);
	if (hThread2 == 0)
		return 1;

	ResumeThread(hThread1);
	ResumeThread(hThread2);

	while (true)
	{
#ifdef INTERLOCKED02
		while (InterlockedCompareExchange(&g_bInterLocked, TRUE, FALSE) == TRUE)
		{
			Sleep(0);
		}
#endif
		// printf("%x\n", g_Test.x);
		unsigned int low = (unsigned int)g_Test.g_llTest;
		unsigned int high = (unsigned int)(g_Test.g_llTest >> 32);
		if (low != high)
		{
			printf("low != high\n");
		}


		printf("%llx\n", g_Test.g_llTest);

#ifdef INTERLOCKED02
		InterlockedCompareExchange(&g_bInterLocked, FALSE, TRUE);
#endif
	}

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hThread1);
	CloseHandle(hThread2);

	return 0;
}
