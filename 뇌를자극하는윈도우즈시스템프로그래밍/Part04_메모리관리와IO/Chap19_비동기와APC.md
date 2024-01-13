# 19장 비동기 I/O와 APC
## Section 01 - 비동기(Asynchronous) I/O

### 비동기 I/O의 이해
동기(Synchronous) I/O

CPU 동작 시간 ->\
(데이터 수신) -> 플레이 -> (데이터 수신) -> 플레이 -> (데이터 수신)
* 데이터 수신 후 플레이라는 아주 기본적인 원칙을 지키고 있다.
* 데이터 수신 작업은 I/O 작업 -> CPU가 동작하지 않는다. -> 블로킹 상태에 놓인다.
* 플레이 작업은 CPU 작업

블로킹(Blocking) 함수 : 한번 호출되면, 완료될 때까지 블로킹되는 함수
* 동기(Synchronous) I/O : 이러한 함수들을 활용한 입출력 연산

위와 같은 문제점을 해결하기 위한 방법

비동기(Asynchronous) I/O

CPU 동작 시간 ->\
\-\-\-\-\-데이터 수신\-\-\-\-\->\
\-\-\-\-\-\-\-\-플레이\-\-\-\-\-\-\-\->
* IO 작업과 플레이 작업을 병행
* 위와 같은 구조를 비동기(Asynchronous) I/O라 한다.

### 중첩(Overlapped) I/O
* 데이터도 읽으면서 플레이도 동시에 가능하려면?
* 블로킹 함수가 아닌 넌블로킹(Non-Blocking) 함수를 사용해야 한다.
* 넌블로킹 함수는 작업의 완료에 상관없이 바로 반환한다.
* ANSI 표준 함수에는 넌블로킹이라는 개념이 없다.
* 그러나 Windows 표준 함수인 ReadFile 함수는 넌블로킹 방식으로 동작 가능하다.
* I/O 연산을 여러번 중첩시켜 실행하는 것이 중첩(Overlapped) I/O 라고 부른다.

### 중첩(Overlapped) I/O 예제
파이프 통신을 하는 서버를 기준으로\
중첩(Overlapped) I/O 기본 예제
* PIPE(FILE_FLAG_OVERLAPPED) - 인자 전달 -> WriteFile(Function Call) <- 인자 전달 - OVERLAPPED -> EVENT(Signaled state로 바뀌면 감지)


중첩 I/O 기반 파이프 통신 1단계
* 파이프 생성 시 FILE_FLAG_OVERLAPPED를 인자로 넘겨 비동기 특성 부여


중첩 I/O 기반 파이프 통신 2단계\
OVERLAPPED 구조체

typedef struct _OVERLAPPED {\
&nbsp;&nbsp;&nbsp;&nbsp;ULONG_PTR Internal;\
&nbsp;&nbsp;&nbsp;&nbsp;ULONG_PTR InternalHigh;\
&nbsp;&nbsp;&nbsp;&nbsp;union {\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;struct {\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;DWORD Offset;\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;DWORD OffsetHigh;\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;} DUMMYSTRUCTNAME;\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;PVOID Pointer;\
&nbsp;&nbsp;&nbsp;&nbsp;} DUMMYUNIONNAME;\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE  hEvent;\
} OVERLAPPED, *LPOVERLAPPED;

1. Internal, InternalHigh : Windows 시스템이 사용하기 위한 변수
2. Offset, OffsetHigh : 파일 포인터 위치
3. hEvent : 이벤트 오브젝트, 입출력 연산이 완료되었음을 확인하기 위한 용도

입출력 연산이 완료되면 이벤트 오브젝트는 signaled 상태가 된다.


중첩 I/O 기반 파이프 통신 3단계
* WriteFile 혹은 ReadFile 함수 호출
* FILE_FLAG_OVERLAPPED가 설정된 파이프와 OVERLAPPED 구조체 변수의 포인터를 넘긴다.
* WriteFile 함수는 중첩 I/O 방식으로 동작하게 된다.

CreateEvent 함수로 오브젝트를 생성할 때 초기값을 signaled 상태로 주면?
* 어떻게 Non-Signaled 상태로 바꿀까?
* ReadFile, WriteFile 함수 호출 시 자동으로 Non-Signaled 상태가 된다.

GetOverlappedResult 함수 : 내가 요청한 I/O 연산이 잘 마무리 되었는지 확인
* 이 함수로 실제 전송된 바이트 크기를 확인 가능

BOOL GetOverlappedResult(\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hFile,\
&nbsp;&nbsp;&nbsp;&nbsp;LPOVERLAPPED lpOverlapped,\
&nbsp;&nbsp;&nbsp;&nbsp;LPDWORD lpNumberOfBytesTransferred,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bWait\
);


WriteFile 호출 후 결과 값
* 오류를 의미하는 NULL이라면 GetLastError를 호출해서
* ERROR_IO_PENDING인지 아니면 더 심각한 오류인지를 판단해야 한다.

WriteFile 함수를 호출하면 완료처리 방식
* 데이터 전송이 끝나야 X
* 전송을 위해 할당된 내부 메모리 버퍼에 복사가 이뤄지고 나면


### 완료루틴(Completion Routine) 기반 확장 I/O
완료루틴(Completion Routine) 
* I/O 연산이 완료되고 이어지는 루틴 컨트롤

완료루틴 기반 확장 I/O
* I/O 연산 A -> 루틴 B, I/O 연산 C -> 루틴 D 

WriteFile 과 ReadFile 함수 뒤에 Ex가 붙고 마지막 인자에 LPOVERLAPPED_COMPLETION_ROUTINE을 넣는다.


### 알림 가능한 상태(Alertable State)
* I/O 연산이 완료되어서 완료루틴을 실행할 차례가 되었다.
* 그런데 이것이 바로 실행되어야 하는데, Windows는 완료루틴 실행 타이밍을 우리가 결정할 수 있도록 두었다.

DWORD SleepEx(\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwMilliseconds,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bAlertable\
)

1. bAlertable : TRUE가 전달되면 이 함수를 호출한 스레드는 알람 가능한 상태가 된다.

SleepEx함수 외의 알람 가능한 상태로 만드는 함수
* WaitForSingleObject
* WaitForMultipleObject


### 지금까지의 내용 정리
* 블로킹 I/O 함수를 호출하게 되면 함수를 반환하는 시점과 I/O 완료시점이 일치하기 때문에 동기 I/O라 한다.
* 중첩 I/O와 완료루틴 I/O는 비동기
* 완료루틴 I/O는 I/O가 완료되면 완료루틴이 호출되고
* 중첩 I/O는 이러한 장치가 없기 때문에 중간에 I/O가 완료되었음을 확인하는 과정이 필요하다.

## Section 02 - APC(Asynchronous Procedure Call)
APC는 비동기 함수 호출 메커니즘을 의미
* 완료루틴 또한 APC를 활용하여 구현

### APC의 구조
APC는 2가지 종류로 나뉜다.
1. User-mode APC
2. Kernel-mode APC - 책의 범위를 벗어남
   1. Normal kernel-mode APC
   2. Special kernel-mode APC

(Fuction & Param) -> WriteFileEx() -> I/O 완료 시 -> APC Queue(Fuction & Param 넣음) -> 실행

모든 스레드는 자신만의 APC Queue를 가지고 있다.
* APC Queue는 스레드마다 독립적이다.
* APC Queue에는 비동기적으로 호출되어야 할 함수들과 매개변수 정보가 저장된다.
* 저장된다고 해서 바로 실행되는 것은 아니고 알람가능한 상태가 되어야 한다.

## 이것만은 알고 갑시다.
1. 다음 여섯 단어를 충분히 설명하는 하나의 문장을 만들어봅시다.
   * 비-동기 I/O, 동기 I/O, 블로킹 함수, 넌블로킹 함수, 중첩(Overlapped), 완료루틴
2. 완료루틴 I/O, 중첩 I/O