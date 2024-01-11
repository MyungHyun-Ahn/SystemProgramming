/*
	CommandPrmpt_One.cpp
	���α׷� ���� : ��� ������Ʈ�� ���
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>
#include <TlHelp32.h>
#include <vector>

#define STR_LEN 256
#define CMD_TOKEN_NUM 10

TCHAR ERROR_CMD[] = _T("'%s'��(��) ������ �� �ִ� ���α׷��� �ƴմϴ�. \n");

int CmdProcessing(int);
TCHAR *StrLower(TCHAR *);
BOOL CmdCreateProcess(TCHAR*);
BOOL CmdEcho(TCHAR[]);
BOOL CmdStart(TCHAR *);
BOOL CmdListProcess();
BOOL CmdKillProcess(TCHAR*);
std::vector<DWORD> GetProcessPIDs(TCHAR*);

TCHAR cmdString[STR_LEN];
TCHAR cmdTokenList[CMD_TOKEN_NUM][STR_LEN];
TCHAR seps[] = _T(" ,\t\n");

int _tmain(int argc, TCHAR *argv[])
{
	// �ѱ� �Է��� ������ �ϱ� ����
	_tsetlocale(LC_ALL, _T("Korean"));

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
			_tcscpy(cmdTokenList[i - 1], argv[i]);

		CmdProcessing(argc - 1);
	}

	DWORD isExit;

	while (true)
	{
		isExit = CmdProcessing(0);
		if (isExit == TRUE)
		{
			_fputts(_T("��ɾ� ó���� �����մϴ�. \n"), stdout);
			break;
		}
	}

	return 0;
}

/***************************************************************************************
	�Լ� : TCHAR int CmdProcessing(int)
	��� : ��ɾ �Է� �޾Ƽ� �ش� ��ɾ �����Ǿ� �ִ� ����� �����Ѵ�.
			exit�� �ԷµǸ� TRUE�� ��ȯ�ϰ� �̴� ���α׷��� ����� �̾�����.
***************************************************************************************/

int CmdProcessing(int argc)
{
	if (argc > 0)
	{
		if (!_tcscmp(cmdTokenList[0], _T("exit")))
		{
			return TRUE;
		}
		else if (!_tcscmp(cmdTokenList[0], _T("echo")))
		{
			CmdEcho(cmdTokenList[1]);
		}
		else if (!_tcscmp(cmdTokenList[0], _T("start")))
		{
			TCHAR optString[STR_LEN] = { 0, };
			TCHAR cmdStringWithOpt[STR_LEN] = { 0, };

			if (argc > 1)
			{
				for (int i = 1; i < argc; i++)
				{
					_stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);
				}


				_stprintf(cmdStringWithOpt, _T("%s %s"), _T("SPProject.exe"), optString);
			}
			else
			{
				_stprintf(cmdStringWithOpt, _T("%s"), _T("SPProject.exe"));
			}

			BOOL isSuccess = CmdStart(cmdStringWithOpt);
			if (isSuccess == FALSE)
				_tprintf(_T("Start ���� \n"));
		}
		else if (!_tcscmp(cmdTokenList[0], _T("aaaaa")))
		{

		}
		else
		{
			BOOL isSuccess = CmdCreateProcess(cmdTokenList[0]);

			if (isSuccess == FALSE)
				_tprintf(ERROR_CMD, cmdTokenList[0]);
		}
	}


	_fputts(_T("Best Command Prompt>> "), stdout);
	_getts_s(cmdString, STR_LEN);

	TCHAR *token = _tcstok(cmdString, seps);
	int tokenNum = 0;

	while (token != NULL)
	{
		_tcscpy(cmdTokenList[tokenNum++], token);
		token = _tcstok(NULL, seps);
	}

	if (!_tcscmp(cmdTokenList[0], _T("exit")))
	{
		return TRUE;
	}
	else if (!_tcscmp(cmdTokenList[0], _T("echo")))
	{
		CmdEcho(cmdTokenList[1]);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("start")))
	{
		TCHAR optString[STR_LEN] = { 0, };
		TCHAR cmdStringWithOpt[STR_LEN] = { 0, };

		if (tokenNum > 1)
		{
			for (int i = 1; i < tokenNum; i++)
			{
				_stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);
			}


			_stprintf(cmdStringWithOpt, _T("%s %s"), _T("SPProject.exe"), optString);
		}
		else
		{
			_stprintf(cmdStringWithOpt, _T("%s"), _T("SPProject.exe"));
		}

		BOOL isSuccess = CmdStart(cmdStringWithOpt);
		if (isSuccess == FALSE)
			_tprintf(_T("Start ���� \n"));
	}
	else if (!_tcscmp(cmdTokenList[0], _T("lp")))
	{
		CmdListProcess();
	}
	else if (!_tcscmp(cmdTokenList[0], _T("kp")))
	{
		if (!CmdKillProcess(cmdTokenList[1]))
			_tprintf(_T("CmdKillProcess Error!\n"));
	}
	else if (!_tcscmp(cmdTokenList[0], _T("")))
	{

	}
	else
	{
		BOOL isSuccess = CmdCreateProcess(cmdTokenList[0]);

		if (isSuccess == FALSE)
			_tprintf(ERROR_CMD, cmdTokenList[0]);
	}

	return 0;
}

std::vector<DWORD> GetProcessPIDs(TCHAR *processName)
{
	std::vector<DWORD> processPids;

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return processPids;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
		return processPids;

	DWORD retValue = -1;

	do
	{
		if (_tcscmp(processName, pe32.szExeFile) == 0)
		{
			processPids.push_back(pe32.th32ProcessID);
		}
	} while (Process32Next(hProcessSnap, &pe32));

	return processPids;
}

/***************************************************************************************
	�Լ� : BOOL CmdKillProcess(TCHAR*)
	��� : �������� ���μ����� �����Ѵ�.
***************************************************************************************/

BOOL CmdKillProcess(TCHAR *processName)
{
	std::vector<DWORD> processIds = GetProcessPIDs(processName);

	if (processIds.empty())
	{
		_tprintf(_T("Invalid Process Ids"));
		return FALSE;
	}

	while (!processIds.empty())
	{
		int processId = processIds.back();
		processIds.pop_back();

		HANDLE hKillProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, processId);

		if (hKillProcess == INVALID_HANDLE_VALUE)
		{
			_tprintf(_T("OpenProcess Error! Code : %d\n"), GetLastError());
			CloseHandle(hKillProcess);
			return FALSE;
		}

		if (!TerminateProcess(hKillProcess, 0))
		{
			_tprintf(_T("KillProcess Error! Code : %d\n"), GetLastError());
			CloseHandle(hKillProcess);
			return FALSE;
		}


		CloseHandle(hKillProcess);
	}

	return TRUE;
}


/***************************************************************************************
	�Լ� : BOOL CmdListProcess(void)
	��� : �������� ���μ����� ����Ѵ�.
***************************************************************************************/
BOOL CmdListProcess()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return FALSE;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
		return FALSE;

	do 
	{
		_tprintf(_T("%30s %5d\n"), pe32.szExeFile, pe32.th32ProcessID);
	} while (Process32Next(hProcessSnap, &pe32));
}


/***************************************************************************************
	�Լ� : BOOL CmdCreateProcess(TCHAR[])
	��� : ���μ��� �̸��� �Է¹޾� �����Ѵ�.
			���� ���θ� ��ȯ�Ѵ�.
***************************************************************************************/
BOOL CmdCreateProcess(TCHAR command[])
{
	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE;
	si.dwX = 100;
	si.dwY = 200;
	si.dwXSize = 300;
	si.dwYSize = 200;

	SetCurrentDirectory(_T("C:\\Procademy\\SystemProgramming\\Code\\Part02\\x64\\Debug")); // ���� ���͸� ����

	BOOL ret = CreateProcess(
		NULL,
		command,
		NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE, 
		NULL, NULL, &si, &pi
	);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return ret;
}

/***************************************************************************************
	�Լ� : BOOL CmdStart(TCHAR[])
	��� : �� cmd â�� ����.
			�߰��� ��ɾ �Է°����ϴ�.
***************************************************************************************/

BOOL CmdStart(TCHAR command[])
{
	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE;
	si.dwX = 100;
	si.dwY = 200;
	si.dwXSize = 300;
	si.dwYSize = 200;

	SetCurrentDirectory(_T("C:\\Procademy\\SystemProgramming\\Code\\Part02\\x64\\Debug")); // ���� ���͸� ����

	BOOL ret = CreateProcess(
		NULL,
		command,
		NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE,
		NULL, NULL, &si, &pi
	);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return ret;
}


/***************************************************************************************
	�Լ� : BOOL CmdEcho(TCHAR[])
	��� : �Է��� ���ڸ� �״�� ����Ѵ�.
***************************************************************************************/

BOOL CmdEcho(TCHAR command[])
{
	_tprintf(command);
	_fputts(_T("\n"), stdout);
	return TRUE;
}


/***************************************************************************************
	�Լ� : TCHAR *StrLower(TCHAR *pStr)
	��� : ���ڿ� ���� �����ϴ� ��� �빮�ڸ� �ҹ��ڷ� �����Ѵ�.
			����� ���ڿ��� �����͸� ��ȯ�Ѵ�.
***************************************************************************************/

TCHAR *StrLower(TCHAR *pStr)
{
	TCHAR *ret = pStr;

	while (*pStr)
	{
		if (_istupper(*pStr))
			*pStr = _totlower(*pStr);

		pStr++;
	}

	return ret;
}