#include <stdio.h>
#include <string.h>

// �ѱ��� ó���ϱ� ���� �ʿ�
#include <locale.h>

int main(void)
{
	_wsetlocale(LC_ALL, L"korean");

	wchar_t str[] = L"ABC";
	int size = sizeof(str);
	int len = wcslen(str);

	wprintf(L"�迭�� ũ��: %d \n", size);
	wprintf(L"���ڿ� ����: %d \n", len);

	return 0;
}