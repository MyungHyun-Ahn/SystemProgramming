/*
	Sort를 실행하는 프로그램
*/

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#define MAX_STRING_NUM 100
#define MAX_STRING_LEN 256

void SortSting(void)
{
	TCHAR stringArr[MAX_STRING_NUM][MAX_STRING_LEN];

	// 콘솔로 문자열 읽기
	int nStr;
	for (nStr = 0; nStr < MAX_STRING_NUM; nStr++)
	{
		TCHAR* isEOF = _fgetts(stringArr[nStr], MAX_STRING_LEN, stdin);

		if (isEOF == NULL)
			break;
	}

	// String Bubble Sort
	TCHAR strTemp[MAX_STRING_LEN];

	for (int i = 0; i < nStr; i++)
	{
		for (int j = nStr - 1; j > i; j--)
		{
			if (_tcscmp(stringArr[j - 1], stringArr[j]) > 0)
			{
				_tcscpy(strTemp, stringArr[j - 1]);
				_tcscpy(stringArr[j - 1], stringArr[j]);
				_tcscpy(stringArr[j], strTemp);
			}
		}

		for (int i = 0; i < nStr; i++)
			_fputts(stringArr[i], stdout);
	}
}

int _tmain(int argc, TCHAR* argv[])
{
	SortSting();

	return 0;
}