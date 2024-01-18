#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

int WINAPI _tWinMain(HINSTANCE hInstanceExe, HINSTANCE, PTSTR pszCmdLine, int nCmdShor)
{
	// ���μ����� �����ϱ� ���� STARTUPINFO ����ü�� �غ�
	STARTUPINFO si = { sizeof(si) };
	SECURITY_ATTRIBUTES saProcess, saThread;
	PROCESS_INFORMATION piProcessB, piProcessC;
	TCHAR szPath[MAX_PATH];

	// A ���μ����� B ���μ����� ������ �غ��Ѵ�.
	// ���ο� ���μ��� Ŀ�� ������Ʈ �ڵ���
	// ��� �����ϵ��� ������ ���̴�.
	saProcess.nLength = sizeof(saProcess);
	saProcess.lpSecurityDescriptor = NULL;
	saProcess.bInheritHandle = TRUE;

	// ���ο� ������ Ŀ�� ������Ʈ �ڵ��� ��� �Ұ����ϵ��� ������ ���̴�.
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;

	// B ���μ����� �����Ѵ�.
	_tcscpy_s(szPath, _countof(szPath), TEXT("ProcessB"));
	CreateProcess(NULL, szPath, &saProcess, &saThread, FALSE, 0, NULL, NULL, &si, &piProcessB);

	// pi ����ü�� A ���μ����� ���õ� �� ���� �ڵ��� ������ �ִ�.
	// hProcess�� B ���μ����� Ŀ�� ������Ʈ �ڵ��̸� ��� �����ϴ�.
	// hThread�� B ���μ����� �� ������ ������Ʈ �ڵ��̸�, ����� �Ұ����ϴ�.

	// A ���μ����� ���ο� C ���μ����� ������ �غ��Ѵ�.

	// psaProcess�� psaThread �Ű������� NULL�� �����ϸ�
	// C ���μ����� ���μ��� ������Ʈ �ڵ��
	// �� ������ ������Ʈ �ڵ��� �⺻������ ����� �Ұ����� ���·� �����ȴ�.

	// A ���μ����� �� �ٸ� ���μ����� �����ϸ�
	// C ���μ����� ���μ��� ������Ʈ �ڵ��
	// ������ ������Ʈ �ڵ��� ��ӵ� �� ����.

	// bInheritHandle�� TRUE�� �����Ͽ��� ������
	// B ���μ����� �� ������Ʈ �ڵ��� ��ӵ�����
	// B ���μ����� �� ������ ������Ʈ �ڵ��� ��ӵ��� �ʴ´�.
	_tcscpy_s(szPath, _countof(szPath), TEXT("ProcessC"));
	CreateProcess(NULL, szPath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &piProcessC);

	return 0;
}