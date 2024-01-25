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

	// �˰��� ����
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

	// Start �Լ� ȣ�� ���� ����� �ð��� �и��� ������ ��ȯ
	__int64 Now() const
	{
		LARGE_INTEGER liPerfNow;
		QueryPerformanceCounter(&liPerfNow);

		return (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000000) / m_liPerfFreq.QuadPart);
	}

private:
	LARGE_INTEGER m_liPerfFreq;		// �ʴ� ī��Ʈ ��
	LARGE_INTEGER m_liPerfStart;	// ���� ī��Ʈ
};

int main()
{
	// PerformLongOperation();

	// �ð� ���� Ÿ�̸Ӹ� �����Ѵ�. (�⺻������ ���� �ð��� ȹ��ȴ�.)
	CStopwatch stopwatch;

	int sum = 0;
	for (int i = 0; i < 100000; i++)
	{
		sum += i;
	}

	__int64 qwElapsedTime = stopwatch.Now();

	return 0;
}