# Chapter 09 커널 오브젝트를 이용한 스레드 동기화
지난 챕터에서는 유저 모드에서 스레드 동기화를 어떻게 수행하는지 알아보았다.
* 유저 모드의 최대 장점은 빠르다는 것이다.
* 스레드의 수행 성능이 중요한 경우라면 항상 유저 모드 스레드 동기화 메커니즘을 가장 먼저 고려해보아야 한다.

유저 모드 스레드 동기화 메커니즘이 최상의 성능을 제공하기는 하지만 한계점이 있다.
* 복잡한 작업을 수행하기에는 적절하지 않은 부분이 있다.
* ex 인터락 계열의 함수는 단 하나의 값만 처리 가능, 스레드를 대기 상태로 전환하지 못한다.
* 크리티컬 섹션을 이용하면 대기 상태로 전환할 수는 있지만, 단일 프로세스 내의 스레드들 사이에서만 동기화를 수행할 수 있다.
* 크리티컬 섹션을 사용하면 대기 시간을 설정할 수 없기 때문에 데드락 상태에 빠지게 된다.

커널 오브젝트 스레드 동기화 메커니즘
* 성능이 그다지 좋지 못한다.
* 유저 모드에서 커널 모드로의 전환이 필요하다. - x86에서 약 200CPU 사이클 정도가 필요한 비싼 작업

실제로 대부분의 커널 오브젝트는 동기화를 위해 사용될 수 있다.
* 이를 위해 모든 커널 오브젝트는 시그널 상태와 논시그널 상태가 될 수 있다.

프로세스 커널 오브젝트는 오브젝트 생성 시 FALSE(논시그널)로 초기화되는 부울 값이 있다.
* 프로세스가 종료되면 운영체제에 의해 자동으로 TRUE 값이 된다.

스레드 커널 오브젝트 또한 동일한 규칙을 따른다.
* 생성 시 FALSE, 종료 시 TRUE

시그널 상태나 논시그널 상태가 될 수 있는 커널 오브젝트
* 프로세스
* 스레드
* 잡
* 파일과 콘솔에 대한 표준 입력/출력/에러 스트림
* 이벤트
* 대기 타이머
* 세마포어
* 뮤텍스

스레드는 특정 오브젝트가 시그널 상태가 될 때까지 자신을 대기 상태로 만들 수 있다.


## Section 01 대기 함수들
대기 함수(Wait Function)를 호출하면 인자로 전달한 커널 오브젝트가 시그널 상태가 될 때까지 이 함수를 호출한 스레드를 대기 상태로 유지한다.
* 이미 커널 오브젝트가 시그널 상태였다면 대기 상태로 전환되지 않는다.

가장 많이 쓰이는 것은 WaitForSingleObject이다.
~~~C++
DWORD WaitForSingleObject(
    HANDLE hObject,
    DWORD dwMilliseconds
);
~~~
* 첫 번째 매개변수는 시그널과 논시그널이 될 수 있는 커널 오브젝트의 핸들을 전달
* 두 번째 매개변수는 얼마나 기다릴 것인지 시간 값
    * INFINITE 혹은 ms 단위의 시간을 전달한다.
    * INFINITE로 지정한 경우 영원히 스레드가 깨어나지 못할 수도 있다.

WaitForSingleObject의 반환 값은 이 함수를 호출한 스레드가 어떻게 다시 스케줄 가능하게 되었는지를 알려준다.
* WAIT_OBJECT_0 : 대기하던 오브젝트가 시그널이 되었다면
* WAIT_TIMEOUT : 타임아웃
* WAIT_FAILED : 잘못된 인자를 전달한 경우

여러 개의 커널 오브젝트들에 대해 시그널 상태를 동시에 검사할 수 있는 함수
~~~C++
DWORD WaitForMultipleObjects (
    DWORD dwCount,
    CONST HANDLE *phObjects,
    BOOL bWaitAll,
    DWORD dwMilliseconds
);
~~~
* dwCount : 검사해야할 커널 오브젝트의 개수 MAXIMUM_WAIT_OBJECTS (64) 최대값
* phObjects : 커널 오브젝트 핸들의 배열을 가리키는 포인터
* bWaitAll : TRUE - 모든 오브젝트가 시그널이 될때까지 대기, FALSE - 하나만 시그널이어도 빠져나옴
* dwMilliseconds : WaitForSingleObject와 동일

bWaitAll 이 FALSE인 경우 어떤 오브젝트가 시그널인지 확인하는 방법
* WAIT_TIMEOUT 혹은 WAIT_FAILED가 아닌 경우
* 반환 값에서 WAIT_OBJECT_0을 뺀 값이 핸들 오브젝트 배열의 인덱스가 된다.
* 다음에 또 호출되는 경우 이미 시그널이 된 커널 오브젝트를 배열에서 빼주어야 한다.

## Section 02 성공적인 대기의 부가적인 영향
성공적인 호출이란 함수 호출 시 매개변수로 전달한 커널 오브젝트가 시그널 상태가 되어 WAIT_OBJECT_0를 반환하는 경우

성공적이지 않은 호출이란 WAIT_TIMEOUT이나 WAIT_FAILED를 반환하는 경우
* 이때는 오브젝트의 상태가 변경되지 않는다.

성공적인 호출을 통해 오브젝트 상태가 변경되는 것 : 성공적인 대기의 부가적인 영향 (successful wait side effect)
* auto-reset인 이벤트 커널 오브젝트의 경우 : WAIT_OBJECT_0을 반환하고, 반환되지 직전에 side effect로 이벤트 커널 오브젝트는 논시그널 상태가 될 것이다.

WaitForMultipleObjects는 모든 작업을 원자적으로 수행하기 때문에 상당히 유용하다.
* 모든 오브젝트를 확인하고 성공적인 대기로 오브젝트의 상태 변경까지 원자적으로 수행한다.
* 이 함수가 커널 오브젝트들의 상태를 확인하는 시점에는 다른 스레드들이 오브젝트의 상태를 변경하지 못한다.
* 데드락이 발생하는 것을 미연에 방지할 수 있다.

만일 다수의 스레드가 하나의 커널 오브젝트를 대기하고 있는 경우라면 시그널 되었을 때 어떤 스레드를 깨울 것인가?
* 마이크로소프트는 "알고리즘은 공평하다" 라는 답변을 내놓았다.
* 내부적인 알고리즘이 스레드 우선순위에 영향을 받지 않음을 의미한다.
* 실제로, 지금까지 마이크로소프트가 사용하고 있는 알고리즘은 "선입선출" 방식이다.

## Section 03 이벤트 커널 오브젝트
모든 커널 오브젝트 중 이벤트가 가장 단순한 구조를 가지고 있다.
* 이벤트는 Usage Count(모든 커널 오브젝트가 가진)
* 자동 리셋(auto-reset) 모드인지 수동 리셋(manual-reset)인지를 판별하는 bool 값
* 이벤트의 시그널 상태 여부를 나타내는 bool 값으로 이뤄져 있다.


수동 리셋 이벤트가 시그널 상태가 되면 이 이벤트를 대기 중인 모든 스레드는 동시에 스케줄 가능 상태가 된다.

자동 리셋 이벤트의 경우 대기 중인 스레드 중 단 하나의 스레드만이 스케줄 가능 상태가 된다.


이벤트를 생성하는 함수
~~~C++
HANDLE CreateEvent(
    PSECURITY_ATTRIBUTES psa,
    BOOL bManualReset,
    BOOL bInitialState,
    PCTSTR pszName
);
~~~
* bManualReset : manual-reset - TRUE, auto-reset - FALSE
* bInitialState : 초기 상태 시그널 - TRUE, 논시그널 - FALSE

윈도우 비스타 이후에는 이벤트를 생성하기 위한 새로운 함수인 CreateEventEx를 제공한다.
~~~C++
HANDLE CreateEventEx(
    PSECURITY_ATTRIBUTES psa,
    PCTSTR pszName,
    DWORD dwFlags,
    DWORD dwDesiredAccess
);
~~~
* dwFlags
  * CREATE_EVENT_INITIAL_SET : 이 값이 지정되면 시그널 상태로 이벤트를 생성
  * CREATE_EVENT_MANUAL_RESET : 이 값이 지정되면 Manual-Reset 이벤트를 생성
* dwDesiredAccess : 이벤트 핸들을 통한 오브젝트에 접근하는 권한
  * SetEvent, ResetEvent, PulseEvent 함수들을 사용하려면 EVENT_MODIFY_STATE 플래그가 설정되어야 한다.

다른 프로세스에서 수행되는 스레드의 경우 pszName을 통해접근 하거나 DuplicateHandle 함수 등을 사용하여 접근할 수 있다.


OpenEvent 함수에 pszName 매개변수에서 지정한 이름을 매개변수로 전달하여 동일한 이벤트에 접근 가능하다.
~~~C++
HANDLE OpenEvent(
    DWORD dwDesiredAccess,
    BOOL bInherit,
    PCTSTR pszName
);
~~~

이벤트를 더 이상 사용하지 않는다면 CloseHandle(); 호출

이벤트를 시그널 상태로 변경하는 함수
~~~C++
BOOL SetEvent(HANDLE hEvent);
~~~

이벤트를 논시그널 상태로 변경하는 함수
~~~C++
BOOL ResetEvent(HANDLE hEvent);
~~~

자동 리셋 이벤트에서는 성공적인 대기가 이뤄지면 자동으로 논시그널 상태가 된다.


이벤트를 시그널 상태로 변경하였다가 곧바로 논시그널 상태로 변경하는 함수
~~~C++
BOOL PulseEvent(HANDLE hEvent);
~~~
* 별로 유용하지는 않다.

## Section 04 대기 타이머 커널 오브젝트
대기 타이머(Waitable timer)는 특정 시간에 혹은 일정한 간격을 두고 자신을 시그널 상태로 만드는 커널 오브젝트
* 특정 시간에 맞추어 어떤 작업을 수행해야 할 경우 사용

대기 타이머를 생성하는 함수
~~~C++
HANDLE CreateWaitableTimer(
    PSECURITY_ATTRIBUTES psa,
    BOOL bManualReset,
    PCTSTR pszName
);
~~~

다른 프로세스에서 이미 생성된 대기 타이머를 가리키는 프로세스 고유 핸들 값을 얻는 함수
~~~C++
HANDLE OpenWaitableTimer(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    PCTSTR pszName
);
~~~

* bManaulReset : 수동 리셋, 자동 리셋 모드 설정
* 대기 타이머는 항상 논 시그널 상태로 생성된다.

언제 시그널 상태가 될 것인지 지정하는 함수
~~~C++
BOOL SetWaitableTimer(
    HANDLE hTimer,
    const LARGE_INTEGER *pDueTime,
    LONG lPeriod,
    PTIMERAPCROUNTINE pfnCompletionRoutine,
    PVOID pvArgToCompletionRoutine,
    BOOL bResume
);
~~~
* hTimer : 대기 타이머 핸들 값
* pDueTime : 시그널 상태가 되는 최초 시간
* lPeriod : 그 후 얼마의 주기로 시그널 상태가 될 것인지
* pfnCompletionRoutine
* pvArgToCompletionRoutine
* bResume 

~~~C++
// 지역변수를 선언한다.
HANDLE hTimer;
SYSTEMTIME st;
FILETIME ftLocal, ftUTC;
LARGE_INTEGER liUTC;

// 자동 리셋 타이머를 생성한다.
hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

// 지역 시간으로 2008년 1월 1일 오후 1:00에 최초 시그널 상태가 된다.
st.wYear            = 2008;
st.wMonth           = 1;
st.wDayOfWeek       = 0;
st.wDay             = 1;
st.wHour            = 13;
st.wMinute          = 0;
st.wSecond          = 0;
st.wMilliseconds    = 0;

SystemTimeToFileTime(&st, &ftLocal);

// 지역 시간을 UTC 시간으로 변경한다.
LocalFileTimeToFileTime(&ftLocal, &ftUTC);
// FILETIME을 LARGE_INTEGER로 변경한다.
// 이는 두 개의 타입이 서로 다른 데이터 정렬을 수행하기 때문이다.
liUTC.LowPart = ftUTC.dwLowDateTime;
liUTC.HighPart = ftUTC.dwHighDateTime;

// 대기 타이머를 설정한다.
SetWaitableTimer(hTimer, &liUTC, 6 * 60 * 60 * 1000, NULL, NULL, FALSE); ...
~~~

SYSTEMTIME 구조체를 이용하여 시간을 초기화
* 머신의 타임 존에 맞는 시간을 설정
* 두 번째 매개변수 타입은 const LARGE_INTEGER* 이므로 SYSTEMTIME 구조체를 직접 사용할 수 없다.
* FILETIME 구조체와 LARGE_INTEGER 구조체는 둘 다 동일하게 두 개의 32비트 값을 필드로 가지므로
* SYSTEMTIME 구조체의 값을 FILETIME 구조체로 변경하면 된다.

다른 문제점은 SetWaitableTimer가 시간을 항상 UTC를 사용한다는 것
* LocalFileTimeToFileTime 함수를 이용하면 변경을 쉽게할 수 있다.
~~~c++
SetWaitableTimer(hTimer, (PLARGE_INTEGER) &ftUTC, 6 * 60 * 60 * 1000, NULL, NULL, FALSE);
~~~
위와 같이 코드를 작성하면 안된다.
* FILETIME과 LARGE_INTEGER 구조체가 동일한 이진 구조를 가지고 있기는 하지만
* FILETIME 구조체는 32비트 경계로 정렬을 수행한다.
* LARGE_INTEGER 구조체는 64비트 경계로 정렬을 수행한다.
* 확실히 하기 위해서는 LARGE_INTEGER의 멤버에 FILETIME의 멤버를 복사한 후 SetWaitableTimer에 전달해야 한다.


lPeriod 매개변수
* 최초 시그널 시간을 과거로 지정해도 실패하지 않는다.

타이머가 시그널 상태가 되어야 하는 절대 시간을 설정하지않고 상대 시간을 이용하여 SetWaitableTimer를 호출할 수 있다.
* 이 경우 pDueTime 매개변수로 음수 값을 전달하면 된다. - 100나노초 단위
* 1초 = 1,000밀리초 = 1,000,000마이크로초 = 10,000,000 100 나노초

SetWaitableTimer를 호출하고 5초 후에 타이머를 시그널 상태로 만들기 위한 예제
~~~C++
// 지역변수를 선언한다.
HANDLE hTimer;
LARGE_INTEGER li;

// 자동 리셋 타이머를 생성한다.
hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

// SetWaitableTimer를 호출한 후 5초가 경과하면 타이머가 시그널 상태로 바뀐다.
const int nTimerUnitsPerSecond = 10000000;

// 음수로 시간을 설정하면 SetWaitableTimer는
// 절대 시간 대신 상대 시간을 사용하는 것으로 판단한다.
li.QuadPart = -(5 * nTimerUnitsPerSecond);

// 타이머를 설정한다.
SetWaitableTimer(hTimer, &li, 6 * 60 * 60 * 1000, NULL, NULL, FALSE); ...
~~~
* lPeriod 값을 0으로 전달하면 한 번만 시그널 상태가 되고 주기적으로 시그널 상태가 되지 않는다.

beResume은 컴퓨터를 대기 상태로 유지하거나 대기 상태로 빠져나오는데 사용한다.
* FALSE가 전달되었다면 대기 타이머는 시그널 상태가 되긴 하겠지만 어떠한 스레드도 CPU 시간을 받지 못하고 대기 상태로 남아있게 된다.
* 나중에 컴퓨터가 대기 상태를 빠져나오게 되면(보통 사용자에 의해) 그때 비로소 대기 타이머를 기다리던 스레드가 수행을 재개하게 된다.

CancelWaitableTimer
~~~C++
BOOL CancelWaitableTimer(HANDLE hTimer);
~~~
* SetWaitableTimer를 재호출할 때까지 타이머가 시그널되지 못하게 한다.
* 타이머의 시그널 기준만을 변경하고 싶다면 SetWaitableTimer를 호출하기 전에 굳이 CancelWaitableTimer를 호출할 필요가 없다.
* SetWaitableTimer만 호출해도 기존에 설정된 시그널 기준이 모두 취소되기 때문이다.

### 대기 타이머를 이용하여 APC 요청을 스레드의 APC 큐에 삽입하는 방법
타이머가 시그널 상태가 될 때까지 대기하기 위해 WaitForSingleObject와 WaitForMultipleObjects 함수를 어떻게 사용하는지에 대해서 알아보았다.

이와는 별개로 마이크로소프트는 SetWaitableTimer를 이용하여 타이머가 시그널 상태가 되었을 때 비동기 함수 호출(Asynchronous Procedure call) 요청을 스레드의 APC 큐애 삽입하는 방법을 제공하고 있다.
* 보통의 경우에는 SetWaitableTimer를 호출할 때 pfnCompletionRoutine과 pvArgToCompletionRoutine의 매개변수에는 NULL 값을 지정한다.
* NULL로 지정하는 이유는 타이머가 시그널 상태가 되는 시점만을 알면 되기 때문이다.
* 하지만 매개변수로 APC 루틴을 전달하면 타이머가 시그널 상태가 되었을 때 APC 요청을 스레드의 APC 큐에 삽입해준다.

~~~C++
VOID APIENTRY TimerAPCRoutine(
    PVOID pvArgToCompletionRoutine,
    DWORD dwTimerLowValue,
    DWORD dwTimerHighValue
);
~~~
* TimerAPCRoutine이라는 이름은 다른 이름으로 변경해도 된다.
* 이 함수는 타이머가 시그널 상태가 되고 SetWaitableTimer를 호출한 스레드가 Alertable 상태에 있는 경우 호출하였던 스레드에 의해 호출된다.
* 스레드를 Alertable 상태로 만들기 위해 SleepEx, WaitForSingleObjectEx 등의 함수를 호출하면 된다.
* 스레드가 위와 같은 함수를 호출하지 않아서 Alertable 상태가 있지 않는 경우라면 시스템은 타이머가 시그널 상태가 되어도 APC 요청을 APC 큐에 삽입하지 않는다.
* 이는 타이머에 의해 필요 없는 APC 요청을 스레드의 APC 큐에 쌓아둠으로써 메모리를 낭비하는 것을 막기 위함이다.

만일 스레드가 Alertable 상태에서 대기 중이고 타이머가 시그널 상태가 되면
* 시스템은 이 스레드를 이용하여 APC 콜백 루틴을 호출한다.
* 콜백 루틴의 첫 번째 매개변수에는 pvArgToCompletionRoutine 매개변수로 전달한 값과 동일한 값이 전달된다.
* 나머지 dwTimerLowValue, dwTimerHighValue로는 타이머가 언제 시그널 상태가 되었는지를 알려주는 정보가 전달된다.

이러한 정보를 사용자에게 보여주는 예제
~~~C++
VOID APIENTRY TimerAPCRoutine(PVOID pvArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
    FILETIME ftUTC, ftLocal;
    SYSTEMTIME st;
    TCHAR szBuf[256];

    // 시간 정보를 FILETIME 구조체에 삽입한다.
    ftUTC.dwLowDateTime = dwTimerLowValue;
    ftUTC.dwHighDateTime = dwTimerHighValue;

    // UTC 시간을 사용자의 지역 시간으로 변경한다.
    FileTimeToSystemTime(&ftUTC, &ftLocal);

    // GetDateFormat 과 GetTimerFormat 함수에서 사용할 수 있도록
    // FILETIME을 SYSTEMTIME 구조체로 변경한다.
    FileTimeToSystemTime(&ftLocal, &st);

    // 타이머가 시그널 상태가 된 시간을 
    // 날짜/시간 형태의 문자열로 변경한다.
    GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, szBuf, _countof(szBuf));
    _tcscat_s(szBuf, _countof(szBuf), TEXT(""));
    GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, _tcschr(szBuf, TEXT('\0')), (int)(_countof(szBuf) - _tcslen(szBuf)));

    // 사용자에게 시간을 보여준다.
    MessageBox(NULL, szBuf, TEXT("Timer went off at..."), MB_OK);
}
~~~

* 큐에 있는 APC 요청들이 모두 처리되어야 비로소 Alertable 함수가 반환되기 때문에 TimerAPCRoutine 함수는 다음번 APC 요청이 APC 큐에 삽입되기 전에 처리를 마치고 반환되도록 작성되어야 한다.

타이머와 APC를 적절하게 사용하는 코드의 예
~~~C++
void SomeFunc()
{
    // 타이머를 생성한다. (수동 리셋이든 자동 리셋이든 상관없다.)
    HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);

    // 타이머의 시그널 설정 주기를 5초로 한다.
    LARGE_INTEGER li = { 0, };
    SetWaitableTimer(hTimer, &li, 5000, TimerAPCRoutine, NULL, FALSE);

    // 타이머가 시그널되기 전에 스레드를 Alertable 상태로 만든다.
    SleepEx(INFINITE, TRUE);
    CloseHandle(hTimer);
}
~~~

마지막으로, 스레드는 단일의 타이머 핸들에 대해 타이머 커널 오브젝트에 대한 시그널 대기와 Alertable 상태 대기를 동시에 수행해서는 안된다.
~~~C++
HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
SetWaitableTimer(hTimer, ..., TimerAPCRoutine, ...);
WaitForSingleObjectEx(hTimer, INFINITE, TRUE);
~~~
* WaitForSingleObjectEx는 커널 오브젝트 핸들의 시그널 대기와 얼러터블 상태 대기와 같이 2번의 대기를 수행하는 꼴이 된다.
* 만일 위와 같이 코드를 작성하고 타이머가 시그널 상태가 되면 성공적인 대기가 수행되어 스레드가 깨어나게 되고 Alertable 상태에서 벗어나기 때문에 APC 루틴은 호출되지 않는다.


## Section 05 세마포어 커널 오브젝트
세마포어(Semaphore) 커널 오브젝트는 리소스의 개수를 고려해야 하는 상황에서 주로 사용된다.
* 2개의 32비트 값을 가지고 있다.
* 최대 리소스 카운트(Maximum Resource Count) : 세마포어가 제어할 수 있는 리소스의 최대 개수
* 현재 리소스 카운트(Current Resource Count) : 사용 가능한 리소스의 개수를 나타내는데 사용

예를 들어 다음과 같은 상황에서 사용된다.
* 서버에 동시에 5명까지 입장가능하게 제한하고 싶은 상황
* 6번째 사용자가 접속하면 에러 메시지를 보내려 한다고 할 때
* 세마포어의 리소스 카운트를 5로 설정하고 리소스 카운트를 초과할 때 에러 메시지를 보낸다.

세마포어는 다음의 규칙에 따라 동작한다.
* 현재 리소스 카운트가 0보다 크면 세마포어는 시그널 상태가 된다.
* 현재 리소스 카운트가 0이면 세마포어는 논시그널 상태가 된다.
* 시스템은 현재 리소스 카운트를 음수로 만들 수 없다.
* 현재 리소스 카운트는 최대 리소스 카운트보다 커질 수 없다.


세마포어 커널 오브젝트 생성 함수
~~~C++
HANDLE CreateSemaphore(
    PSECURITY_ATTRIBUTE psa,
    LONG lInitialCount,
    LONG lMaximumCount,
    PCTSTR pszName
);

// dwDesiredAccess 매개변수로 접근 권한을 설정할 수 있다.
HANDLE CreateSemaphoreEx(
    PSECURITY_ATTRIBUTE psa,
    LONG lInitialCount,
    LONG lMaximumCount,
    PCTSTR pszName,
    DWORD dwFlags,
    DWORD dwDesiredAccess
);
~~~
* dwFlags : 항상 0으로 설정

이미 생성된 세마포어의 핸들 값을 얻어오는 함수
~~~C++
HANDLE OpenSemaphore(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    PCTSTR pszName
);
~~~

* lMaximumCount : 최대 21억까지의 리소수 개수를 지정할 수 있다.


~~~C++
HANDLE hSemaphore = CreateSemaphore(NULL, 0, 5, NULL);
~~~
* lInitialCount를 0으로 지정하였기 때문에 세마포어 커널 오브젝트는 논시그널 상태가 된다.
* 따라서 세마포어가 시그널 상태가 되길 기다리는 스레드들은 대기 상태가 된다.

스레드가 리소스에 대한 접근을 요청하기 위해 대기 함수를 호출할 때는 세마포어의 핸들을 전달하면 된다.
* 대기 함수는 세마포어의 값이 0보다 크면 세마포어의 값을 1만큼 감소시키고 대기 함수를 호출한 스레드를 스케줄 가능 상태로 만든다.
* 물론 세마포어의 현재 리소스 카운트 값을 확인하고 그 값을 변경하는 등의 모든 동작은 원자적으로 수행된다.


세마포어의 현재 리소스 카운트를 증가시키기 위해서는 ReleaseSemaphore 함수를 호출하면 된다.
~~~C++
BOOL ReleaseSemaphore(
    HANDLE hSemaphore,
    LONG lReleaseCount,
    PLONG plPreviousCount
);
~~~
* lReleaseCount에 전달된 값만큼 현재 리소스 카운트 값을 증가시키는 역할을 수행한다.
* plPreviousCount로 증가시키기 이전의 값을 받아올 수 있다. - 이 값을 사용하는 경우는 상당히 드물다.

lReleaseCount 값을 0으로 전달하고 plPreviousCount 값만으로 이전 값을 받아오는 작업은 불가능하다.
* 0을 리턴한다. FALSE
* 세마포어의 값을 변경하지 않고 이전의 값만 받아오는 동작을 불가능하다.

## Section 06 뮤텍스 커널 오브젝트
뮤텍스 커널 오브젝트는 스레드가 단일의 리소스에 대해 베타적으로 접근할 수 있도록 해준다.
* MUTual EXclusion 이라는 특성으로부터 이름이 나온 것

뮤텍스가 가진 요소
* 사용 카운트
* 스레드 ID : 시스템 내의 어떤 스레드가 뮤텍스를 소유하고 있는지 
* 반복 카운터 : 뮤텍스를 소유하고 있는 스레드가 몇 회나 반복적으로 뮤텍스를 소유하고자 했는지

동작 방식은 크리티컬 섹션과 동일하다.
* 크리티컬 섹션은 유저 모드 동기화 오브젝트이고, 뮤텍스는 커널 모드 동기화 오브젝트이다.
* 경쟁 상황이 발생한 경우에는 예외다.

뮤텍스는 크리티컬 섹션에 비해 느리지만,
* 서로 다른 프로세스에서 동일 뮤텍스에 대해 접근이 가능
* 리소스에 대한 접근 권한을 획득할 때 시간 제한을 지정할 수 있다는 장점


뮤텍스는 다양한 용도로 사용될 수 있다.
* 다수의 스레드가 동시에 접근하는 메모리 블록을 보호하기 위해 사용
* 공유 데이터의 무결성을 유지 가능

뮤텍스의 동작 규칙
* 스레드 ID가 0(유효하지 않은 스레드 ID)면 뮤텍스는 시그널 상태 - 어떠한 스레드도 소유하지 않은 상태
* 스레드 ID가 0이 아니면 논시그널 상태 - 특정 스레드가 소유한 상태
* 다른 커널 오브젝트와는 달리 뮤텍스는 특수한 코드를 포함하고 있어서 일반적인 규칙을 위반하는 경우도 있다.

뮤텍스 생성 함수
~~~C++
HANDLE CreateMutex(
    PSECURITY_ATTRIBUTES psa,
    BOOL bInitialOwner,
    PCTSTR pszName
);

HANDLE CreateMutexEx(
    PSECURITY_ATTRIBUTES psa,
    PCTSTR pszName,
    DWORD dwFlags,
    DWORD dwDesiredAccess
);
~~~
* dwDesiredAccess : 뮤텍스의 접근 권한을 바로 지정
* bInitialOwner : FALSE일 경우 스레드ID와 반복 카운터는 0으로 설정, TRUE일 경우 ID - 호출 스레드, 반복 카운터 1, 논시그널 상태
* dwFlags : bInitialOwner와 동일한 의미 0은 FALSE, CREATE_MUTEX_INITIAL_OWNER는 TRUE

이미 생성된 뮤텍스를 가리키는 프로세스 고유의 핸들 값을 얻는 함수
~~~C++
HANDLE OpenMutex(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    PCTSTR pszName
);
~~~


공유 리소스에 접근하려는 스레드의 경우 뮤텍스의 핸들을 이용하여 대기 함수를 호출
* 시그널 상태면 스레드 ID를 대기 함수 호출 스레드로 설정 반복 카운터는 1로 설정
* 논시그널 상태면 대기 상태, 스레드가 시그널 상태가되면 즉시 위 과정 수행
* 뮤텍스 커널 오브젝트의 상태 확인과 변경 작업은 항상 원자적


뮤텍스는 일반적인 커널 오브젝트의 시그널 규칙과는 다른 예외사항이 있다.
* 뮤텍스의 경우 스레드 ID값이 대기 함수를 호출한 스레드의 ID 값과 동일하면 논시그널 상태임에도 스레드를 스케줄 가능 상태로 만든다.
* 다른 커널 오브젝트에서는 발견할 수 없는 특수한 것
* 반복 카운터가 1을 초과하는 경우는 동일 스레드가 여러번 대기 함수를 호출한 것

뮤텍스의 소유권을 해제하는 함수
~~~C++
BOOL ReleaseMutex(HANDLE hMutex);
~~~
* 뮤텍스의 반복 카운터 값을 1 감소시킨다.
* 동일 스레드가 여러번 호출한 경우 ReleaseMutex 또한 여러번 호출해야 한다.

뮤텍스 오브젝트가 시그널 상태가 되면 시스템은 동일 뮤텍스를 기다리고 있는 다른 뮤텍스를 기다리고 있는 다른 스레드들이 있는지 확인
* 대기 중 스레드가 여러개라면 이 중 하나를 공평하게 선택하여 뮤텍스를 소유하도록 한다.
* 누구도 대기하고 있지 않다면 시그널 상태를 유지

### 버림 문제 (Abandonment issues)
만약 뮤텍스를 소유한 스레드가 소유권을 해제하지 않고 스레드를 종료하면 어떻게 될까?
* ExitThread 등등
* 시스템은 뮤텍스가 버려졌다고 판단한다.
* 뮤텍스의 버림 : 뮤텍스를 소유한 스레드가 종료되어 소유권을 해제하지 못하는 상황

시스템은 뮤텍스와 스레드 커널 오브젝트를 계속해서 추적하고 있기 때문에 언제 뮤텍스가 버려졌는지 알 수 있다.
* 뮤텍스의 버림이 발생하면 버려진 뮤텍스의 스레드 ID와 반복 카운터를 0으로 설정한다.
* 이후 대기 스레드 중 하나를 공평하게 선택하여 스레드를 스케줄한다.

위 경우에서 대기 함수는 WAIT_OBJECT_0이 아닌 WAIT_ABANDONED라는 특별한 값을 반환한다.
* 이런 상황은 정상적인 상황이 아니기 때문에 대체 방법을 가지고 있어야 한다.

### 뮤텍스와 크리티컬 섹션
특성 : 뮤텍스 : 크리티컬 섹션
* 성능 : 느림 : 빠름
* 프로세스 간 사용 여부 : 가능 : 불가능
* 선언 : HANDLE hmtx; : CRITICAL_SECTION cs;
* 초기화 : CreateMutex : InitializeCriticalSection
* 삭제 : CloseHandle : DeleteCriticalSection
* 무한 대기 : WaitForSingleObject(h, INFINITE) : EnterCriticalSection
* 0 대기 : WaitForSingleObject(h, 0) : TryEnterCriticalSection
* 임의 시간 대기 : WaitForSingleObject(h, ms) : 불가능
* 해제 : ReleaseMutex : LeaveCriticalSection
* 다른 커널 오브젝트와 함께 대기 가능 여부 : 가능 : 불가능