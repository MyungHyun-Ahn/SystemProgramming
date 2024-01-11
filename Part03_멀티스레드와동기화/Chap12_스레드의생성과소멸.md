# 12장 스레드의 생성과 소멸
## Section 01 - Windows에서의 스레드 생성과 소멸
### 스레드의 생성
CreateThread 함수
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE CreateThread(\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpThreadAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;SIZE_T dwStackSize,\
&nbsp;&nbsp;&nbsp;&nbsp;LPTHREAD_START_ROUTINE lpStartAddress,\
&nbsp;&nbsp;&nbsp;&nbsp;LPVOID lpParameter,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwCreationFlags,\
&nbsp;&nbsp;&nbsp;&nbsp;LPDWORD lpThreadId\
)

* 실패 시 NULL 반환, 성공하면 Thread 핸들 반환

1. lpThreadAttributes : 핸들의 상속 여부 결정, NULL 전달 시 상속 안함
2. dwStackSize : 스레드 생성 시 스택 크기 결정, 0 전달 시 Default : 1mb
3. lpStartAddress : 스레드의 main 함수, 함수 포인터, 반환타입 DWORD, 매개변수 LPVOID
4. lpParameter : 스레드 함수에 전달할 인자, main 함수의 argv와 비슷
5. dwCreationFlags : 스레드의 생성 및 실행을 조절하기 위한 인자
   * CREATE_SUSPENDED : 스레드 생성과 동시에 Blocked 상태 ResumeThread가 호출되면 실행 시작
   * STACK_SIZE_PARAM_IS_A_RESERVATION : dwStackSize로 전달되는 값의 크기는 reserve 메모리 크기를 의미, 아닐 경우 commit 메모리 크기를 의미
6. lpThreadId : 스레드 ID를 전달받기 위한 변수의 주소값, 필요없다면 NULL

스레드를 생성할 수 있는 최대 개수는?
* 메모리가 허용하는 만큼


스레드와 main 함수의 종료
* 스레드 함수의 return 문은 스레드의 종료
* main 함수의 return 문은 프로세스의 종료로 이어짐

10638개 만들고 프로그램이 뻗음
* 그런데 여러 스레드를 실행시키고 출력을 진행하면 어떤 것이 먼저 실행될지 알 수 있을까?
* 결론은, 알 수 없다.


Sleep 함수
* 인자로 Millisecond 단위로 지정
* 0을 인자로 전달할 경우, 자신에게 할당된 타임 슬라이스를 포기하고 우선순위가 같은 다른 스레드에게 실행 기회 양보


### 스레드의 소멸(스레드 생성에 대한 추가적인 이야기 포함)

Case 1 : 스레드 종료 시 return을 이용하면 좋은 경우 (거의 대부분의 경우)
* 1 ~ 10 까지 더하는 경우 - 입출력 작업이 빈번하다고 가정
* Blocked 상태에 자주 진입, 그런데 빨리 끝내기 위해 세개의 스레드를 생성했다하자
* 정해진 시간 동안 CPU에게 보다 많은 일을 시킬 수 있고, Blocked 상태에 놓이는 경우에도 스레드들이 일을 나눠 감당하기 때문에 속도가 높아질 확률이 높다.

Case 2 : 스레드 종료 시 ExitThread 함수 호출이 유용한 경우(특정 위치에서 스레드를 종료시키는 경우)

ExitThread 함수

VOID ExitThread (DWORD dwExitCode);

1. dwExitCode : 스레드 종료코드

여기서 등록되는 스레드 종료코드는 GetExitCodeThread 함수에서 얻을 수 있다.

만약 스레드가 A함수 -> B함수 -> C함수를 실행시켰다고 하자
* C함수에서 return을 호출했을 경우
* B함수, A함수 까지 모두 return을 해야 스레드가 종료된다.
* ExitThread 함수를 이용하면 바로 스레드를 종료시킬 수 있다.\

**스택 프레임에 만약 C++ 객체가 존재하는 경우 소멸자가 호출되지 않는다.**

Case 3 : 스레드 종료 시 TerminateThread 함수 호출이 유용한 경우 (외부에서 스레드를 종료시키는 경우)
* main 함수 내에서 스레드를 생성한 경우 외부에서 스레드 핸들을 얻어 종료시킬 수 있다.

BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode);

* 실패 시 0 반환

1. hThread : 종료시킬 스레드 핸들
2. dwExitCode : 종료 코드

종료 당하는 스레드는 종료 직전까지도 자신이 종료되는지 알지 못한다.
* 종료 작업이 진행되지 않는다. (메모리 할당 해제 등)
* 이 함수 꼭 써야해?

## Section 02 - 스레드의 성격과 특성
### 힙, 데이터 영역 그리고 코드 영역의 검증
* 전역 변수를 선언하고 덧셈 연산 진행

### 동시접근에 있어서의 문제점
덧셈이 이뤄지는 과정
1. 메모리에 저장된 데이터를 레지스터로 이동
2. ALU에서 덧셈 진행
3. 결과를 메모리에 저장

세 가지 과정으로 나누어 진행되는데 여러 스레드가 동시 작업하며 타이밍이 어긋나는 경우 원치않는 결과가 나타날 수도 있다.

* 컨텍스트 스위칭은 매우 빈번하게 일어난다.

### 프로세스로부터의 스레드 분리

* 프로세스 하나당 핸들 테이블 한개
* 스레드 Usage Count 또한 프로세스와 마찬가지로 생성하면 2
* 스레드를 생성하고 CloseHandle 함수를 호출해야 한다.


### ANSI 표준 C 라이브러리와 스레드
* 멀티 스레드 기반으로 ANSI 표준 함수의 호출은 메모리 동시 참조 문제를 야기할 수 있다.
* 마이크로소프트에서는 멀티 스레드에서 안전한 ANSI 표준 함수를 제공

### 결론, 그냥 return을 이용하자

## Section 03 - 스레드의 상태 컨트롤
### 스레드의 생태 변화
* 프로세스의 상태 변화와 100% 똑같다.

### Suspend & Resume
특정 스레드를 선택해서 Ready 상태로 옮길 수 있고, Blocked 상태로도 옮길 수 있다.

DWORD SuspendThread(HANDLE hThread);
* 스레드를 Blocked 상태로 둔다.
 
DWORD ResumeThread(HANDLE hThread);
* 스레드를 Ready 상태로 둔다.

SuspendThread 함수를 호출하면 SuspendThread Count가 증가한다.
* 1번 호출 -> 1
* 2번 호출 -> 2
* ResumeThread 함수 호출 -> 1
* 한번 더 호출 0

Suspend Count가 0이 되면 스레드가 Ready 상태로 진입한다.


## Section 04 - 스레드의 우선순위 컨트롤
Windows에서는 프로세스가 우선순위를 갖는 것이 아닌 스레드가 갖는다.]

스레드의 상대적 우선순위
1. THREAD_PRIORITY_LOWEST : -2
2. THREAD_PRIORITY_BELOW_NORMAL : -1
3. THREAD_PRIORITY_NORMAL : 0 (Defualt)
4. THREAD_PRIORITY_ABOVE_NORMAL : +1
5. THREAD_PRIORITY_HIGHEST : +2

프로세스 우선순위를 기준으로 상대적 우선순위를 더하거나 빼서 우선순위를 계산한다.

BOOL SetThreadPriority(HANDLE hThread, int nPriority);

int GetThreadPriority(HANDLE hThread);


## 이것만은 알고 갑시다.
1. CreateThread 함수와 _beginthreadex 함수의 차이점
2. 
3. 둘 이상의 스레드가 동시접근하는 메모리 공간의 문제점
4. 스레드의 상태 변화
5. 프로세스로부터 스레드 분리




