/*
	MBCS2cpp
*/

#include <stdio.h>

int main(void)
{
	char str[] = "�ѱ��Դϴ�";
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
�ѱ�
�ѱ��Դϴ�

�� �������� ���ڿ��� ���� ������ ���̰� 5�̴�.
�׷��� �������� ���� fputc �Լ��� 10�� ȣ���ؾ� �������� ����� �̷�����.
���ڿ� ���̴� 5������, ���ڿ��� �Ҵ�� ũ�Ⱑ 10����Ʈ�� ����� 1����Ʈ �����͸� ����ϴ� fputc �Լ��� 10�� ȣ���ؾ߸� �Ѵ�.
*/