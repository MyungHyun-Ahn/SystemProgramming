#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

extern "C" const IMAGE_DOS_HEADER __ImageBase;

void DumpModule()
{
	// ���� ���� ���ø����̼��� ���� �ּҸ� �����´�.
	// ���� ���� �ڵ尡 DLL ���� �ִ� ��� �ٸ� ���� ����� �� �ִ�.
	HMODULE hModule = GetModuleHandle(NULL);
	_tprintf(TEXT("with GetModuleHandle(NULL) = 0x%x\r\n"), hModule);


	// ���� ����� ���� �ּ� (hModule/hInstance)�� ��� ����
	// ���� ������ __ImageBase�� ����Ѵ�.
	_tprintf(TEXT("with __ImageBase = 0x%x\r\n"), (HINSTANCE)&__ImageBase);

	// ���� ���� ���� DumpModule�� �ּҸ� GetModuleHandleEx�� �Ű�������
	// �����Ͽ� ���� ���� ���� ����� ���� �ּ�(hModule/hInstance)�� ���´�.
	hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)DumpModule, &hModule);
	_tprintf(TEXT("with GetModuleHandleEx = 0x%x\r\n"), hModule);
}

int _tmain(int argc, TCHAR *argv[])
{
	DumpModule();
	return 0;
}