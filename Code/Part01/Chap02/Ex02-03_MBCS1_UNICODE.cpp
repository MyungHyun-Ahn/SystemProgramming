#include <stdio.h>
#include <string.h>

int main(void)
{
	wchar_t str[] = L"ABC";
	int size = sizeof(str);
	int len = strlen(str);

	printf("배열의 크기: %d \n", size);
	printf("문자열 길이: %d \n", len);

	return 0;
}

/*
지금까지 공부해왔던 문자열 조작함수들은 SBCS 기반 문자열 처리 함수였다.
*/