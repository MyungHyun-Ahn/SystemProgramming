#include <stdio.h>
#include <string.h>

int main(void)
{
	wchar_t str[] = L"ABC";
	int size = sizeof(str);
	int len = wcslen(str);

	printf("배열의 크기: %d \n", size);
	printf("문자열 길이: %d \n", len);

	return 0;
}

/*
SBCS 함수를 WBCS 기반의 함수로 바꾸려면
함수명의 str을 wcs로 변경하면 된다.

strcpy -> wcscpy
*/