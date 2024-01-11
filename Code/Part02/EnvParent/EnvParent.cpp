/*
	EnvParent.cpp
	���α׷� ���� : ȯ�溯�� �����ϴ� ���μ���
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, TCHAR *argv[])
{
	SetCurrentDirectory(_T("C:\\Procademy\\SystemProgramming\\Code\\Part02\\x64\\Debug")); // ���� ���͸� ����

	SetEnvironmentVariable(_T("Good"), _T("morning"));
	SetEnvironmentVariable(_T("Hey"), _T("Ho!"));
	SetEnvironmentVariable(_T("Big"), _T("Boy"));

	STARTUPINFO si = { 0. };
	PROCESS_INFORMATION pi = { 0, };
	si.cb = sizeof(si);

	TCHAR command[] = _T("EnvChild.exe");

	CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}