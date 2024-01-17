/******************************************************************************
Module:  Singleton.cpp
Notices: Copyright (c) 2008 Jeffrey Richter & Christophe Nasarre
******************************************************************************/


#include "resource.h"

#include "..\CommonFiles\CmnHdr.h"     /* See Appendix A. */
#include <windowsx.h>
#include <Sddl.h>          // for SID management : SID 관리를 위해 추가
#include <tchar.h>
#include <strsafe.h>



///////////////////////////////////////////////////////////////////////////////


// Main dialog
// 주 다이얼로그
HWND     g_hDlg;

// Mutex, boundary and namespace used to detect previous running instance
// 이미 수행되고 있는 인스턴스가 있는지 확인하기 위한 뮤텍스, 바운더리, 네임스페이스
HANDLE   g_hSingleton = NULL;
HANDLE   g_hBoundary = NULL;
HANDLE   g_hNamespace = NULL;

// Keep track whether or not the namespace was created or open for clean-up
// 네임스페이스가 생성되었는지 혹은 정리를 위해 오픈되었는지의 여부를 저장
BOOL     g_bNamespaceOpened = FALSE;

// Names of boundary and private namespace
// 바운더리 이름과 프라이비트 네임스페이스 이름
PCTSTR   g_szBoundary = TEXT("3-Boundary");
PCTSTR   g_szNamespace = TEXT("3-Namespace");


#define DETAILS_CTRL GetDlgItem(g_hDlg, IDC_EDIT_DETAILS)


///////////////////////////////////////////////////////////////////////////////


// Adds a string to the "Details" edit control
void AddText(PCTSTR pszFormat, ...) {

   va_list argList;
   va_start(argList, pszFormat);

   TCHAR sz[20 * 1024];

   Edit_GetText(DETAILS_CTRL, sz, _countof(sz));
   _vstprintf_s(
      _tcschr(sz, TEXT('\0')), _countof(sz) - _tcslen(sz), 
      pszFormat, argList);
   Edit_SetText(DETAILS_CTRL, sz);
   va_end(argList);
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {

   switch (id) {
      case IDOK:
      case IDCANCEL:
         // User has clicked on the Exit button
         // or dismissed the dialog with ESCAPE
         EndDialog(hwnd, id);
         break;
   }
}


///////////////////////////////////////////////////////////////////////////////


void CheckInstances() {

   // Create the boundary descriptor
   // 바운더리 디스크립터 생성
   g_hBoundary = CreateBoundaryDescriptor(g_szBoundary, 0);

   // Create a SID corresponding to the Local Administrator group
   // 로컬 관리자 그룹과 연관된 SID 생성
   BYTE localAdminSID[SECURITY_MAX_SID_SIZE];
   PSID pLocalAdminSID = &localAdminSID;
   DWORD cbSID = sizeof(localAdminSID);
   if (!CreateWellKnownSid(
      WinBuiltinAdministratorsSid, NULL, pLocalAdminSID, &cbSID)
      ) {
      AddText(TEXT("AddSIDToBoundaryDescriptor failed: %u\r\n"), 
         GetLastError());
      return;
   }
   
   // Associate the Local Admin SID to the boundary descriptor
   // --> only applications running under an administrator user
   //     will be able to access the kernel objects in the same namespace
   // 로컬 관리자 SID 값을 바운더리 디스크립터에 추가
   // --> 로컬 관리자 권한으로 애플리케이션이 수행될 경우에만
   //     동일 네임스페이스 내의 커널 오브젝트에 접근이 가능함
   if (!AddSIDToBoundaryDescriptor(&g_hBoundary, pLocalAdminSID)) {
      AddText(TEXT("AddSIDToBoundaryDescriptor failed: %u\r\n"), 
         GetLastError());
      return;
   }

   // Create the namespace for Local Administrators only
   // 로컬 관리자만을 위한 네임스페이스 생성
   SECURITY_ATTRIBUTES sa;
   sa.nLength = sizeof(sa);
   sa.bInheritHandle = FALSE;
   if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
      TEXT("D:(A;;GA;;;BA)"), 
      SDDL_REVISION_1, &sa.lpSecurityDescriptor, NULL)) {
      AddText(TEXT("Security Descriptor creation failed: %u\r\n"), GetLastError());
      return;
   }

   g_hNamespace = 
      CreatePrivateNamespace(&sa, g_hBoundary, g_szNamespace);

   // Don't forget to release memory for the security descriptor
   // 보안 디스크립터가 저장된 메모리 공간을 해제하는 것을 잊으면 안된다.
   LocalFree(sa.lpSecurityDescriptor);


   // Check the private namespace creation result
   // 프라이비트 네임스페이스 생성 결과 확인
   DWORD dwLastError = GetLastError();
   if (g_hNamespace == NULL) {
      // Nothing to do if access is denied
      // --> this code must run under a Local Administrator account
       // 접근이 거부되는 경우 아무것도 하지 않음
       // --> 이 코드는 로컬 관리자 계정하에 수행되어야 한다.
      if (dwLastError == ERROR_ACCESS_DENIED) {
         AddText(TEXT("Access denied when creating the namespace.\r\n"));
         AddText(TEXT("   You must be running as Administrator.\r\n\r\n"));
         return;
      } else { 
         if (dwLastError == ERROR_ALREADY_EXISTS) {
         // If another instance has already created the namespace, 
         // we need to open it instead. 
         // 만약 다른 인스턴스가 동일한 네임스페이스를 이미 생성했다면
         // 생성된 네임스페이스를 오픈한다.
            AddText(TEXT("CreatePrivateNamespace failed: %u\r\n"), dwLastError);
            g_hNamespace = OpenPrivateNamespace(g_hBoundary, g_szNamespace);
            if (g_hNamespace == NULL) {
               AddText(TEXT("   and OpenPrivateNamespace failed: %u\r\n"), 
               dwLastError);
               return;
            } else {
               g_bNamespaceOpened = TRUE;
               AddText(TEXT("   but OpenPrivateNamespace succeeded\r\n\r\n"));
            }
         } else {
            AddText(TEXT("Unexpected error occured: %u\r\n\r\n"),
               dwLastError);
            return;
         }
      }
   }
   
   // Try to create the mutex object with a name 
   // based on the private namespace 
   // 프라이비트 네임스페이스에 기반한
   // 뮤텍스 오브젝트를 생성한다.
   TCHAR szMutexName[64];
   StringCchPrintf(szMutexName, _countof(szMutexName), TEXT("%s\\%s"), 
      g_szNamespace, TEXT("Singleton"));

   g_hSingleton = CreateMutex(NULL, FALSE, szMutexName);
   if (GetLastError() == ERROR_ALREADY_EXISTS) {
      // There is already an instance of this Singleton object
      AddText(TEXT("Another instance of Singleton is running:\r\n"));
      AddText(TEXT("--> Impossible to access application features.\r\n"));
   } else  {
      // First time the Singleton object is created
      AddText(TEXT("First instance of Singleton:\r\n"));
      AddText(TEXT("--> Access application features now.\r\n"));
   }
}


///////////////////////////////////////////////////////////////////////////////


BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {

   chSETDLGICONS(hwnd, IDI_SINGLETON);

   // Keep track of the main dialog window handle
   g_hDlg = hwnd;

   // Check whether another instance is already running
   CheckInstances();
      
   return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

   switch (uMsg) {
      chHANDLE_DLGMSG(hwnd, WM_COMMAND,    Dlg_OnCommand);
      chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
   }

   return(FALSE);
}


///////////////////////////////////////////////////////////////////////////////


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

   // Show main window 
   DialogBox(hInstance, MAKEINTRESOURCE(IDD_SINGLETON), NULL, Dlg_Proc);

   // Don't forget to clean up and release kernel resources
   if (g_hSingleton != NULL) {
      CloseHandle(g_hSingleton);
   }

   if (g_hNamespace != NULL) {
      if (g_bNamespaceOpened) {  // Open namespace
         ClosePrivateNamespace(g_hNamespace, 0);
      } else { // Created namespace
         ClosePrivateNamespace(g_hNamespace, PRIVATE_NAMESPACE_FLAG_DESTROY);
      }
   }

   if (g_hBoundary != NULL) {
      DeleteBoundaryDescriptor(g_hBoundary);
   }

   return(0);
}


//////////////////////////////// End of File //////////////////////////////////
