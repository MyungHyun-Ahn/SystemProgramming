#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

int WINAPI _tWinMain(HINSTANCE hInstanceExe, HINSTANCE, PTSTR pszCmdLine, int nCmdShor)
{
	// 프로세스를 생성하기 위해 STARTUPINFO 구조체를 준비
	STARTUPINFO si = { sizeof(si) };
	SECURITY_ATTRIBUTES saProcess, saThread;
	PROCESS_INFORMATION piProcessB, piProcessC;
	TCHAR szPath[MAX_PATH];

	// A 프로세스가 B 프로세스의 생성을 준비한다.
	// 새로운 프로세스 커널 오브젝트 핸들을
	// 상속 가능하도록 생성할 것이다.
	saProcess.nLength = sizeof(saProcess);
	saProcess.lpSecurityDescriptor = NULL;
	saProcess.bInheritHandle = TRUE;

	// 새로운 스레드 커널 오브젝트 핸들을 상속 불가능하도록 생성할 것이다.
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;

	// B 프로세스를 생성한다.
	_tcscpy_s(szPath, _countof(szPath), TEXT("ProcessB"));
	CreateProcess(NULL, szPath, &saProcess, &saThread, FALSE, 0, NULL, NULL, &si, &piProcessB);

	// pi 구조체는 A 프로세스와 관련된 두 개의 핸들을 가지고 있다.
	// hProcess는 B 프로세스의 커널 오브젝트 핸들이며 상속 가능하다.
	// hThread는 B 프로세스의 주 스레드 오브젝트 핸들이며, 상속이 불가능하다.

	// A 프로세스가 새로운 C 프로세스의 생성을 준비한다.

	// psaProcess와 psaThread 매개변수로 NULL을 전달하면
	// C 프로세스의 프로세스 오브젝트 핸들과
	// 주 스레드 오브젝트 핸들은 기본적으로 상속이 불가능한 형태로 생성된다.

	// A 프로세스가 또 다른 프로세스를 생성하면
	// C 프로세스의 프로세스 오브젝트 핸들과
	// 스레드 오브젝트 핸들은 상속될 수 없다.

	// bInheritHandle로 TRUE를 전달하였기 때문에
	// B 프로세스의 주 오브젝트 핸들은 상속되지만
	// B 프로세스의 주 스레드 오브젝트 핸들은 상속되지 않는다.
	_tcscpy_s(szPath, _countof(szPath), TEXT("ProcessC"));
	CreateProcess(NULL, szPath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &piProcessC);

	return 0;
}