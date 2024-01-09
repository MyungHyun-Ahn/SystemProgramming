/*
	MBCS2cpp
*/

#include <stdio.h>

int main(void)
{
	char str[] = "한글입니다";
	int i;

	for (i = 0; i < 5; i++)
		fputc(str[i], stdout);

	fputs("\n", stdout);

	for(i = 0; i < 10; i++)
		fputc(str[i], stdout);

	fputs("\n", stdout);
	return 0;
}

/*
한글
한글입니다

위 예제에서 문자열은 누가 보더라도 길이가 5이다.
그러나 실행결과를 보면 fputc 함수를 10번 호출해야 정상적인 출력이 이뤄진다.
문자열 길이는 5이지만, 문자열에 할당된 크기가 10바이트인 관계로 1바이트 데이터를 출력하는 fputc 함수를 10번 호출해야만 한다.
*/