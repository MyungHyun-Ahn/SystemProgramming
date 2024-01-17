# Chapter 02 - 문자와 문자열로 작업하기
문자열을 다룰 때 주로 발생하는 버퍼 오버런 에러는 윈도우의 취약점을 공격하는 주요 방법이다.\
이번 장에서는 문자열을 다룰 때 발생하는 문제를 방지하기 위한 C 런타임 라이브러리의 새로운 함수를 다룬다.

개발자는 항상 유니코드 문자와 새로운 안전 문자열 함수를 이용해서 개발해야 한다.

유니코드가 아닌 문자를 이용하여 프로그래밍하고 있다면 당장 유니코드 기반으로 바꾸는 것이 좋다.

## Section 01 - 문자 인코딩
* ANSI 문자는 256가지의 문자를 표현할 수 있다.
* 세계 각국의 모든 언어를 표현하려면 1바이트로는 부족하다.
* 이걸 해결하기 위해 DBCS(double-bytes character set)이 만들어졌다.

윈도우는 UTF-16으로 인코딩한다.
* 문자를 2바이트(16비트에 저장)

유니코드의 코드 포인트(특정 문자의 위치)는 여러 영역으로 나뉘어 있다.

## Section 02 - ANSI 문자와 유니코드 문자 그리고 문자열 자료형
char 자료형이 8비트의 ANSI 문자를 표현하기 위해 존재한다.
* 최근에 마이크로소프트의 C/C++ 컴파일러는 16비트 유니코드(UTF-16)을 표현하기 위한 wchar_t 자료형을 내장 자료형으로 처리할 수 있는 기능이 추가되었다.
* /Zx:wchar_t 컴파일러 스위치를 지정할 때만 내장 자료형으로 컴파일된다.
* 최근의 Visual Studio의 경우 C++ 프로젝트를 생성하면 기본적으로 컴파일러 스위치가 켜진다.

wchar_t를 내장 자료형으로 처리하지 못하는 컴파일러의 경우 다음과 같이 정의
* typedef unsigned short wchar_t;

WinNT.h 헤더 파일에 윈도우 자료형을 구분짓기 위해 다음과 같이 정의
1. typedef char CHAR;
2. typedef wchar_t WCHAR;
3. typedef CHAR *PCHAR;
4. typedef CHAR *PSTR;
5. typedef CONST CHAR *PCSTR;
6. ... 

typedef __nullterminated WCHAR *NWPSTR, *LPWSTR, *PWSTR;
* __nulltermianted : 헤더 표기, 널로 끝나는 문자열로 지정


윈도우 개발자라면 항상 윈도우 자료형을 사용하는 것이 좋다.

매크로를 통해 UNICODE나 ANSI 문자를 사용하도록 작성하는 것도 가능하다.

UNICODE, 매크로를 이용하면 컴파일 시 ANSI 문자나 유니코드 문자간의 변경이 가능한 단일 소스코드를 작성할 수 있다.
* TCHAR c = TEXT('A');
* TEXT 매크로

## Section 03 - 윈도우 내의 유니코드 함수와 ANSI 함수
윈도우 NT 이후 모든 윈도우 버전은 유니코드를 바탕으로 작성되었다.
* 텍스트를 출력하고, 문자열을 다루는 핵심 함수들은 모두 유니코드 바탕이다.
* ANSI 문자열을 전달하면 전달된 문자열을 유니코드로 전환하고 운영체제로 전달한다.
* ANSI 문자열을 반환하는 함수의 경우에도 유니코드 -> ANSI로의 전환이 일어난다.
* 문자열 변경을 위한 메모리와 시간 낭비를 고려해야 한다.

윈도우즈 API의 함수들은 두가지 버전을 제공한다.
1. 함수명 맨 뒤 W : 유니코드 버전 함수
2. 함수명 맨 앞 A : ANSI 버전 함수

그러나 우리는 W, A 둘 다 붙지 않은 버전의 함수를 사용한다.
* 매크로로 정의되어 있기 때문

ANSI 함수들은 내부적으로 ANSI 코드에서 유니코드로 변경하는 단계를 수행한 후 다시 유니코드 함수를 호출하여 결과를 얻어온다.


1. CreateWindowExA : ANSI 버전 함수 호출
2. ANSI 문자열 -> 유니코드 문자열
3. 다시  CreateWindowExW : 유니코드 버전 함수 호출
4. CreateWindowExW 함수가 반환되면
5. 유니코드 문자열 -> ANSI 문자열
6. CreateWindowsExA 함수 반환

많은 반환과정을 거치기 때문에 처음부터 유니코드 문자열을 사용하는 것이 좋다.
* 더 많은 메모리를 소모하고 느리게 동작한다.
* 심지어 윈도우 문자열 변경 과정에 일부 버그가 있다고 알려져 있다.

WinExec와 OpenFile 등 몇몇 윈도우 API 함수는 16비트 윈도우 용으로 제작되었기 때문에 ANSI 문자만 받는다.
* 유니코드를 받는 CreateProcess와 CreateFile를 사용하자.
* 이러한 함수들은 사실 내부적으로 최신의 함수를 호출한다.
* 오래된 함수는 유니코드 문자열도 받아들이지 못하고, 제공하는 기능도 적다.

## Section 04 - C 런타임 라이브러리 내의 유니코드 함수와 ANSI 함수
대표적인 예로 ANSI 문자를 받는 strlen과 유니코드를 받는 wcslen이 있다.
* TChar.h 헤더파일에 _tcslen으로 매크로가 정의되어 있다.
* 대부분의 ANSI 함수에 _t를 붙이면 모두 호환 가능하다.

Visual Studio에서 프로젝트를 생성하면 _UNICODE가 정의된다.
* C++ 표준안은 아니기 때문에 직접 UNICODE 매크로를 정의하지 말아야 한다.

## Section 05 - C 런타임 라이브러리 내의 안전 문자열 함수
문자열을 다루는 함수들은 항시 잠재적인 위험에 노출되어 있다.

메모리를 깨뜨리는 예시

WCHAR szBuffer[3] = L"";\
wcscpy(szBuffer, L"abc");
* 문자열 종결 문자 역시 하나의 문자다.

strcpy와 wcscpy의 문제점은 버퍼의 최대 크기를 인자로 받지 않는다는 것이다.
* 메모리에 문제가 생겨도 에러를 보고받을 수 없고 정상적으로 메모리가 사용되는지 알 수 없다.

마이크로소프트는 C 런타임 라이브러리를 통해 제공되던 안전하지 않은 함수를 대체하는 함수를 소개했다.
* StrSafe.h 헤더 파일에서 제공하는 안전 문자열 함수를 사용하는 것이 좋다.
* _s가 붙은 버전의 함수를 사용하는 것이 좋다.


### 새로운 안전 문자열 함수에 대한 소개
* StrSafe.h 헤더를 포함하면 String.h 헤더도 함께 포함된다.
* StrSafe.h 헤더에는 기존 함수를 사용하면 경고를 나타내도록 설정되어 있다.
* StrSafe.h 헤더는 다른 include 구문보다 뒤에 나타나야 한다.

_s가 붙은 버전의 함수
* 버퍼의 크기를 추가로 받는다.
* 내부적으로 가장 먼저 인자의 유효성을 검증한다.
* 검증이 실패하면 errno에 에러 코드를 설정하고 errno_t 타입의 값을 반환한다.
* assertion 다이얼로그 박스를 표시하고 애플리케이션을 종료한다.
* 릴리즈 모드의 경우 이러한 단계 없이 바로 종료된다.

C 런타임 라이브러리는 인자의 유효성 검증이 실패하였을 경우 사용자가 정의한 함수를 통해 에러 내용을 전달하는 기능을 제공한다.

void InvalidParameterHandler(PCTSTR expression, PCTSTR function, PCTSTR file, unsigned int line, uintptr_t /\*pRerved\*/);
* expression : C 런타임 함수 내에서 발생한 테스트 실패에 대해 설명하는 문자열
* function : 함수 이름
* file : 소스 파일 명
* line : 소스 코드 라인

다음 단계로 _set_invalid_parameter_handler를 호출하여 앞서 작성한 함수를 등록해야 한다.
* 여기까지 진행해도 assertion 다이얼로그는 나타나기 때문에 다음 함수를 어플리케이션 시작 지점에 호출한다.
* _CrtSetReportMode(_CRT_ASSERT, 0);

이제 String.h에 정의된 기존 문자열 함수를 대체하는 안전 문자열 함수를 사용하면 된다.

### 문자열 조작을 수행하는 동안 좀 더 많은 제어를 수행할 수 있도록 하는 법
* 이런 함수를 사용하면 어떤 값으로 문자열을 채울지, 문자열 잘림을 어떻게 처리할지 등을 지정할 수 있다.

HRESULT StringCchCat(PTSTR pszDest, size_t cchDest, PCTSTR pszSrc);\
HRESULT StringCchCatEx\
HRESULT StringCchCopy\
HRESULT StringCchCopyEx\
HRESULT StringCchPrintf\
HRESULT StringCchPrintfEx
* 모든 함수가 함수명에 Cch를 포함하고 있다. Count of characters
* 보통의 경우 _countof 매크로를 이용하면 적절한 값을 얻을 수 있다.
* Cb를 포함한 경우 Count of Byte로 sizeof 연산자로 값을 얻을 수 있다.

HRESULT 반환 값
* S_OK : 성공 - 정상복사, "\0"으로 문자열 정상 종료
* STRSAFE_E_INVALID_PARAMETER : 인자로 NULL로 전달되었다.
* STRSAFE_E_INSUFFICIENT_BUFFER : 복사 대상 버퍼가 작다.

Ex 버전의 함수는 필요할 때 찾아보자.

대상 버퍼가 충분히 크다면 나머지 값은 0xfd 값으로 채워진다.
* Ex 버전의 함수를 이용하면 채워지는 값을 결정할 수 있다.


### 윈도우의 문자열 함수
문자열의 비교, 정렬의 작업은 매우 일반적인 작업들인데
* 이를 위한 최상의 함수는 CompareString(Ex), CompareStringOrdinal 이다.

int CompareString(\
&nbsp;&nbsp;&nbsp;&nbsp;LCID locale,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwCmdFlags,\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pString1,\
&nbsp;&nbsp;&nbsp;&nbsp;int cch1,\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pString2,\
&nbsp;&nbsp;&nbsp;&nbsp;int cch2\
);
1. LCID : 지역 ID, GetThreadLocale 함수로 얻을 수 있다.
2. dwCmdFlags : 비교 방법을 조정하는 플래그
3. 나머지 변수 : 문자열, 문자의 개수(바이트 아님)
4. cch에 음수를 주면 널로 끝나는 문자로 인식

프로그램 내에서 일반적인 문자열(경로명, 레지스트리키/값, XML 등)을 비교할 때는 CompareStringOrdinal

int CompareStringOrdinal(\
&nbsp;&nbsp;&nbsp;&nbsp;PCWSTR pString1,\
&nbsp;&nbsp;&nbsp;&nbsp;int cch1,\
&nbsp;&nbsp;&nbsp;&nbsp;PCWSTR pString2,\
&nbsp;&nbsp;&nbsp;&nbsp;int cch2,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bIgnoreCase,\
);
* 이 함수는 유니코드 문자열만 받는다.

C 런타임 라이브러리의 cmp 형태와 반환 타입이 다르다.
* 0인 경우 : 함수 호출이 실패했음을 의미
* CSTR_LESS_THAN(1) : str1이 str2보다 작다.
* CSTR_EQUAL(2) : 같다.
* CSTR_GREATER_THAN(3) : str1이 str2보다 크다.

좀 더 편리하게 이용하기 위해 반환값에서 2를 빼면
* C 런타임 라이브러리와 같은 의미를 가진다.

## Section 06 - 왜 유니코드를 사용하는 것이 좋은가?
* 다른 나라의 언어로 지역화하기 좋다.
* 단일 파일로 모든 언어를 지원할 수 있다.
* 코드가 더 빠르게 수행되고 더 작은 메모리를 활용하기 때문에 효율적이다.
* 윈도우가 제공하는 모든 함수를 편하게 사용할 수 있다.
* COM과 상호 운용이 쉽다.
* 닷넷 프레임워크와 상호 운용이 쉽다.
* 리소스를 쉽게 다룰 수 있다.
  

## Section 07 - 문자와 문자열 작업에 대한 권고사항
* 문자열을 char 타입이나 byte 타입의 배열로 생각하지 말고 문자의 배열로 생각하라
* 중립 자료형을 사용해라
* 명시적인 자료형을 사용해라. BYTE나 PBYTE
* TEXT나 _T 매크로를 사용해라.
* 문자나 문자열에 관련된 자료형을 애플리케이션 전반에 걸쳐 변경해라.
* 문자열에 대한 산술적인 계산 부분을 수정하라. sizeof, _countof


문자열을 다루는 함수의 권고사항
* 함수의 이름이 _s로 끝나거나 StringCch로 시작하는 안전 문자열 함수를 사용하라.
* 안전하지 않은 함수는 사용하지 마라.
* 컴파일러가 자동적으로 버퍼 오버런을 감지할 수 있도록 /GS, /RTCs 컴파일러 플래그를 활용해라.
* Kernel32가 제공하는 lstrcat, lstrcpy 등의 문자열 관련 함수를 사용하지 마라.
* 문자열을 비교하는 경우 CompareStringOrdinal을 사용해라

## Section 08 - 유니코드 문자열과 ANSI 문자열 사이의 변경
멀티 바이트 문자열 To 와이드 문자 문자열

int MultiByteToWideChar(\
&nbsp;&nbsp;&nbsp;&nbsp;UINT uCodePage,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwFlags,\
&nbsp;&nbsp;&nbsp;&nbsp;PCSTR pMultiByteStr,\
&nbsp;&nbsp;&nbsp;&nbsp;int cbMultiByte,\
&nbsp;&nbsp;&nbsp;&nbsp;PWSTR pWideCarStr,\
&nbsp;&nbsp;&nbsp;&nbsp;int cchWideChar\
);
1. uCodePage : 멀티바이트 문자열과 관련된 코드 페이지 지정
2. dwFlags : 악센트 기호와 같은 발음 기호 추가 옵션
3. 나머지는 변경할 문자와 길이

멀티 바이트 문자 문자열을 유니코드로 변경하는 과정
1. MultiByteToWideChar 함수 호출 : pWideCharStr = NULL, cchWideChar = 0, cbMultiByte = -1
2. 유니코드 문자열로의 변경에 필요한 충분한 메모리 공간을 할당
   * MultiByteToWideChar 반환 값에 sizeof(wchar_t)를 곱한 값
3. MultiByteToWideChar 함수를 재호출한다.
   * pWideCharStr에 할당된 버퍼의 주소를 전달하고
   * cchWideChar에 앞서 호출한 MultiByteToWideChar의 반환 값을 전달한다.
4. 변경된 문자열을 사용한다.
5. 유니코드 문자열의 메모리 공간을 해제한다.


와이드-문자 문자열을 멀티바이트-문자 문자열로 변경하는 함수

int WideCharToMultiByte(\
&nbsp;&nbsp;&nbsp;&nbsp;UINT uCodePage,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwFlags,\
&nbsp;&nbsp;&nbsp;&nbsp;PCWSTR pWideCharStr,\
&nbsp;&nbsp;&nbsp;&nbsp;int cchWideChar,\
&nbsp;&nbsp;&nbsp;&nbsp;PSTR pMultiByteStr,\
&nbsp;&nbsp;&nbsp;&nbsp;int cbMultiByteStr,\
&nbsp;&nbsp;&nbsp;&nbsp;PCSTR pDefaultChar,\
&nbsp;&nbsp;&nbsp;&nbsp;PBOOL pfUsedDefaultChar\
);
1. pDefaultChar : uCodePage에 적절한 문자가 없을 때 사용, NULL로 지정하면 ? 로 채워짐
2. pfUsedDefaultChar : 성공 여부


### ANSI와 유니코드 DLL 함수의 익스포트
* 변경해서 함수에 전달하면 된다.

### 텍스트가 ANSI인지 유니코드인지 여부를 확인하는 방법
확인하는 함수

BOOL IsTextUnicode(CONST VOID pvBuffer, int cb, PINT pResult);

텍스트 파일의 경우 내용이 어떤 식으로 저장되었는지 안정적으로 빠른 방법이 없다.
* IsTextUnicode 함수는 전달되는 버퍼의 내용을 근간으로 확률적이고 규정에 의거한 방법을 활용한다.
* 과학적인 방법이 아니기 때문에 잘못된 결과를 반환할 수 있다.
* 세 번째 매개변수는 어떤 방식으로 테스트를 수행할지 지정한다.