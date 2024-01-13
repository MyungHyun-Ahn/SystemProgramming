#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>

#define TYPE CHAR
#define LPTYPE CHAR *

#define MAX_LOOP 1024 * 10

int _tmain(int argc, TCHAR *argv[])
{

	LPTYPE pInt = (LPTYPE)malloc(sizeof(TYPE));

	printf("memory size : %d \npointer addr : %p\n", (int)sizeof(TYPE), pInt);

	
	DWORD dwPrevSize = (int)sizeof(TYPE);

	for (int i = 2; i < MAX_LOOP; i++)
	{

		LPTYPE pNewInt = (LPTYPE )realloc(pInt, i * sizeof(TYPE));
		_tprintf(_T("memory size : %d \npointer addr : %p\n"), i * (int)sizeof(TYPE), pNewInt);

		if (pNewInt != pInt)
		{
			_tprintf(_T("memory address is changed %p to %p\n"), pInt, pNewInt);
			_tprintf(_T("prev memory size : %d, next memory size : %d\n"), dwPrevSize, i * (int)sizeof(TYPE));
			_tprintf(_T("size gep : %d\n"), i * (int)sizeof(TYPE) - dwPrevSize);
			dwPrevSize = i * (int)sizeof(TYPE), pNewInt;
		}

		pInt = pNewInt;
	}

	free(pInt);

	return 0;

}