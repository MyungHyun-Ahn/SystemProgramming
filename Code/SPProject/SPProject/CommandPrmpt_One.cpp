/*
	CommandPrmpt_One.cpp
	프로그램 설명 : 명령 프롬프트의 골격
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>

#define STR_LEN 256
#define CMD_TOKEN_NUM 10

TCHAR ERROR_CMD[] = _T("'%s'은(는) 실행할 수 있는 프로그램이 아닙니다. \n");

int CmdProcessing(void);
TCHAR *StrLower(TCHAR *);
BOOL CmdCreateProcess(TCHAR*);

int _tmain(int agrc, TCHAR *argv[])
{
	// 한글 입력을 가능케 하기 위해
	_tsetlocale(LC_ALL, _T("Korean"));

	DWORD isExit;

	while (true)
	{
		isExit = CmdProcessing();
		if (isExit == TRUE)
		{
			_fputts(_T("명령어 처리를 종료합니다. \n"), stdout);
			break;
		}
	}

	return 0;
}


TCHAR cmdString[STR_LEN];
TCHAR cmdTokenList[CMD_TOKEN_NUM][STR_LEN];
TCHAR seps[] = _T(" ,\t\n");

/***************************************************************************************
	함수 : TCHAR int CmdProcessing(void)
	기능 : 명령어를 입력 받아서 해당 명령어에 지정되어 있는 기능을 수행한다.
			exit가 입력되면 TRUE를 반환하고 이는 프로그램의 종료로 이어진다.
***************************************************************************************/

int CmdProcessing(void)
{
	_fputts(_T("Best Command Prompt>> "), stdout);
	_getts_s(cmdString, STR_LEN);

	TCHAR *token = _tcstok(cmdString, seps);
	int tokenNum = 0;

	while (token != NULL)
	{
		_tcscpy(cmdTokenList[tokenNum++], StrLower(token));
		token = _tcstok(NULL, seps);
	}

	if (!_tcscmp(cmdTokenList[0], _T("exit")))
	{
		return TRUE;
	}
	else if (!_tcscmp(cmdTokenList[0], _T("")))
	{
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

/***************************************************************************************
	함수 : BOOL CmdCreateProcess(TCHAR *pStr)
	기능 : 프로세스 이름을 입력받아 실행한다.
			성공 여부를 반환한다.
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

	SetCurrentDirectory(_T("C:\\Procademy\\SystemProgramming\\Code\\Part02\\x64\\Debug")); // 현재 디렉터리 설정

	BOOL ret = CreateProcess(
		NULL,
		command,
		NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE, 
		NULL, NULL, &si, &pi
	);

	return ret;
}


/***************************************************************************************
	함수 : TCHAR *StrLower(TCHAR *pStr)
	기능 : 문자열 내에 존재하는 모든 대문자를 소문자로 변경한다.
			변경된 문자열의 포인터를 반환한다.
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