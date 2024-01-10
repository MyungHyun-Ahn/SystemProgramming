/*
	NonStopAdderManager.cpp

	���α׷� ����
		1. 1~10���� �����Ͽ� ���
		2. �������� ������ �ִ� ����

	total 518
	STILL_ACTIVE : ���μ��� Ȱ��ȭ ���� �ڵ� 258

	�ش� ���� �ذ��ϱ� ���� WaitForSingleObject �Լ� ��� ����
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, TCHAR *argv[])
{
	STARTUPINFO si1 = { 0, };
	STARTUPINFO si2 = { 0, };

	PROCESS_INFORMATION pi1;
	PROCESS_INFORMATION pi2;

	DWORD returnVal1;
	DWORD returnVal2;

	TCHAR command1[] = _T("PartAdder.exe 1 5");
	TCHAR command2[] = _T("PartAdder.exe 6 10");

	DWORD sum = 0;

	si1.cb = sizeof(si1);
	si2.cb = sizeof(si2);

	SetCurrentDirectory(_T("C:\\Procademy\\SystemProgramming\\Code\\Part02\\x64\\Debug")); // ���� ���͸� ����

	CreateProcess(
		NULL,
		command1,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si1,
		&pi1
	);

	CreateProcess(
		NULL,
		command2,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si2,
		&pi2
	);

	CloseHandle(pi1.hThread);
	CloseHandle(pi2.hThread);

	WaitForSingleObject(pi1.hProcess, INFINITE);
	WaitForSingleObject(pi2.hProcess, INFINITE);

	GetExitCodeProcess(pi1.hProcess, &returnVal1);
	GetExitCodeProcess(pi2.hProcess, &returnVal2);
	
	if (returnVal1 == -1 || returnVal2 == -1)
		return -1; // ������ ����

	sum += returnVal1;
	sum += returnVal2;

	_tprintf(_T("total : %d \n"), sum);

	CloseHandle(pi1.hProcess);
	CloseHandle(pi2.hProcess);

	return 0;
}
