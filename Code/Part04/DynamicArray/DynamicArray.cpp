#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define MAX_PAGE 10

int *nextPageAddr;
DWORD pageCnt = 0;
DWORD pageSize;
int PageFaultExceptionFilter(DWORD);


int _tmain(int argc, TCHAR *argv[])
{
	LPVOID baseAddr;
	int *lpPtr;
	SYSTEM_INFO sSysInfo;

	GetSystemInfo(&sSysInfo);


	// MAX_PAGE�� ������ŭ ������ RESERVE!
	baseAddr = VirtualAlloc(NULL, MAX_PAGE * pageSize, MEM_RESERVE, PAGE_NOACCESS);
	if (baseAddr == NULL)
	{
		_tprintf(_T("VirtualAlloc Reserve Failed!"));
	}

	lpPtr = (int *) baseAddr; // �迭�� ���� ������ ���� �ǹ� �ο�
	nextPageAddr = (int *) baseAddr;


	// page fault �߻� �� ���� �߻�
	for (int i = 0; i < (MAX_PAGE * pageSize) / sizeof(int); i++)
	{
		__try
		{
			lpPtr[i] = i;
		}
		__except (PageFaultExceptionFilter(GetExceptionCode()))
		{
			ExitProcess(GetLastError());
		}
	}


	for (int i = 0; i < (MAX_PAGE * pageSize) / sizeof(int); i++)
		_tprintf(_T("%d "), lpPtr[i]);

	BOOL isSuccess = VirtualFree(baseAddr, 0, MEM_RELEASE);
}

int PageFaultExceptionFilter(DWORD)
{
	return 0;
}
