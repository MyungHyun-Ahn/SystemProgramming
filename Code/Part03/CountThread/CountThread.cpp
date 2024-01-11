/*
	CountThread.cpp
	프로그램 설명 : 생성 가능한 스레드의 개수 측정
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define MAX_THREADS (1024 * 10)

DWORD ThreadProc(LPVOID lpParam)
{
	DWORD threadNum = (DWORD)lpParam;

	while (true)
	{
		_tprintf(_T("thread num : %d \n"), threadNum);
		Sleep(5000);
	}

	return 0;
}

DWORD cntOfThread = 0;

int _tmain(int argc, TCHAR *argv[])
{
	DWORD dwThreadID[MAX_THREADS];
	HANDLE hThread[MAX_THREADS];

	// 생성 가능한 최대 개수의 스레드 생성
	while (true)
	{
		hThread[cntOfThread] = CreateThread(
			NULL, // 디폴트 보안 속성 지정
			0, // 디폴트 스택 사이즈
			ThreadProc, // 스레드 함수
			(LPVOID)cntOfThread, // 스레드 함수의 전달인자
			0, // 디폴트 생성 플래그
			&dwThreadID[cntOfThread] // 스레드 ID 반환
		);

		if (hThread[cntOfThread] == NULL)
		{
			_tprintf(_T("MAXIMUM THREAD NUMBER : %d \n"), cntOfThread);
			break;
		}

		cntOfThread++;
	}

	for (DWORD i = 0; i < cntOfThread; i++)
	{
		CloseHandle(hThread[i]);
	}

	return 0;
}