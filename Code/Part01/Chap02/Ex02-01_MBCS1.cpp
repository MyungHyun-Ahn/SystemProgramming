/*
	MBCS1.cpp
*/

#include <stdio.h>
#include <string.h>

int main(void)
{
	char str[] = "ABC한글";
	int size = sizeof(str);
	int len = strlen(str);

	printf("배열의 크기: %d \n", size);
	printf("문자열 길이: %d \n", len);

	return 0;
}

/*
배열의 크기: 8
문자열 길이: 7

배열의 크기
ABC(3바이트) + 한글(4바이트) + NULL 문자(1바이트) = 8바이트
영문 1바이트, 한글 2바이트로 처리되고 있다.
MBCS를 기반으로 예제가 실행되고 있음을 보여준다.

문자열 길이
strlen은 문자열의 길이를 계산함에 있어 NULL 문자를 포함하지 않는다.
ABC(길이 3) + 한글(길이 4) = 7

"ABC한글"의 실제 길이는 5이지만, "한글"이라는 단어는 길이가 4로 인식되고 있다.
** 이것이 바로 MBCS의 문제점이다.
*/