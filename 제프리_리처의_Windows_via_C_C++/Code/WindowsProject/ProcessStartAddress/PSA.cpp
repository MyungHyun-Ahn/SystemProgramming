#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

extern "C" const IMAGE_DOS_HEADER __ImageBase;

void DumpModule()
{
	// 수행 중인 애플리케이션의 시작 주소를 가져온다.
	// 수행 중인 코드가 DLL 내에 있는 경우 다른 값이 얻어질 수 있다.
	HMODULE hModule = GetModuleHandle(NULL);
	_tprintf(TEXT("with GetModuleHandle(NULL) = 0x%x\r\n"), hModule);


	// 현재 모듈의 시작 주소 (hModule/hInstance)를 얻기 위해
	// 가상 변수인 __ImageBase를 사용한다.
	_tprintf(TEXT("with __ImageBase = 0x%x\r\n"), (HINSTANCE)&__ImageBase);

	// 현재 수행 중인 DumpModule의 주소를 GetModuleHandleEx에 매개변수로
	// 전달하여 현재 수행 중인 모듈의 시작 주소(hModule/hInstance)를 얻어온다.
	hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCTSTR)DumpModule, &hModule);
	_tprintf(TEXT("with GetModuleHandleEx = 0x%x\r\n"), hModule);
}

int _tmain(int argc, TCHAR *argv[])
{
	DumpModule();
	return 0;
}