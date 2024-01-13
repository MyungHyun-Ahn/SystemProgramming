#pragma comment(lib, "swapDll.lib")
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <swapDll.h>

int _tmain(int argc, TCHAR *argv[])
{
	int a = 10;
	int b = 20;

	_tprintf(TEXT("Before: %d, %d\n"), a, b);

	swapDll(&a, &b);

	_tprintf(TEXT("After: %d, %d\n"), a, b);

	return 0;
}