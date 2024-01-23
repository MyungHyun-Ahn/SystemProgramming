# Chapter 06 스레드의 기본
모든 프로세스는 적어도 하나 이상의 스레드를 사용하기 때문에 스레드는 반드시 이해하고 넘어가야 한다.

스레드는 2가지 요소로 구성되어 있다. (프로세스와 마찬가지)
* 운영체제가 스레드를 다루기 위해 사용하는 스레드 커널 오브젝트. 스레드 커널 오브젝트는 시스템이 스레드에 대한 통계 정보를 저장하는 공간이다.
* 스레드가 코드를 수행할 때 함수의 매개변수와 지역변수를 위한 스레드 스택

프로세스는 스스로 수행될 수 없다.
* 단순히 생각한다면 스레드의 저장소로 볼 수 있다.
* 스레드는 프로세스의 내에서만 살아 있을 수 있다.
* 스레드는 프로세스 주소 공간 내에 있는 코드를 수행하고 데이터를 다룬다.

따라서 하나의 프로세스 내에 둘 이상의 스레드가 존재하는 경우, 이러한 스레드들은 단일 주소 공간을 공유하게 된다.
* 스레드들은 동일한 코드를 수행할 수도 있고, 동일 데이터를 조작할 수도 있다.
* 커널 오브젝트 핸들 테이블 역시 공유한다.

프로세스는 자신만의 주소 공간을 가지기 때문에 스레드에 비해 더욱더 많은 시스템 리소스를 사용한다.

스레드는 단지 하나의 커널 오브젝트와 스레드 스택 정도만을 필요로 할 뿐이다.


## Section 01 스레드를 생성해야 하는 경우
* 멀티스레딩을 사용하면 사용자 인터페이스를 좀 더 단순화시킬 수 있다.
* I/O를 분리된 스레드로 수행하면 애플리케이션의 사용자 인터페이스가 좀 더 즉각적인 응답을 보이도록 작성할 수 있다.
* 애플리케이션을 좀 더 확장성이 좋은 구조로 설계할 수 있다.

## Section 02 스레드를 생성하지 말아야 하는 경우
* 출력하는 동안 값이 수정될 수도 있다.
* 사용자 인터페이스를 위한 컴포넌트들은 반드시 동일한 스레드를 사용해야만 한다.

## Section 03 처음으로 작성하는 스레드 함수

모든 스레드는 수행을 시작할 진입점 함수를 반드시 가져야 한다.

~~~c++
DWORD WINAPI ThreadFunc(PVOID pvParam)
{
    DWORD dwResult = 0;

    ...

    return (dwResult);
}
~~~

스레드 함수는 우리가 원하는 작업은 어떤 것이라도 수행할 수 있다.
* 스레드 함수가 반환되는 시점에 스레드 스택 반환한다.
* 스레드 커널 오브젝트 사용 카운트도 감소

* 주 스레드 진입점 함수이름은 main, wmain, WinMain, wWinMain
* 문자열 매개변수를 받기 때문에 버전 2개
* 스레드 함수는 반드시 값을 반환해야 한다. - 스레드의 종료코드가 된다.
* 스레드 함수는 가능한 한 함수로 전달된 매개변수와 지역변수만을 사용하도록 작성하는 것이 좋다.
  * 동시접근 문제

## Section 04 CreateThread 함수
주 스레드 외에 추가로 스레드를 생성하고 싶다면 이미 수행중인 스레드에서 CreateThread를 호출하면 된다.

~~~c++
HANDLE CreateThread(
    PSECURITY_ATTRIBUTES psa,
    DWORD cbStackSize,
    PTHREAD_START_ROUTINE pfnStartAddr,
    PVOID pvParam,
    DWORD dwCreateFlags,
    PDWORD pdwThreadID
);
~~~
* CreateThread 함수가 호출되면 시스템은 스레드 커널 오브젝트를 생성
* 스레드 커널 오브젝트는 스레드 자체는 아니며, 운영체제가 스레드를 다루기 위한 데이터 구조체 정도로 생각할 수 있다.
* 다음은 스레드가 사용할 스택을 확보한다.
* 새로운 스레드는 스레드를 생성한 프로세스와 동일한 컨텍스트에서 수행된다.
* 동일 프로세스 내의 스레드들은 손쉽게 상호 통신을 할 수 있다.

1. psa : 보안 특성, NULL 기본 보악 특성
2. cbStackSize : 스레드 스택 크기 기본 1MB(COMMIT 된 초기 크기)
3. pfnStartAddr, pvParam : 스레드 함수, 매개변수
4. dwCreateFlags : 0, 즉시 CPU에 의해 스케줄링, CREATE_SUSPENDED, CPU가 즉시 스케줄되지 않도록 일시정지 상태에 둔다.
5. pdwThreadID : 스레드 ID 값을 저장할 변수의 주소를 준다.


## Section 05 - 스레드의 종료
스레드는 4가지 방법으로 종료될 수 있다.
* 스레드 함수가 반환된다. (이 방법을 강력히 추천)
* 스레드 함수 내에서 ExitThread 함수를 호출한다.
* 동일한 프로세스나 다른 프로세스에서 TerminateThread 함수를 호출한다.
* 스레드가 포함된 프로세스가 종료된다.

### 스레드 함수 반환
스레드를 종료하려는 경우 항상 스레드 함수가 반환되도록 설계하는 것이 좋다.
* 스레드가 사용한 자원을 적절히 정리할 수 있는 유일한 방법

스레드 함수가 반환되면 수행되는 작업
* 스레드 함수 내에서 생성한 모든 C++ 오브젝트가 소멸자를 통해 적절히 제거

### ExitThread 함수
스레드를 강제 종료 시킬 수 있다.
* void ExitThread(DWORD dwExitCode);

스레드를 강제로 종료하고 운영체제가 스레드에서 사용했던 모든 운영체제 리소스를 정리하도록 한다.
* C++ 리소스는 정리되지 않는다.

### TerminateThread 함수
잘 설계된 어플리케이션이라면 이 함수를 호출하지 않을 것이다.
* 종료되는 스레드는 종료 시점까지도 자기 자신이 종료된다는 것을 모른다.


### 프로세스가 종료되면
* 프로세스 내의 모든 스레드가 종료된다.
* 프로세스의 리소스와 스레드 스택 등 자원이 정리된다.
* 프로세스 내의 남아있는 스레드에게 TerminateThread 함수를 호출한다.

### 스레드가 종료되면
* 스레드가 소유하고 있던 모든 유저 오브젝트 핸들이 삭제된다.
* 스레드의 종료 코드가 STILL_ACTIVE에서 ExitThread나 TerminateThread에서 지정한 종료 코드로 변경된다.
* 스레드 커널 오브젝트가 시그널 상태로 변한다.
* 프로세스 내의 마지막 스레드라면 시스템은 프로세스도 종료한 것으로 간주한다.

스레드의 종료 코드를 얻어오는 함수
~~~c++
BOOL GetExitCodeThread(
  HANDLE hThread,
  PDWORD pdwExitCode
);
~~~

* pdwExitCode를 통해 종료 코드를 반환한다.
* 스레드가 종료되지 않았다면 STILL_ACTIVE가 반환된다.
* 함수가 성공하면 TRUE

## Section 06 스레드의 내부

### 스레드 커널 오브젝트
컨텍스트
* SP -> 스레드 스택
* IP -> NTDLL.dll

다른 속성과 통계 정보
* 사용 카운트 = 2
* 정지 카운트 = 1
* 종료 코드 = STILL_ACTIVE
* 시그널 상태 = FALSE

### 스레드 스택
* pvParam : 상위 주소
* pfnStartAddress
* ... : 하위 주소

### NTDLL.dll
VOID RtlUserThreadStart();



### 어떤 작업들이 수행되는가?
1. CreateThread 함수를 호출하면 시스템은 스레드 커널 오브젝트를 생성
2. 각종 속성들 초기화 사용 카운트 : 2, 정지 카운트 : 1
3. 스레드 커널 오브젝트가 생성되고 시스템은 스레드 스택으로 활용할 메모리 공간 할당(프로세스의 주소 공간)
4. 스택에 가장 상위에 두개의 값을 기록한다.
5. 첫 번째 값 : pvParam
6. 두 번째 값 : pfnStartAddr
7. 각 스레드는 자신만의 CPU 레지스터 세트를 가지는데, 이를 스레드 컨텍스트라고 한다.
8. 스레드가 마지막으로 수행되었을 당시의 CPU 레지스터 값을 가지고 있다.
9. CPU 레지스터 세트는 CONTEXT 구조체(WinNT.h) 형태로 스레드 커널 오브젝트 내에 저장된다.
10. IP 레지스터와 SP 레지스터는 스레드 컨텍스트에 저장되는 값 중 가장 중요하다.
11. 스레드 커널 오브젝트가 초기화되면 CONTEXT 구조체 내의 스택 포인터 레지스터는 pfnStartAddr을 저장하고 있는 스레드 스택의 주소로 설정된다.
12. IP 레지스터는 RtlUserThreadStart라는 함수의 주소를 가리키도록 설정된다.
13. 스레드의 초기화가 완료되면 시스템은 CreateThread 함수 호출 시 CREATE_SUSPENDED 플래그가 전달되었는지 확인한다.
14. 플래그가 전달되지 않았다면 정지 카운트를 감소시켜 프로세서에게 스케줄 될 수 있도록 한다.
15. 스레드가 CPU 시간을 얻으면 시스템은 스레드 컨텍스트에 마지막으로 저장된 값을 CPU 레지스터로 로드한다.
16. 스레드는 프로세스 주소 공간 내에 있는 코드를 수행하고 데이터를 변경하는 등의 작업을 수행하게 된다.


RtlUserThreadStart가 기본적으로 수행하는 작업
~~~c++
VOID RtlUserThreadStart(PTHREAD_START_ROUTINE pfnStartAddr, PVOID pvParam)
{
  __try {
    ExitThread((pfnStartAddr) (pvParam));
  }
  __except(UnhandledExceptionFilter(GetExceptionInformation())) {
    ExitProcess(GetExceptionCode());
  }

  // 이 부분은 수행되지 않는다.
}
~~~
* 새로운 스레드의 IP가 RtlUserThreadStart로 설정되어 있기 때문에 실질적으로 스레드가 수행되는 최초 위치
* 다른 함수에 의해 호출되는 구조는 아니다.
* 매개변수들은 운영체제가 임의로 스레드 스택에 삽입한 값

RtlUserThreadStart 함수를 호출하면 수행하는 작업
* 예외처리 프레임 생성
* 스레드 함수 호출
* 스레드 함수가 반환되면 ExitThread 함수 호출
* 스레드가 예외를 유발하고 처리되지 않으면 SEH 프레임이 예외를 처리함

결국 스레드는 RtlUserThreadStart로부터 반환되지 못하고 내부적으로 종료


## Section 07 C/C++ 런타임 라이브러리에 대한 고찰
C/C++ 런타임 라이브러리는 멀티스레드 프로그램에서 사용 가능하도록 작성되지 않았다.
* ex) 전역 변수 errno 등
* 다른 스레드가 다른 스레드의 전역 변수에 접근하지 못하도록 하는 메커니즘이 필요

C/C++ 프로그램이 정상적으로 동작하려면 C/C++ 런타임 라이브러리를 사용하는 각 스레드 별 적절한 구조의 데이터 블록을 생성해야 한다.
* 각자 자신의 데이터 블록에만 접근가능해야 한다.

운영체제는 새로운 스레드가 생성되었을 때 어떻게 데이터 블록을 할당해야할지 모른다.
* 따라서 개발자가 관리해야 한다.
* CreateThread 함수 대신 _beginthreadex 함수 호출


~~~c++
unsigned long _beginthreadex(
  void *security,
  unsigned stack_size,
  unsigned (*start_address)(void *),
  void *arglist,
  unsigned initflag,
  unsigned *thrdaddr
);
~~~
* CreateThread와 비슷하지만 자료형이 조금씩 다르다.
* 명시적으로 캐스팅하여 넘겨준다.

캐스팅 매크로
~~~C++
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, pvParam, fdwCreate, pdwThreadID)                \
      ((HANDLE) _beginthreadex(              \
      (void *) (psa),                        \
      (unsigned) (cbStackSize),              \
      (PTHREAD_START)(pfnStartAddr),         \
      (void *)(pvParam),                     \
      (unsigned) (dwCreateFlags),            \
      (unsigned *) (pdwThreadID)))
~~~

_beginthreadex  에서 주목할 것
* 각 스레드는 C/C++ 런타임 라이브러리 힙에 _tiddata 메모리 블록을 가진다.
* 스레드 함수의 주소, 매개변수는 _tiddata 메모리 블록 내 저장
* 내부적으로 CreateThread 호출
* CreateThread 호출 시 _threadstartex 함수가 수행
* 스레드 핸들 반환, 문제 시 0

_threadstartex에서 주목해야할 부분
* 새로 생성된 스레드는 RtlUserThreadStart를 호출하고 _threadstartex 진입
* _tiddata 구조체가 매개변수로 전달
* 새로 생성된 스레드와 _tiddata 블록을 연계
* _callthreadstartex 함수에서 SEH 프레임 구성
* 사용자 정의 스레드 함수가 수행
* 사용자가 지정한 스레드 함수의 반환값은 곧 종료 코드

_endthreadex 주목할 점
* _tiddata 블록이 삭제되고 스레드를 삭제(ExitThread)한다.

## Section 08 자신의 구분자 얻기
자신의 커널 오브젝트를 얻는 함수
* HANDLE GetCurrentProcess();
* HANDLE GetCurrentThread();

위 함수로 받은 핸들은 허위 핸들이다.
* 이 값을 이용하여 현재 프로세스나 스레드에 대해 자신의 기능을 수행할 수 있다.

### 허위 핸들을 실제 핸들로 변경하기
허위 핸들을 실제 핸들로 변경하는 함수

~~~c++
BOOL DuplicateHandle(
  HANDLE hSourceProcess,
  HANDLE hSource,
  HANDLE hTargetProcess,
  PHANDLE phTarget,
  DWORD dwDesiredAccess,
  BOOL bInheritHandle,
  DWORD dwOptions
);
~~~

* 복사된 핸들도 해제를 해주어야 한다.

