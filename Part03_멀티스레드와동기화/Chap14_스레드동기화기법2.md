# 14장 스레드 동기화 기법 2

## Section 02 - 실행순서에 있어서 동기화
메모리에 접근하는 스레드의 실행순서를 동기화한다.
* 즉, 실행순서 동기화라는 것은 메모리 접근 동기화를 포함하는 개념
* 단, 초점은 실행순서에 맞춰져 있다.

### 생산자/소비자 모델
입출력을 하나의 스레드가 동시에 하는 경우
* 출력속도가 입력속도를 따라가지 못하면
* 어느 순간부터는 문자열이 소멸되기 시작한다.

그렇기 때문에 두 개의 스레드를 활용해서 하나는 입력, 다른 하나를 출력하도록 한다.
* 두 개의 스레드가 입력, 출력 속도에 상관없이 독립적으로 실행

### 이벤트(Event) 기반의 동기화
이벤트(Event) 오브젝트
1. 생산자가 빵을 구워낸 상태가 되면 - signal
2. 이 상태를 감지한 소비자가 빵을 소비한다. - non-signal

이벤트(Event) 오브젝트 생성 함수

HANDLE CreateEvent(\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpEventAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bManualReset,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInitialState,\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpName\
)

* 실패 시 return NULL

1. lpEventAttributes : 보안 속성
2. bManualReset : FALSE - Auto Reset, TRUE - Maual Reset
3. bInitialState : 초기호 상태 여부 TRUE - signaled 상태
4. lpName : 이벤트 오브젝트 이름 지정

WaitForSingleObject() 함수로 임계영역 진입
* WaitForSingleObject 함수가 블로킹되었다면 Signaled 상태가 되는 순간 블로킹된 함수를 빠져나온다.
* 자동 리셋 모드라면 Non-Signaled 상태로의 변경은 자동으로 이루어진다.

수동 리셋 이벤트 (Manual-Reset Event)
1. Non-Signaled - SetEvent -> Signaled 상태
2. Signaled - WaitForSingleObject -> ResetEvent -> Non-Signaled 상태

자동 리셋 이벤트 (Auto-Reset Event)
1. Non-Signaled - SetEvent -> Signaled 상태
2. Signaled - WaitForSingleObject -> Non-Signaled 상태

### 수동 리셋 모드 이벤트의 활용 예
* 두 개의 소비자 스레드가 순서대로 출력을 완료하면 문제가 없다.
* 그러나 문자열을 입력받은 스레드가 이벤트를 Signaled 상태로 변경하면
* 두 개의 스레드가 동시에 일을 시작한다.
* 그럼 실행 순서를 보장받지 못하고 원치않은 결과를 얻을 수도 있다.

추가로 뮤텍스 기반의 동기화 또한 필요한 경우다.


## Section 02 - 이벤트(Event) 더하기 뮤텍스(Mutex)
### 이벤트와 뮤텍스 오브젝트 적용 예제


## Section 03 - 타이머 기반 동기화
* 정해진 시간이 지나면 자동으로 Signaled 상태가 되는 동기화 기법

1. 수동 리셋타이머 : 가장 일반적인 타이머 - 알람 시계 생각
2. 주기적 타이머 : 주기적으로 알람이 울리도록 설정

## 이것만은 알고 갑시다.
1. 실행순서 동기화와 이벤트 오브젝트

2. 타이머 기반 동기화
3. 이벤트 더하기 뮤텍스