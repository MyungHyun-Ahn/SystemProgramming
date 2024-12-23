# 2장 아스키코드 vs 유니코드
## Section 01 - Windows에서의 유니코드(UNICODE)
### 문자셋(Character Sets)의 종류와 특성
아스키코드 (ASCII CODE)
* 미국에서 정의하고 있는 표준
* 알파벳은 26개, 몇몇 확장 문자를 포함해도 256개를 넘지 않음
* 1바이트 가지고도 충분히 표현 가능
* 즉, 아스키 코드는 1바이트

유니코드 (UNICODE)
* 우리나라를 예를 들면 컴퓨터가 한글을 인식하기 위해서는 글자 하나하나마다 값을 지정
* 그렇다고 해서 미국에서 영어를 표현하기 위해 정의한 아스키코드 값을 한글에 사용할 수도 없는 일이다. 추가로 대한민국 컴퓨터도 영어를 표현해야하기 때문이다.
* 그래서 등장한 것이 유니코드이다.
* 유니코드는 문자를 표현하는데 2바이트를 사용한다.
* 나타낼 수 있는 문자의 종류가 65536개 이므로 전세계의 모든 문자와 다양한 종류의 기호를 표현할 수 있다.

문자셋(Set)

SBCS(Single Byte Character Set)
* 1바이트 만을 사용하는 방식
* 아스키 코드가 대표적인 SBCS

MBCS(Multi Byte Character Set)
* 다양한 바이트 수를 사용해서 문자를 표현하는 방식
* 유니코드가 MBCS에 해당? - 오답
* MBCS는 SBCS를 포함하기 때문
* 유니코드는 대부분의 문자들을 2바이트로 처리하되 경우에 따라서는 1바이트로 처리
* 아스키코드에서 정의하고 있는 문자를 표현할 때는 1바이트로 처리한다.

WBCS(Wide Byte Character Set)
* 유니코드가 WBCS 방식에 해당한다.

### MBCS 기반의 문자열
문제점 1
* 영문은 1바이트, 한글은 2바이트로 처리되고 있다.
* 문자열 길이 또한 한글은 한 글자에 2로 처리된다.

문제점 2
* fputc 함수는 1바이트만 처리하는 함수이다.
* 한글로 구성된 문자열의 실제 길이는 5이지만 10번 호출해야 모두 출력 가능하다.

### WBCS 기반의 프로그래밍
char를 대신하는 wchar_t
* char 형 변수는 1바이트 메모리 공간이 할당
* wchar_t 형 변수는 2바이트 메모리 공간이 할당
* 따라서 유니코드를 기반으로 문자를 표현하는 것이 가능하다.
* typedef unsigned short wchar_t;

"ABC"를 대신하는 L"ABC"
* wchar_t str[] = "ABC";
* 위와 같이 문자열을 선언하면 문제가 발생한다.
* 오른쪽의 값은 여전히 MBCS 방식이기 때문이다.
* 다음과 같이 문자열을 선언해야 한다.
* wchar_t str[] = L"ABC";
* 문자열 앞에 선언된 문자 L은 "WBCS 기반으로 표현하라"는 의미를 지닌다.
* NULL 문자 또한 2바이트로 처리한다.

strlen을 대신하는 wcslen
* SCBS 함수로 wchar_t 타입의 문자열을 처리할 수 없다.
* WBCS 함수를 사용해야 한다.
* 기존 SCBS 함수의 str 부분을 wcs로 변경하면 된다.
* ex) strcpy -> wcscpy

완전한 유니코드 기반으로 : 첫 번째
* Windows 2000 이상의 운영체제는 기본적으로 유니코드를 지원한다.
* 또한, 내부적으로 모든 문자열을 유니코드 기반으로 처리한다.
* 문자열 처리 함수 또한 SBCS 기반 함수와 WBCS 기반 함수 중 하나로 구분된다.
* SBCS : printf
* WBCS : wprintf
---
* Windows 2000 이상의 운영체제에서 위와 같은 함수의 호출이 이뤄지면 운영체제는 내부적으로 2바이트 유니코드 형식으로 변환한다.
* **이는 프로그램 성능에 다소 영향을 미치는 요소가 될 수 있다.**
* 물론 유니코드 기반으로 프로그램을 작성하면, 성능에는 전혀 영향을 미치지 않을 것이다.

완전한 유니코드 기반으로 : 두 번째
* wmain 함수로 인자를 전달하면 L"" 로 인자를 받는다.

## Section 02 - MBCS와 WBCS의 동시 지원
* 프로그램 구현에 있어서 MBCS 기반이냐 WBCS 기반이냐를 선택하는 것은 골치아프다.
* 모두 지원하도록 구현하면 되는 문제이다.

### #include <windows.h>
* windows.h는 Windows 기반 프로그래밍을 하는 데 있어 항상 포함해야 하는 헤더이다.

### Windows에서 정의하고 있는 자료형
* Windows에서는 typedef 키워드를 통하여 몇몇 기본 자료형에 Windows 스타일의 새로운 이름을 정의하고 있다.

CHAR와 WCHAR
* typedef char CHAR;
* typedef wchar_t WCHAR;

### MBCS와 WBCS(유니코드)를 동시에 지원하기 위한 매크로
* 매크로 UNICODE, _UNICODE
* 위 매크로는 Windows.h가 아닌 tchar.h에 선언되어 있다.
* tchar.h를 추가로 선언해야 한다.

### 이것만은 알고 갑시다.

1. SBCS, MBCS, WBCS의 이해

2. 유니코드 기반 문자열 처리 함수
3. UNICODE와 _UNICODE
4. 유니코드 방식과 MBCS 방식을 모두 지원하기 위한 main 함수의 구성