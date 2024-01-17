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
	pageSize = sSysInfo.dwPageSize;

	// MAX_PAGE의 개수만큼 페이지 RESERVE!
	baseAddr = VirtualAlloc(NULL, MAX_PAGE * pageSize, MEM_RESERVE, PAGE_NOACCESS);
	if (baseAddr == NULL)
	{
		_tprintf(_T("VirtualAlloc Reserve Failed!"));
	}

	lpPtr = (int *) baseAddr; // 배열의 시작 번지와 같은 의미 부여
	nextPageAddr = (int *) baseAddr;


	// page fault 발생 시 예외 발생
	for (int i = 0; i < (MAX_PAGE * pageSize) / sizeof(int); i++)
	{
		__try
		{
			lpPtr[i] = i;
		}
		__except(PageFaultExceptionFilter(GetExceptionCode()))
		{
			ExitProcess(GetLastError());
		}
	}


	for (int i = 0; i < (MAX_PAGE * pageSize) / sizeof(int); i++)
		_tprintf(_T("%d "), lpPtr[i]);

	BOOL isSuccess = VirtualFree(baseAddr, 0, MEM_RELEASE);

	if (isSuccess)
		_tprintf(_T("Release succeeded!"));
	else
		_tprintf(_T("Release failed!"));

	return 0;
}

int PageFaultExceptionFilter(DWORD exptCode)
{
	// 예외의 원인이 'page fault'가 아니라면
	if (exptCode != EXCEPTION_ACCESS_VIOLATION)
	{
		_tprintf(_T("Exception code = %d \n"), exptCode);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	_tprintf(_T("Exception is a page fault \n"));

	if (pageCnt >= MAX_PAGE)
	{
		_tprintf(_T("Exception: out of pages \n"));
		return EXCEPTION_EXECUTE_HANDLER;
	}	

	LPVOID lpvResult = VirtualAlloc((LPVOID)nextPageAddr, pageSize, MEM_COMMIT, PAGE_READWRITE);

	if (lpvResult == NULL)
	{
		_tprintf(_T("VirtualAlloc failed \n"));
		return EXCEPTION_EXECUTE_HANDLER;
	}
	else
	{
		_tprintf(_T("Allocating another page. \n"));
	}

	pageCnt++;
	nextPageAddr += pageSize / sizeof(int); // 실수 주의 !

	return EXCEPTION_CONTINUE_EXECUTION;
}
