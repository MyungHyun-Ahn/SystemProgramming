#include <stdio.h>
#include <string.h>

int main(void)
{
	wchar_t str[] = L"ABC";
	int size = sizeof(str);
	int len = wcslen(str);

	printf("�迭�� ũ��: %d \n", size);
	printf("���ڿ� ����: %d \n", len);

	return 0;
}

/*
SBCS �Լ��� WBCS ����� �Լ��� �ٲٷ���
�Լ����� str�� wcs�� �����ϸ� �ȴ�.

strcpy -> wcscpy
*/