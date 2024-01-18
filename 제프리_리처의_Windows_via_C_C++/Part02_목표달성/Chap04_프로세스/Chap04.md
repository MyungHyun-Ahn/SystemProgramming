# Chepter 04 프로세스
시스템이 어떻게 수행 중인 애플리케이션을 관리하는 지에 대해 알아볼 것이다.
* 프로세스란, 프로세스 커널 오브젝트를 어떻게 생성하는지
* 프로세스 커널 오브젝트를 이용하여 프로세스를 어떻게 사용하는지
* 프로세스의 다양한 속성과 특성 다루기
* 프로세스 생성과 종료

프로세스는 일반적으로 수행 중인 프로그램의 인스턴스(instance)라고 정의하며, 두 개의 컴포넌트로 구성된다.
* 프로세스를 관리하기 위한 목적으로 운영체제가 사용하는 커널 오브젝트.
* 시스템은 각종 통계 정보를 프로세스 커널 오브젝트에 저장하기도 한다.
* 실행 모듈은 DLL의 코드와 데이터를 수용하는 주소 공간.
* 이러한 주소 공간은 스레드 스택이나 힙 할당과 같은 동적 메모리 할당에 사용되는 공간도 포함한다.

프로세스는 자력으로 수행될 수 없다.
* 반드시 프로세스의 컨텍스트 내에서 수행되는 스레드가 있어야 한다.

스레드는 프로세스의 주소 공간 상에 위치하고 있는 코드를 수행할 책임이 있다.
* 프로세스는 다수의 스레드를 가질 수 있다.
* 스레드들은 프로세스 주소 공간 내에서 동시에 코드를 수행한다.

각 스레드들은 자신만의 CPU 레지스터 집합과 스택을 가진다.

프로세스가 생성되면 자동으로 생성해주는 첫 스레드 : 주(main) 스레드
* 만약 이 주 스레드가 없다면, 시스템은 자동적으로 프로세스를 파괴한다.

모든 스레드가 동시에 수행되도록 CPU는 시간을 나누어준다.
* 라운드 로빈 방식으로 퀀텀이라는 단위 시간만큼 수행할 수 있다.

마이크로소프트 윈도우는 CPU 별로 서로 다른 스레드를 수행하도록 스케줄링한다.
* 윈도우 커널이 담당

## Section 01 첫 번째 윈도우 애플리케이션 작성
윈도우는 두 가지 형태의 애플리케이션을 제공
1. CUI : Console User Interface
2. GUI : Graphical User Interface

마이크로소프트의 Visual Studio를 이용하여 애플리케이션을 생성
* Visual Studio는 실행 파일에 알맞은 서브시스템 타입을 실행 파일에 포함시킬 수 있도록
* 링커 스위치를 설정한다.
* CUI 링커 스위치 : /SUBSYSTEM:CONSOLE
* GUI 링커 스위치 : /SUBSYSTEM:WINDOWS

애플리케이션을 실행 -> 운영체제의 로더 -> 실행 파일 헤더 확인 -> 서브시스템 값을 가져옴

윈도우 애플리케이션의 두 가지 진입점

GUI

int WINAPI _tWinMain(\
&nbsp;&nbsp;&nbsp;&nbsp;HINSTANCE hInstanceExe,\
&nbsp;&nbsp;&nbsp;&nbsp;HINSTANCE,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszCmdLine,
&nbsp;&nbsp;&nbsp;&nbsp;int nCmdShow\
);

CUI

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[]);

* 이런 함수들을 우리가 직접 실행하지는 않는다.
* C/C++ 런타임이 시작 함수(C/C++ runtime startup function)를 호출한다.
* 링크 시 -entry: 명령행 옵션을 통해 설정된다.

C/C++ 런타임 시작 함수가 하는 일
* 함수가 호출될 수 있도록 C/C++ 런타임 라이브러리 초기화
* 전역 오브젝트와 static으로 선언된 C++ 오브젝트를 코드가 수행되기 전에 적절히 생성

어떤 진입점 함수가 언제 구현해야 하는지

* 애플리케이션 타입 : 진입점 : 실행 파일에 포함되는 런타임 시작 함수
* ANSI 사용 GUI : _tWinMain (WinMain) : WinMainCRTStartup
* 유니코드 사용 GUI : _tWinMain (wWinMain) : wWinMainCRTStartup
* ANSI 사용 CUI : _tmain (main) : mainCRTStartup
* 유니코드 사용 CUI : _tmain (wmain) : wmainCRTStartup

/SUBSYSTEM:WINDOWS 링커 스위치 설정
* 진입점을 찾지 못할 경우 : unresolved external symbol
* 진입점을 찾았다면 런타임 시작함수 호출

/SUBSYSTEM:CONSOLE 링커 스위치 설정
* 진입점을 찾지 못한 경우 : unresolved external symbol
* 진입점을 찾았다면 런타임 시작함수 호출

/SUBSYSTEM 링커 스위치를 제거할 수 있다.
* 제거하면 링크는 자동적으로 적합한 설정을 찾아낸다.
* 프로젝트를 잘못 만들었을 경우 /SUBSYSTEM 링커 스위치를 제거한다.

모든 C/C++ 런타임 시작 함수는 동일한 작업을 수행한다.
* 다른 점은 ANSI 문자열이나 유니코드 문자열을 처리해야 한다는 점 정도다.

Visual C++에서는 C/C++ 런타임 라이브러리의 소스코드가 포함되어 있다.
* crtexe.c 파일을 살펴보면 4개의 시작 함수에 대한 구현 내용을 찾아볼 수 있다.

시작함수가 수행하는 작업
1. 새로운 프로세스의 전체 명령행을 가리키는 포인터 획득
2. 새로운 프로세스의 환경변수를 가리키는 포인터 획득
3. C/C++ 런타임 라이브러리의 전역변수를 초기화. 사용자 코드가 StdLib.h 파일을 인클루드하면 변수에 접근 가능
4. C/C++ 런터임 라이브러리의 메모리 할당 함수와 저수준 입출력 루틴이 사용하는 힙을 초기화
5. 모든 전역 오브젝트와 static C++ 클래스 오브젝트의 생성자 호출

이러한 초기화 과정이 모두 완료되고 진입점 함수를 호출


진입점 함수가 반환되면 반환 값(nMainRetVal)을 인자로 하여 C/C++ 런타임 라이브러리의 exit 함수를 호출한다.

exit 함수가 수행하는 일
* _onexit 함수를 이용하여 등록해 두었던 함수를 호출한다.
* 모든 전역 클래스 오브젝트와 static C++ 클래스 오브젝트의 소멸자를 호출한다.
* DEBUG 빌드의 경우 메모리 누수 기능이 켜있다면 메모리 누수 상황 함수를 호출해서 표시한다.
* nMainRetVal 값을 인자로 하여 ExitProcess 함수를 호출한다.

### 프로세스 인스턴스 핸들
모든 실행 파일과 DLL 파일은 프로세스 메모리 공간에 로드될 때 고유의 인스턴스 핸들을 받는다.
* WinMain의 첫 번째 매개변수인 hInstanceExe를 통해 전달받는다.

LoadIcon의 경우

HICON LoadIcon(HINSTANCE hInstance, PCTSTR pszIcon);
* hInstance : WinMain 첫 번째 매개변수
  * hInstanceExe 매개변수를 전역변수로 저장해 두어 실행 파일의 전체 소스에서 접근할 수 있게 한다.

DWORD GetModuleFileName(\
&nbsp;&nbsp;&nbsp;&nbsp;HMODULE hInstModule,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszPath,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD cchPath);
* GetModuleFileName 함수의 경우 HMODULE을 받는데 HINSTANCE와 완전히 동일하다.

WinMain 함수의 hInstance 값
* 프로세스의 메모리 주소 공간 상에 실행 파일을 로드할 시작 메모리 주소다.

만약 시스템이 실행 파일의 내용을 0x00400000에 로드하고자 한다면
* hInstance의 값은 0x00400000
* 0x00400000라는 시작주소는 역사적인 이유로 사용하고 있다. (윈도우98의 시작주소였음)
* 마이크로소프트 링커에서 /BASE:address의 옵션을 사용하여 시작주소를 바꿀 수 있다.

HMODULE GetModuleHandle(PCTSTR pszModule);
* 실행 파일이나 DLL 파일이 프로세스의 메모리 공간 상의 어디에 로드되어 있는지 알아내는 함수
* 널로 끝나는 문자열을 인자로 받는다.
* 파일을 찾지 못하면 NULL 반환
* NULL 값을 넣으면 현재 실행 파일이 로드된 시작주소를 반환한다.

이 함수가 DLL 내에서 호출된다면 어떤 모듈에 포함되어 코드가 수행되는지 확인하는 방법
1. 링커가 정의하는 __ImageBase라는 가상 변수가 현재 수행 중인 모듈의 시작 주소를 가리키고 있다.
2. GetModuleHandleEx 호출
   * GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS를 인자로 넣고
   * 두 번째 매개변수에 현재 수행 중인 함수의 주소
    * 마지막 인자로 HMODULE을 가리키는 포인터 값

GetModuleHandle의 중요한 특성
1. 자신을 호출한 프로세스의 주소 공간만 확인
2. NULL을 인자로 넣으면 실행 파일의 시작 위치
   * DLL 내에서도 DLL 파일의 시작주소가 아닌 실행 파일의 시작주소

### 프로세스의 이전 인스턴스 핸들
WinMain 함수에서 hPrevInstance 매개변수로 항상 NULL을 전달한다.
* 16비트 애플리케이션의 포팅의 편의를 위해 유일하게 WinMain 함수에만 남아있다.

두 번째 매개변수를 참조할 필요가 없기 때문에 WinMain 함수를 이렇게 작성한다.

int WINAPI _tWinMain(HINSTANCE hInstanceExe, HINSTANCE, PSTR pszCmdLine, int nCmdShow);
* 두 번째 매개변수에 이름이 없기 때문에 컴파일러는 매개변수가 참조되지 않았다는 경고를 발생시키지 않는다.
* Visual Studio에서는 UNREFERENCED_PARAMETER 매크로를 사용하여 컴파일 경고가 발생하지 않게 할 수 있다.


### 프로세스의 명령행
새로운 프로세스가 생성되면 프로세스에 명령행이 전달된다.
* 명령행이 비어있는 경우는 거의 없다. : 첫 번째 토큰이 실행파일의 이름이기 때문
* '\0' 단 한글자로 이루어진 명령행을 받을 때도 있다.
  
C 런타임 시작 함수가 GUI 애플리케이션을 수행해야 하는 경우에는 먼저 GetCommandLine 윈도우 함수를 이용하여 프로세스의 명령행 전체를 가져온다.
* 이 중 실행 파일명을 제외한 나머지 부분을 WinMain 함수의 pszCmdLine 매개변수를 통해 전달한다. 
* pmzCmdLine가 가리키는 메모리 버퍼에 값을 쓸 수도 있긴 하지만 버퍼의 끝을 초과하여 값을 쓰는 실수를 범할 수도 있다.
* 버퍼를 읽기 전용으로 다루는 것이 좋다.

GetCommandLine 함수는 여러번 호출하더라도 항상 동일한 버퍼의 주소를 반환한다.
* pszCmdLine을 읽기 전용으로 다뤄야 하는 이유

명령행으로 전달된 내용을 토큰으로 구분하여 사용하는 것을 선호
* __argc, __argv

유니코드 문자열을 여러 개의 토큰으로 분리

PWSTR* CommandLineToArgvW(\
&nbsp;&nbsp;&nbsp;&nbsp;PWSTR pszCmdLine,\
&nbsp;&nbsp;&nbsp;&nbsp;int *pNumArgs);
* pszCmdLine : 명령행
* pNumArgs : 명령행 인자의 수가 반환
* 반환 값 : 유니코드 문자열의 배열


위 함수는 내부적으로 메모리를 할당한다.
* 이렇게 할당된 메모리를 삭제하지 않는다.
* 프로세스가 종료되는 시점에 자동으로 해제된다.

만약 명시적으로 삭제하고 싶다면
* HeapFree 함수를 호출한다.

int nNumArgs;\
PWSTR *ppArgv = CommandLineToArgvW(GetCommandLineW(), &nNumArgs);

// 인자들을 사용한다.

HeapFree(GetProcessHeap(), 0, ppArgv);


### 프로세스의 환경변수
모든 프로세스는 자기 자신과 연관된 환경블록(environment block)을 가지고 있다.
* 환경 블록 : 프로세스의 주소 공간에 할당된 메모리 블록

=::=::= ...\
VarName1=VarValue1\ 0\
VarName2=VarValue2\ 0\
VarName3=VarValue3\ 0 ...\
VarNameX=VarValueX\ 0\
\\ 0

* 각 문자열의 첫 번째 : 환경변수의 이름
* 두 번째 : 할당하고자 하는 변수의 값
* =로 시작하는 문자열은 환경 변수로 사용되는 것이 아니다.


환경 블록에 접근하는 방법
1. 전체 환경 블록을 얻기 위해 GetEnvironmentStrings 함수를 호출
   * 문자열을 = 를 기준으로 하나 씩 파싱한다.
   * Strings에 의해 반환된 문자열은 FreeEnvironmentStrings로 할당 해제한다.
2. CUI 환경에서만 유효 : 세 번째 매개변수인 env 이용
   * =로 시작하는 문자열은 제거된 상태로 전달된다.

=는 환경변수의 구분자 역할을 하기 때문에 환경변수 이름에 =가 들어가면 안된다.

공백문자도 이름으로 쓸 수 있다.


사용자가 윈도우에 로그온을 하면 시스템은 쉘 프로세스를 생성하고 이와 관련된 환경 문자열들을 설정한다.

시스템 전반에 영향을 주는 환경변수 목록 위치
* HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment

현재 로그온한 사용자에게만 영향을 주는 환경변수 목록 위치
* HKEY_CURRENT_USER\\Environment

일반적으로 Child 프로세스는 Parent 프로세스의 환경변수 집합을 그대로 상속한다.
* 어떤 환경변수를 상속해줄지 지정할 수 있다.
* 상속을 통해 전달되는 환경변수는 그대로 전달되는 것이 아닌 복사를 통해 전달된다.

애플리케이션은 일반적으로 환경변수를 이용하여 사용자가 애플리케이션의 동작 방식을 변경할 수 있도록 한다.

환경변수를 이용하는 프로그램에서 사용할 수 있는 유용한 함수

환경변수의 존재 여부와 값 가져오기

DWORD GetEnvironmentVariable(\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszName,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszValue,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD cchValue);
* pszName : 환경변수 이름 문자열의 포인터
* pszValue : 환경변수의 값을 저장할 버퍼
* cchValue : 버퍼의 크기
* 반환값 : 환경변수 존재하지 않을 시 0, 성공 시 버퍼에 저장한 문자 개수
* cchValue 매개변수로 0을 전달하면 필요한 크기를 얻어올 수 있다.

환경변수에는 대체 가능한 문자열이 포함된 경우가 많다.

%USERPROFILE%\\Documents
* %로 감싼 부분은 대체 가능 문자열을 의미
* USERPROFILE이라는 이름의 환경변수의 값으로 대체한다.

대체 가능 문자열에 대한 변경 작업은 매우 일반적이기 때문에 다음 함수가 있다.

DWORD ExpandEnvironmentStrings(\
&nbsp;&nbsp;&nbsp;&nbsp;PTCSTR pszSrc,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszDst,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD chSize);
* pszSrc : 대체 가능 환경변수 문자열을 포함한 문자열의 주소
* pszDst : 변경될 문자열의 주소
* chSize : 변경될 문자열의 크기
* chSize가 저장하기에 충분하지 않다면 빈 문자열로 변경됨
* ExpandEnvironmentStrings 함수는 2번 호출하는 것이 일반적이다.
* pszDst, chSize를 각각 NULL, 0으로 넣으면 필요한 크기가 반환된다.

환경변수를 추가, 삭제하거나 값을 변경하는 함수

BOOL SetEnvironmentVariable(\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszName,\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszValue);
* pszName의 값을 이름으로 pszValue의 값을 값으로 하는 환경변수를 설정한다.
* 동일한 이름이 있다면 변경
* pszValue에 NULL을 주면 삭제한다.


### 프로세스의 선호도
보통 프로세스 내의 스레드는 어떤 CPU에서도 수행될 수 있다.
* 일부 CPU에서 실행되게도 할 수 있는데 이것을 프로세스의 선호도라고 한다.

### 프로세스의 에러모드
각 프로세스는 에러나 예외를 어떻게 처리할지 플래그 값을 가지고 있다.
* SetErrorMode를 통해 지정할 수 있다.

UINT SetErrorMode(UINT fuErrorMode);
* fuErrorMode는 비트 연산을 통해 여러개를 전달할 수 있다.


기본적으로 Child 프로세스는 Parent 프로세스의 에러 모드 플래그를 상속한다.

### 프로세스의 현재 드라이브와 디렉터리
파일의 전체 경로가 제공되지 않으면 윈도우 함수들은 현재 드라이브와 디렉터리에서 찾는다.

시스템은 내부적으로 프로세스의 현재 드라이브와 디렉터리를 저장한다.
* 프로세스 단위로 유지되기 때문에 한 스레드가 변경하면 다른 스레드도 변경된다.

현재 드라이브와 디렉터리를 얻어오는 함수와 설정하는 함수

DWORD GetCurrentDirectory(\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD cchCurDir,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszCurDir);

BOOL SetCurrentDirectory(PCTSTR pszCurDir);


### 프로세스의 현재 디렉터리
시스템은 프로세스의 현재 드라이브와 디렉터리는 저장하지만 모든 드라이브의 현재 디렉터리를 저장하진 않는다.

다수의 드라이브에 대해 현재 디렉터리를 처리하는 환경변수
* =C:=C:\\Utility\\Bin
* =D:=D:\\Program Files

함수 호출 시 현재 드라이브가 아닌 드라이브의 문자를 전달하게 되면
* 시스템은 프로세스의 환경변수 블록에서 지정된 드라이브와 관련된 환경변수가 있는지 찾는다.
* 만일 변수가 존재하면 그 값을 지정된 드라이브의 현재 디렉터리로 본다.
* 만약 없다면 지정된 드라이브의 루트 디렉터리를 현재 디렉터리로 본다.


Parent 프로세스가 현재 디렉터리 설정 정보를 Child 프로세스에게 전달하고 싶다면
* 자동으로 상속되지는 않는다.
* Parent 프로세스는 드라이브 문자를 이름으로 하는 환경변수를 생성
* Child 프로세스가 수행되기 전 등록해야 한다.


DWORD GetFullPathName(\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszFile,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD cchPath,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszPath,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR *ppszFilePart);
* 현재 디렉터리의 정보를 얻어오는 함수

결국 드라이브 문자를 이름으로 하는 환경변수를 환경블록 시작 위치에 기록해야 한다.



## Section 02 - CreateProcess 함수
프로세스를 생성하는 함수

BOOL CreateProcess(\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszApplicationName,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszCommandLine,\
&nbsp;&nbsp;&nbsp;&nbsp;PSECURITY_ATTRIBUTES psaProcess,\
&nbsp;&nbsp;&nbsp;&nbsp;PSECURITY_ATTRIBUTES psaThread,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInheritHandles,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD fdwCreate,\
&nbsp;&nbsp;&nbsp;&nbsp;PVOID pvEnvironment,\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszCurDir,\
&nbsp;&nbsp;&nbsp;&nbsp;PSTARTUPINFO psiStartInfo,\
&nbsp;&nbsp;&nbsp;&nbsp;PPROCESS_INFORMATION ppiProcInfo);

CreateProcess의 동작 과정
1. 스레드가 CreateProcess를 호출하면 Usage Count가 1인 프로세스 커널 오브젝트를 생성
   * 프로세스 커널 오브젝트는 프로세스 자체를 의미하는 것이 아니다.
   * 프로세스를 관리하기 위한 데이터 구조체이다.
2. 시스템은 새로운 프로세스를 위한 가상 주소 공간을 생성
3. 실행 파일의 코드와 데이터 및 수행에 필여한 추가적인 DLL 파일을 프로세스의 주소 공간에 로드
4. 시스템은 생성된 프로세스의 주 스레드를 위한 스레드 커널 오브젝트(Usage Count 1)를 생성
5. 주 스레드는 링커에 의해 진입점으로 지정된 C/C++ 런타임 시작 코드를 실행
   * 이러한 시작 코드는 사용자가 지정한 진입점 함수를 호출
6. 시스템이 성공적으로 프로세스와 주 스레드를 생성하였다면 CreateProcess는 TRUE를 반환


### pszApplicationName과 pszCommandLine
* pszApplicationName : 실행 파일 명
* pszCommandLine : 명령행 문자열

pszCommandLine이 PTSTR인 이유
* 함수 내에서 변경될 수 있는 형태로 전달되어야 함을 의미
* CreateProcess는 내부적으로 명령행 문자열의 변경작업을 수행하고
* 반환하기 전에 되돌려 놓는다.

pszCommandLine을 읽기 전용으로 전달하면 접근 위반이 발생한다.
* CreateProcess가 내부적으로 전달된 문자열을 수정하려할 때 발생
* 이 문제를 해결하려면 버퍼에 복사해둔 뒤 인자로 주기

윈도우 비스타에서 ANSI 버전의 CreateProcess를 읽기 전용 문자열을 주어도 문제되지 않는다.
* 내부적으로 유니코드로의 변환을 위해 복사본이 만들어지기 때문

CommadLine의 첫 번째 토큰 : 실행하고자 하는 프로그램 이름
* 확장자가 전달되지 않으면 .exe로 간주

실행 파일을 찾기 위한 단계
1. 생성할 프로세스의 실행 파일명에 포함된 디렉터리
2. 생성할 프로세스의 현재 디렉터리
3. 윈도우 시스템 디렉터리
4. 윈도우 디렉터리
5. PATH 환경변수에 포함된 디렉터리

### psaProcess, psaThread, 그리고 bInheritHandles
새로운 프로세스를 생성하기 위해 시스템은 프로세스 커널 오브젝트와 스레드 커널 오브젝트를 생성해야 한다.
* 이를 위해 psaProcess, psaThread 인자로 보안 특성을 받는다.
* NULL로 지정하면 기본 보안 디스크립터를 사용한다.
* 또한 child 프로세스의 child 프로세스를 위해 상속 옵션을 지정하기 위해 사용한다.


bInheritHandles의 값이 TRUE면 Parent 프로세스의 값을 상속한다.


### fdwCreate
새로운 프로세스를 어떻게 생성할지를 결정하게 된다.

1. DEBUG_PROCESS
2. DEBUG_ONLY_THIS_PROCESS
3. CREATE_SUSPENDED
4. DETACHED_PROCESS
5. CREATE_NEW_CONSOLE
6. CREATE_NO_WINDOW
7. CREATE_NEW_PROCESS_GROUP
8. ... msdn을 참고할 것

fdwCreate 매개변수를 이용하면 우선순위 클래스를 지정할 수 있다.
* 프로세스에 우선순위를 변경할 수 있는 것
* 사용하지 않는 것이 좋고, 대부분이 사용하지 않는다.
* 지정하면 프로세스 내의 스레드들이 다른 스레드와 차별적으로 스케줄링된다.


### pvEnvironment
새로운 프로세스가 사용할 환경변수 문자열을 포함하는 메모리 블록을 가리키는 포인터를 지정한다.
* 대부분 Parent 프로세스에게 상속받기 위해 NULL를 넣는다.

GetEnvironmentStrings 로 반환 받은 값을 넣어도 된다.
* NULL로 넣는 것과 완전히 동일하다.


### pszCurDir
생성될 프로세스의 현재 드라이브와 디렉터리를 설정한다.
* NULL인 경우 Parent 프로세스와 동일하게 설정된다.


### psiStartInfo
STARTUPINFO나 STARTUPINFOEX 구조체의 포인터를 넣는다.
* 대부분은 기본 값을 사용하는데
* 이 경우에도 모든 멤버를 0으로 초기화하고, cb 멤버를 구조체의 크기로 설정해야 한다.

cb 멤버가 구조체의 첫 번째 멤버이므로
* STARTUPINFO si = { sizeof(si) };와 같이 해도 된다.

만약 0으로 초기화하지 않으면
* 각 멤버는 CreateProcess를 호출하는 프로세스 스택에 있는 쓰레기 값을 가지게 된다.
* 이런 경우 CreateProcess 함수가 실패할 수 있다.

각 구조체의 멤버는 msdn을 보도록 하자.


### ppiProcInfo
PROCESS_INFORMATION 구조체를 가리키는 포인터로 지정한다.
* CreateProcess 함수는 반환되기 직전에 이 구조체의 멤버를 초기화한다.

typedef struct _PROCESS_INFORMATION {\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hProcess;\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hThread;\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwProcessId;\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwThreadId;\
} PROCESS_INFORMATION;


새로운 프로세스를 만들면 시스템은 새로운 프로세스와 스레드의 커널 오브젝트를 생성한다.
* Usage Count 1로 초기화
* CreateProcess 반환 직전에 커널 오브젝트를 최대 권한으로 열고
* PROCESS_INFORMATION 구조체의 Parent 프로세스에서 사용할 수 있는 hProcess와 hThread에 할당한다. Usage Count 2
* 생성된 커널 오브젝트가 삭제되려면 Parent 프로세스에서도 CloseHandle을 호출해야한다.

Child 스레드의 커널 오브젝트는 반드시 삭제해야만 리소스 누수를 피할 수 있다.
* 더 이상 접근할 일이 없다면 바로 삭제하자

고유한 프로세스 ID와 스레드 ID
* 같은 ID는 존재할 수 없다.
* ID 값은 즉각적으로 재사용된다.
* 잘못된 ID를 사용하지 않으려면 새로운 프로세스가 동일한 ID를 가질 수 있다는 것을 기억하자.

현재 수행 중인 프로세스의 ID를 얻는 방법
* GetCurrentProcessId : 프로세스 ID 얻기
* GetCurrentThreadId : 스레드 ID 얻기

커널 오브젝트 핸들을 이용하여 ID 얻기
* GetProcessId
* GetThreadId

스레드의 핸들을 이용하여 해당 스레드를 소유한 프로세스 ID 얻기
* GetProcessIdOfThread 

Parent-Child 관계는 Child가 생성되는 시점에 프로세스들 사이에 존재한다는 것이다.
* Child 프로세스가 코드를 수행하기 전까지 윈도우는 관계를 전혀 고려하지 않는다.
* ToolHelp 함수를 이용하면 관계를 PROCESSENTRY32 구조체를 통해 확인할 수 있다.
* th32ParentProcessID 멤버에 Parent 프로세스 ID를 반환한다.


## Section 03 - 프로세스의 종료
프로세스가 종료되는 방법은 4가지
1. 주 스레드의 진입점 함수가 반환된다.
2. 프로세스 내의 어떤 스레드가 ExitProcess 함수를 호출한다.
3. 다른 프로세스의 스레드가 TerminateProcess 함수를 호출한다.
4. 프로세스 내의 모든 스레드가 각자 종료된다.


### 주 스레드 진입점 함수의 반환
프로세스가 종료되어야 할 때는 항상 주 스레드의 진입점 함수가 반환하도록 하는 것이 좋다.
* 이 방법만이 주 스레드의 리소스들이 적절히 해제되는 것을 보장한다.

주 스레드 진입점 함수가 반환되면 하는 일
1. 주 스레드에 의해 생성된 C++ 오브젝트들이 소멸자를 이용하여 적절하게 소멸된다.
2. 운영체제는 스레드 스택의 용도로 할당한 메모리 공간을 적절히 해제한다.
3. 시스템은 진입점 함수의 반환 값으로 프로세스의 종료 코드를 설정한다.
4. 시스템은 프로세스 커널 오브젝트의 사용 카운트를 감소시킨다.


### ExitProcess 함수
프로세스 내의 ExitProcess 함수를 호출되면 프로세스는 종료된다.

VOID ExitProcess(UINT fuExitCode);
* 프로세스를 종료하고 fuExitCode의 값으로 프로세스의 종료코드를 설정한다.
  
주 스레드의 진입점 함수가 반환되면
1. C/C++ 런타임 시작 코드로 제어가 돌아가고
2. 프로세스에 의해 사용된 C 런타임 리소스를 적절히 해제한다.
3. 해제가 끝난 뒤에 진입점 함수의 반환값을 인자로하여
4. ExitProcess 함수를 호출한다.

이러한 동작 방식 때문에 주 스레드의 진입점 함수가 반환되면
* 전체 프로세스가 종료되는 것이다.

C/C++ 런타임 시작 코드는 진입점 함수가 반환되면 다른 스레드의 수행 여부와 관계없이 이 함수를 호출한다.


C/C++ 프로그램은 이 함수를 가능한 호출하면 안된다.
* C/C++ 런타임이 관리하는 리소스의 정리작업은 수행되지 않는다.


### TerminateProcess 함수
BOOL TerminateProcess(\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hProcess,\
&nbsp;&nbsp;&nbsp;&nbsp;UINT fuExitCode);

ExitProcess와 차이점
* TerminateProcess 함수는 다른 프로세스도 종료시킬 수 있다.

다른 방법으로는 종료시킬 수 없을 때만 TerminateProcess를 호출한다.
* 종료에 관련된 어떠한 통지도 받지 못한다.
* 적절한 정리 작업, 종료 회피 불가

프로세스가 어떤 식으로든지 종료되면, 운영체제는 그가 사용하던 리소스를 남김없이 제거한다.

TerminateProcess 함수는 비동기 함수다.

### 프로세스 내의 모든 스레드가 종료되면
운영체제가 더 이상 프로세스의 주소 공간을 유지할 필요가 없다고 판단
* 프로세스를 종료시킨다.
* 종료 코드는 마지막으로 종료된 스레드다.


### 프로세스가 종료되면
다음 작업을 수행한다.
1. 프로세스 내에 남아 있는 스레드가 종료된다.
2. 할당했던 모든 오브젝트가 파괴된다.
3. 종료 코드는 STILL_ACTIVE에서 종료 시 설정한 종료 코드로 변경된다.
4. 프로세스 커널 오브젝트의 상태가 Signaled 상태로 변경된다.
5. 프로세스 커널 오브젝트의 사용 카운트가 1 감소한다.


### Child 프로세스
프로그램 설계 시 다른 코드 블록을 수행해야 하는 상황
* 대부분 함수나 서브루틴을 호출하여 수행한다.
  * 대부분의 상황에서는 싱글 태스킹 동기화면 충분하다.
* 스레드를 생성한다.
  * 동기화 문제에 직면한다.
* Child 프로세스를 생성한다.
  * 주소 공간을 보호받을 수 있다.