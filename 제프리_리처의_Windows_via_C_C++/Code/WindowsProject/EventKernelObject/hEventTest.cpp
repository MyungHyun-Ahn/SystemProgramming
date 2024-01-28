// 이벤트 커널 오브젝트 스레드 동기화

#include <stdio.h>
#include <Windows.h>
#include <process.h>

#define MANUAL_RESET
// #define AUTO_RESET

DWORD g_Sum = 0;

HANDLE g_EventHandle;

void __stdcall threadProc1(void)
{
	WaitForSingleObject(g_EventHandle, INFINITE);
	ResetEvent(g_EventHandle);

	for (int i = 0; i < 100000; i++)
	{
		g_Sum++;
	}

	SetEvent(g_EventHandle);
}

void __stdcall threadProc2(void)
{
	WaitForSingleObject(g_EventHandle, INFINITE);
	ResetEvent(g_EventHandle);

	for (int i = 0; i < 100000; i++)
	{
		g_Sum++;
	}

	SetEvent(g_EventHandle);
}


int main()
{
#ifdef AUTO_RESET
	g_EventHandle = CreateEvent(NULL, FALSE, TRUE, NULL);
#endif

#ifdef MANUAL_RESET
	g_EventHandle = CreateEvent(NULL, TRUE, TRUE, NULL);
#endif
	if (g_EventHandle == 0)
		return 1;



	HANDLE thread1 = (HANDLE)_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)threadProc1, NULL, CREATE_SUSPENDED, NULL);
	if (thread1 == 0)
		return 1;

	HANDLE thread2 = (HANDLE)_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)threadProc2, NULL, CREATE_SUSPENDED, NULL);
	if (thread2 == 0)
		return 1;

	// SetEvent(g_EventHandle);


	ResumeThread(thread1);
	ResumeThread(thread2);

	WaitForSingleObject(thread1, INFINITE);
	WaitForSingleObject(thread2, INFINITE);

	CloseHandle(thread1);
	CloseHandle(thread2);

	return 0;
}