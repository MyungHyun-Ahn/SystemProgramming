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