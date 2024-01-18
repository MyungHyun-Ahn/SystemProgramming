#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

void DumpEnvStrings()
{
	PTSTR pEnvBlock = GetEnvironmentStrings();

	TCHAR szName[MAX_PATH];
	TCHAR szValue[MAX_PATH];
	PTSTR pszCurrent = pEnvBlock;
	HRESULT hr = S_OK;
	PCTSTR pszPos = NULL;
	int current = 0;


	while (pszCurrent != NULL)
	{

		if (*pszCurrent != TEXT('='))
		{
			pszPos = _tcschr(pszCurrent, TEXT('='));

			pszPos++;

			size_t cbNameLength = (size_t)pszPos - (size_t)pszCurrent - sizeof(TCHAR);
			hr = StringCbCopyN(szName, MAX_PATH, pszCurrent, cbNameLength);
			if(FAILED(hr))
				break;


			hr = StringCchCopyN(szValue, MAX_PATH, pszPos, _tcslen(pszPos) + 1);

			if (SUCCEEDED(hr))
			{
				_tprintf(TEXT("[ %u] %s=%s\r\n"), current, szName, szValue);
			}
			else
			{
				if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
				{
					_tprintf(TEXT("[ %u] %s=%s...\r\n"), current, szName, szValue);
				}
				else
				{
					_tprintf(TEXT("[ %u] %s=???...\r\n"), current, szName);
				}
			}
		}
		else
		{
			_tprintf(TEXT("[ %u] %s\r\n"), current, pszCurrent);
		}

		current++;


		while (*pszCurrent != TEXT('\0'))
			pszCurrent++;
		pszCurrent++;

		if (*pszCurrent == TEXT('\0'))
			break;
	}

	FreeEnvironmentStrings(pEnvBlock);
}

void DumpEnvVariables(PTSTR pEnvBlock[])
{
	int current = 0;
	PTSTR *pElement = (PTSTR *)pEnvBlock;
	PTSTR pCurrent = NULL;
	while (pElement != NULL)
	{
		pCurrent = (PTSTR)(*pElement);
		if (pCurrent == NULL)
		{
			pElement = NULL;
		}
		else
		{
			_tprintf(TEXT("[% u] %s\r\n"), current, pCurrent);
			current++;
			pElement++;
		}
	}
}

int _tmain(int argc, TCHAR *argv[], TCHAR *env[])
{
	// DumpEnvStrings();
	DumpEnvVariables(env);
	return 0;
}