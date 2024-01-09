#include <stdio.h>
#include <string.h>

// 한글을 처리하기 위해 필요
#include <locale.h>

int main(void)
{
	_wsetlocale(LC_ALL, L"korean");

	wchar_t str[] = L"ABC";
	int size = sizeof(str);
	int len = wcslen(str);

	wprintf(L"배열의 크기: %d \n", size);
	wprintf(L"문자열 길이: %d \n", len);

	return 0;
}