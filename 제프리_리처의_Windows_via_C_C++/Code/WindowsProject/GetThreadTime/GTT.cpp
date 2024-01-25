#include <stdio.h>
#include <tchar.h>
#include <windows.h>


__int64 FileTimeToQuadWord(PFILETIME pft)
{
	return (Int64ShllMod32(pft->dwHighDateTime, 32) | pft->dwLowDateTime);
}

VOID PerformLongOperation()
{
	FILETIME ftKernelTimeStart, ftKernelTimeEnd;
	FILETIME ftUserTimeStart, ftUserTimeEnd;
	FILETIME ftDummy;

	__int64 qwKernelTimeElapsed, qwUserTimeElapsed, qwTotalTimeElapsed;

	GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeStart, &ftUserTimeStart);

	// 알고리즘 수행
	int sum = 0;
	for (int i = 0; i < 10000; i++)
	{
		sum += i;
	}

	qwKernelTimeElapsed = FileTimeToQuadWord(&ftKernelTimeEnd) - FileTimeToQuadWord(&ftKernelTimeStart);

	qwUserTimeElapsed = FileTimeToQuadWord(&ftUserTimeEnd) - FileTimeToQuadWord(&ftUserTimeStart);

	qwTotalTimeElapsed = qwKernelTimeElapsed + qwUserTimeElapsed;
}

class CStopwatch
{
public:
	CStopwatch() { QueryPerformanceFrequency(&m_liPerfFreq); Start(); }

	void Start() { QueryPerformanceCounter(&m_liPerfStart); }

	// Start 함수 호출 이후 경과된 시간을 밀리초 단위로 변환
	__int64 Now() const
	{
		LARGE_INTEGER liPerfNow;
		QueryPerformanceCounter(&liPerfNow);

		return (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000000) / m_liPerfFreq.QuadPart);
	}

private:
	LARGE_INTEGER m_liPerfFreq;		// 초당 카운트 수
	LARGE_INTEGER m_liPerfStart;	// 시작 카운트
};

int main()
{
	// PerformLongOperation();

	// 시간 측정 타이머를 생성한다. (기본적으로 현재 시간이 획득된다.)
	CStopwatch stopwatch;

	int sum = 0;
	for (int i = 0; i < 100000; i++)
	{
		sum += i;
	}

	__int64 qwElapsedTime = stopwatch.Now();

	return 0;
}