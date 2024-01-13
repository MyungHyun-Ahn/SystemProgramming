#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, TCHAR* argv)
{
	SYSTEM_INFO si;
	DWORD allocGranularity;
	DWORD pageSize;

	GetSystemInfo(&si);
	pageSize = si.dwPageSize;

	allocGranularity = si.dwAllocationGranularity;

	_tprintf(_T("Page Size : %u KB\n"), pageSize / 1024);
	_tprintf(_T("Allocation Granularity : %u KB\n"), allocGranularity / 1024);

	// page size 4 kb
	// alloc granularity 64 kb - page 크기의 배수

	return 1;
}