/*
	MBCS1.cpp
*/

#include <stdio.h>
#include <string.h>

int main(void)
{
	char str[] = "ABC�ѱ�";
	int size = sizeof(str);
	int len = strlen(str);

	printf("�迭�� ũ��: %d \n", size);
	printf("���ڿ� ����: %d \n", len);

	return 0;
}

/*
�迭�� ũ��: 8
���ڿ� ����: 7

�迭�� ũ��
ABC(3����Ʈ) + �ѱ�(4����Ʈ) + NULL ����(1����Ʈ) = 8����Ʈ
���� 1����Ʈ, �ѱ� 2����Ʈ�� ó���ǰ� �ִ�.
MBCS�� ������� ������ ����ǰ� ������ �����ش�.

���ڿ� ����
strlen�� ���ڿ��� ���̸� ����Կ� �־� NULL ���ڸ� �������� �ʴ´�.
ABC(���� 3) + �ѱ�(���� 4) = 7

"ABC�ѱ�"�� ���� ���̴� 5������, "�ѱ�"�̶�� �ܾ�� ���̰� 4�� �νĵǰ� �ִ�.
** �̰��� �ٷ� MBCS�� �������̴�.
*/