# 13장 스레드 동기화 기법 1
## Section 01 - 스레드 동기화란 무엇인가?
### 두 가지 관점에서의 스레드 동기화
스레드의 관점에서 동기화는 일치한다는 의미가 아니다.
* 순서에 있어서 질서가 지켜진다를 의미

실행 순서의 동기화
* 스레드의 실행순서를 정의하고, 순서에 반드시 따르도록 하는 것이 스레드 동기화이다.
* A -> B -> C

메모리 접근에 대한 동기화
* 메모리 접근에 있어서 동시접근을 막는 것 또한 스레드 동기화에 해당한다.
* 실행의 관점은 떠나 메모리의 동시접근 문제만 발생하지 않으면 된다.

### 스레드 동기화에 있어서의 두 가지 방법
유저 모드 동기화
* 커널의 힘을 빌리지 않는 동기화

커널 모드 동기화
* 커널의 힘을 빌리는 동기화
* 커널 모드로의 전환이 발생하므로 성능의 저하로 이어진다.
* 유저 모드에서는 제공받지 못하는 기능을 제공받을 수 있다.

## Section 02 - 임계 영역(Critical Section) 접근 동기화
* 이번 주제는 메모리 접근의 동기화에 대해서 공부한다.
* 메모리 영역의 동기화는 즉, 임계 영역의 접근을 동기화 하겠다는 뜻이다.

### 임계 영역(Critical Section)의 이해
* 임계 영역이란 배타적 접근(한 순간에 하나의 스레드만 접근)이 요구되는 공유 리소스에 접근하는 코드 블록을 의미

Windows의 대표적인 동기화 기법

유저 모드 동기화
1. 크리티컬 섹션(Critical Section) 기반의 동기화
   * 메모리 접근 동기화
2. 인터락 함수(Interlocked Family Of Function) 기반의 동기화
   * 메모리 접근 동기화

커널 모드 동기화
1. 뮤텍스(Mutex) 기반의 동기화
   * 메모리 접근 동기화
2. 세마포어(Semaphore) 기반의 동기화
   * 메모리 접근 동기화
3. 이름있는 뮤텍스(Named Mutex) 기반의 동기화
   * 프로세스 간 동기화
4. 이벤트(Event) 기반의 동기화
   * 실행순서 동기화

## Section 03 - 유저 모드의 동기화 (Synchronization In User Mode)
* 커널 모드로의 전환이 불필요하기 때문에 성능상의 이점
* 단순하다.
* 이 기법으로 문제해결이 충분한 상황이라면 굳이 커널 모드의 동기화 기법을 사용할 필요는 없다.

### 크리티컬 섹션(Critical Section) 기반의 동기화
화장실 열쇠에 비유하면 이해하기 쉽다.
1. 화장실 열쇠를 가져온다.
2. 화장실에 들어간다. -> 다른 사람은 들어가지 못한다.
3. 화장실에서 나와 열쇠를 돌려놓는다. -> 다른 사람이 들어올 수 있다. 
   
Windows의 Critical Section 동기화 함수
1. CRITICAL_SECTION 오브젝트 생성
2. InitializeCriticalSection : 크리티컬 섹션 오브젝트의 초기화
   * 반드시 이 과정을 거친다.
3. EnterCriticalSection : 임계 영역에 진입
4. LeaveCriricalSection : 임계 영역에서 빠져 나옴
5. DeleteCriticalSection : 크리티컬 섹션 오브젝트를 반환

EnterCriticalSection 함수는 컨텍스트 스위칭을 막으므로 \
동시 호출하는 경우에도 동기화가 보장된다. - Interupt Disable

### 인터락(Interlocked Family Of Function) 기반의 동기화
* 변수 하나의 접근방식을 동기화하는 것이 목적이라면
* 이러한 용도로 특화된 인터락 함수를 사용하는 것도 나쁘지 않다.
* 내부적으로 한 순간에 하나의 스레드에 의해서만 실행되도록 보장

LONG InterlockedIncrement(LONG volatile *Addend);
* 증가된 결과 값 리턴

LONG InterlockedDecrement(LONG volatile *Addend);
* 감소한 결과 값 리턴

1. Addend : 조작할 변수의 주소 값 전달

위 함수 외에도 다양한 인터락 함수가 존재 - msdn 참고


### volatile : 컴파일러에 의한 최적화 억제


## Section 04 - 커널 모드 동기화 (Synchronization In Kernel Mode)
* 유저 모드보다 느리다.
* Windows 커널 레벨에서 제공한다.
* 유저 모드에서 제공하지 않는 기능을 사용할 수 있다.

### 뮤텍스(Mutex) 기반의 동기화
* Mutex Object는 열쇠에 비유

뮤텍스 오브젝트 생성 함수

HANDLE CreateMutex (\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpMutexAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInitialOwner,
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpName\
);

* 함수가 실패하면 NULL 반환

1. lpMutexAttributes : 보안 속성
2. bInitialOwner : 뮤텍스를 생성하는 스레드에게 기회를 먼저 줄지 말지 여부
3. lpName : 일단은 NULL , 이름있는 뮤텍스의 경우 이름 전달


뮤텍스는 누군가 획득했을 경우 Non-Signaled 상태이고, 획득 가능할 경우 Signaled 상태이다.


WaitForSingleObject 함수를 통해 뮤텍스의 획득 가능 여부를 알 수 있다.
* WaitForSingleObject 함수는 Signaled 상태의 핸들을 Non-Signaled 상태로 돌린다.

뮤텍스를 Signaled 상태로 돌려놓는 함수

BOOL ReleaseMutex(HANDLE hMutex);
* 함수가 실패하면 리턴 0

뮤텍스 오브젝트의 반환은 CloseHandle을 사용하면 된다.

* WaitForSingleObject 함수는 다양한 용도로 사용되기 때문에 래핑하여 사용하는 것을 추천


### 세마포어(Semaphore) 기반의 동기화
* 뮤텍스는 세마포어의 일종이다.
* 세마포어 중 바이너리 세마포어가 바로 뮤텍스
* 세마포어에는 카운트 기능이 추가되었다.

세마포어를 생성하는 함수

HANDLE CreateSemaphore(\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpMutexAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;LONG lInitialCount,
&nbsp;&nbsp;&nbsp;&nbsp;LONG lMaximumCount,
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpName\
);

* 함수 실패 시 NULL 반환

1. lpMutexAttributes : 보안 속성
2. lInitialCount : 임계 영역에 접근 가능한 스레드의 수
3. lMaximumCount : 세마포어가 가지는 값의 최대 값
4. lpName : 이름을 붙이는 용도


세마포어의 값
* lInitialCount 값에 의해 초기 카운트가 결정된다.
* 카운트가 0인 경우 Non-Signaled 상태
* WaitForSingleObject 함수를 호출할 때마다 카운트가 1씩 감소
* 만약 초기 값이 10인 경우 WaitForSIngleObject 함수를 10번 호출 가능하고, 11번 째에 블로킹 상태가 된다.


세마포어의 최대 카운트가 10이라면 생성되는 열쇠의 수는 10개라고 이해하면 쉽다.

임계영역을 빠져나온 스레드 호출 함수

BOLL ReleaseSemaphore(\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hSemaphore,\
&nbsp;&nbsp;&nbsp;&nbsp;LONG lReleaseCount,\
&nbsp;&nbsp;&nbsp;&nbsp;LPLONG lpPreviousCount\
)

* 함수 실패 시 NULL 반환

1. hSemaphore : 세마포어 핸들
2. lReleaseCount : 세마포어의 증가시킬 값, 최대가 넘어가면 카운트는 변경되지 않고 NULL 반환
3. 변경되기 전의 값을 저장할 변수, 필요없다면 NULL

### 이름있는 뮤텍스 기반의 프로세스 동기화
* 뮤텍스에 이름을 붙여 프로세스간 동기화

### 뮤텍스 소유와 WAIT_ABANDONED
* 뮤텍스는 소유한 사람이 직접 반환하는 것이 원칙이다.
* 그러나 세마포어와 그 이외의 동기화 오브젝트는 다른 스레드가 반환해도 문제되지 않는다.

예기치 않은 스레드 종료로 Mutex 객체가 반환되지 않았을 경우
* WAIT_ABANDONED : 반환을 통해 다른 스레드가 반환할 수 있도록 해준다.


## 이것만은 알고 갑시다.
1. 유저 모드 동기화와 커널 모드 동기화의 차이점 그리고 장단점

2. 임계 영역의 의미
3. 뮤텍스와 세마포어의 차이점 및 유사점
4. volatile 키워드