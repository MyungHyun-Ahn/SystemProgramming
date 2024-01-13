/*
	�ڵ� ���̺��� �ε����� Ȯ���ϴ� ����
*/

#include <stdio.h>
#include <windows.h>
#include <tchar.h>

int _tmain(int argc, TCHAR *argv[])
{
	Sleep(10000);

	HANDLE handles[10] = { 0, };

	for (int i = 0; i < 10; i++)
	{
		_tprintf(TEXT("Input Enter : Create Handle"));
		TCHAR tchar = _gettchar();
		SECURITY_ATTRIBUTES sa = { 0, };
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		handles[i] = CreateSemaphore(&sa, 10, 10, NULL);
		// ���� 2��Ʈ�� �����찡 ���������� ����ϱ� ������ ���� ����Ʈ >> 2
		long long lpHandleVal = (reinterpret_cast<long long>(handles[i]) >> 2);

		_tprintf(TEXT("Handle Table Index : %lld\n"), lpHandleVal);
	}


	for (int i = 0; i < 10; i++)
	{
		if (handles[i] != NULL)
			CloseHandle(handles[i]);
	}

	return 0;
}