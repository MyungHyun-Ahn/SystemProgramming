/******************************************************************************
Module:  ErrorShow.cpp
Notices: Copyright (c) 2008 Jeffrey Richter & Christophe Nasarre
******************************************************************************/


#include "..\CommonFiles\CmnHdr.h"     /* See Appendix A. */
#include <Windowsx.h>
#include <tchar.h>
#include "Resource.h"


///////////////////////////////////////////////////////////////////////////////


#define ESM_POKECODEANDLOOKUP    (WM_USER + 100)
const TCHAR g_szAppName[] = TEXT("Error Show");


///////////////////////////////////////////////////////////////////////////////


BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {

   chSETDLGICONS(hwnd, IDI_ERRORSHOW);

   // Don't accept error codes more than 5 digits long
   Edit_LimitText(GetDlgItem(hwnd, IDC_ERRORCODE), 5);

   // Look up the command-line passed error number
   SendMessage(hwnd, ESM_POKECODEANDLOOKUP, lParam, 0);
   return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {

   switch (id) {

   case IDCANCEL:
      EndDialog(hwnd, id);
      break;

   case IDC_ALWAYSONTOP:
      SetWindowPos(hwnd, IsDlgButtonChecked(hwnd, IDC_ALWAYSONTOP) 
         ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
      break;

   case IDC_ERRORCODE: 
      EnableWindow(GetDlgItem(hwnd, IDOK), Edit_GetTextLength(hwndCtl) > 0);
      break;

   case IDOK:
      // Get the error code
      // 에러 코드를 획득한다.
      DWORD dwError = GetDlgItemInt(hwnd, IDC_ERRORCODE, NULL, FALSE);

      // Buffer that gets the error message string
      // 에러 메시지 텍스트를 저장하기 위한 버퍼
      HLOCAL hlocal = NULL;

      // Use the default system locale since we look for Windows messages.
      // Note: this MAKELANGID combination has 0 as value
      // 윈도우 메시지 문자열을 얻기 위해 기본 시스템 설정을 사용한다.
      // 주의 : 아래의 MAKELANGID는 0 값을 반환한다.
      DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

      // Get the error code's textual description
      // 에러 코드의 메시지 텍스트를 가져온다.
      // FORMAT_MESSAGE_FROM_SYSTEM : 운영체제가 정의하는 에러 코드와 대응되는 메시지 텍스트를 얻고자 한다는 것을 의미
      // FORMAT_MESSAGE_ALLOCATE_BUFFER : 에러 메시지 텍스트를 저장할 수 있는 메모리 공간을 할당해 줄 것을 요청
      // FORMAT_MESSAGE_IGNORE_INSERTS : 메시지 텍스트에 %로 시작하는 placeholder를 실질적인 값으로 변경하지 않을 것을 지정하는 플래그
      BOOL fOk = FormatMessage(
         FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
         FORMAT_MESSAGE_ALLOCATE_BUFFER, 
         NULL, dwError, systemLocale, 
         (PTSTR) &hlocal, 0, NULL);

      if (!fOk) {
         // Is it a network-related error?
         // 네트워크와 관련된 에러인가?
         // Dll에 포함한 에러코드를 얻기위해
         HMODULE hDll = LoadLibraryEx(TEXT("netmsg.dll"), NULL, 
            DONT_RESOLVE_DLL_REFERENCES);

         if (hDll != NULL) {
            fOk = FormatMessage(
               FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS |
               FORMAT_MESSAGE_ALLOCATE_BUFFER,
               hDll, dwError, systemLocale,
               (PTSTR) &hlocal, 0, NULL);
            FreeLibrary(hDll);
         }
      }

      if (fOk && (hlocal != NULL)) {
         SetDlgItemText(hwnd, IDC_ERRORTEXT, (PCTSTR) LocalLock(hlocal));
         LocalFree(hlocal);
      } else {
         SetDlgItemText(hwnd, IDC_ERRORTEXT, 
            TEXT("No text found for this error number."));
      }

      break;
   }
}


///////////////////////////////////////////////////////////////////////////////


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

   switch (uMsg) {
      chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
      chHANDLE_DLGMSG(hwnd, WM_COMMAND,    Dlg_OnCommand);

   case ESM_POKECODEANDLOOKUP:
      SetDlgItemInt(hwnd, IDC_ERRORCODE, (UINT) wParam, FALSE);
      FORWARD_WM_COMMAND(hwnd, IDOK, GetDlgItem(hwnd, IDOK), BN_CLICKED, 
         PostMessage);
      SetForegroundWindow(hwnd);
      break;
   }

   return(FALSE);
}


///////////////////////////////////////////////////////////////////////////////


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) {

   HWND hwnd = FindWindow(TEXT("#32770"), TEXT("Error Show"));
   if (IsWindow(hwnd)) {
      // An instance is already running, activate it and send it the new #
      SendMessage(hwnd, ESM_POKECODEANDLOOKUP, _ttoi(pszCmdLine), 0);
   } else {
      DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_ERRORSHOW), 
         NULL, Dlg_Proc, _ttoi(pszCmdLine));
   }
   return(0);
}


//////////////////////////////// End of File //////////////////////////////////
