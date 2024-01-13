#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, TCHAR *argv[])
{
	TCHAR fileName[] = _T("data.txt");
	TCHAR fileData[] = _T("Just test string~~");

	HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return -1;

	DWORD numOfByteWritten = 0;

	WriteFile(hFile, fileData, sizeof(fileData), &numOfByteWritten, NULL);

	_tprintf(_T("Written data size : %u \n"), numOfByteWritten);
	OVERLAPPED
	return 0;
}
