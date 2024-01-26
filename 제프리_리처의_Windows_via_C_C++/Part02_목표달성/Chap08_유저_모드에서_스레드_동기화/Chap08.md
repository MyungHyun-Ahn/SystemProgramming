# Chapter 08 유저 모드에서의 스레드 동기화

다음 두 가지의 기본적인 상황에서 스레드는 상호 통신을 수행해야 한다.
* 다수의 스레드가 공유 리소스에 접근해야 하며, 리소스가 손상되지 않도록 해야 하는 경우
* 어떤 스레드가 하나 혹은 다수의 다른 스레드에게 작업이 완료되었음을 알려야 하는 경우

윈도우는 스레드 동기화를 간편하게 수행할 수 있는 다양한 방법들을 제공하고 있다.
* 여러 개의 스레드가 동시에 수행될 때 이들이 어떤 작업을 하는지 파악하기 어렵다.
* 멀티스레드는 순차적으로 접근하지 않는다.

## Section 01 원자적 접근: Interlocked 함수들
만약 전역 변수에 여러 스레드가 접근하여 1씩 더하는 경우
* 원하는 결과가 나오지 않을 수도 있다.

g_x에 1을 더하는 어셈블리 코드
~~~
MOV EAX, [g_x]  ; g_x 값을 레지스터에 옮긴다.
INC EAX         ; 레지스터의 값을 증가시킨다.
MOV [g_x], EAX  ; 레지스터에서 g_x로 값을 저장한다.
~~~
* 두 개의 스레드는 이 코드를 정확히 동일한 시간에 수행하지 않을 수 있다.
* 코드가 뒤섞여 실행되면 기대한 결과값이 2지만 1이 나올 수 있다.

이 문제를 해결하기 위해서는 수행 중에 인터럽트되지 않고 값을 원자적으로 증가시킬 수 있는 방법이 필요하다.
* 인터락 계열의 함수

값을 증가시키는 인터락 함수
~~~c++
// 4바이트 정수 증가
LONG InterlockedExchangeAdd(
    PLONG volatile plAddend,
    LONG lIncrement
);

// 8바이트 정수 증가
LONGLONG InterlockedExchangeAdd64(
    PLONGLONG volatile plAddend,
    LONGLONG lIncrement
);
~~~
* 위 함수는 값을 증가시키는 동작이 원자적으로 동작될 것임을 보장한다.

단순히 1만 증가시키려면 InterlockedIncrement 함수를 사용해도 된다.

인터락 함수들은 어떻게 동작할까?
* x86 계열의 CPU라면 인터락 함수들은 버스에 하드웨어 시그널을 실어서 다른 CPU가 동일 메모리 주소에 접근하지 못하도록 한다.

인터락 함수를 이용하면 CPU의 개수와는 무관하게 변수의 값을 원자적으로 변경할 수 있다.


인터락 함수를 알아야하는 이유
* 매우 빠르다 : 단 몇 CPU 사이클만에 끝남
* 유저 모드와 커널 모드의 전환도 일어나지 않음

InterlockedExchangeAdd의 2번째 인자로 음수값을 전달하여 뺄셈도 가능하다.

추가적인 인터락 함수
~~~C++
// 첫 번째 매개변수로 전달되는 주소가 담고 있는 값을 두 번째 매개변수로 전달되는 값으로 변경한다.
// 스핀락을 구현하는 경우 매우 유용하다.
LONG InterlockedExchange(
    PLONG volatile plTarget,
    LONG lValue
);

LONGLONG InterlockedExchange64(
    PLONGLONG volatile plTarget,
    LONGLONG lValue
);

PVOID InterlockedExchangePointer(
    PVOID* volatile ppvTarget,
    PVOID pvValue
);
~~~

SpinLock
~~~C++
// 공유 리소스의 사용 여부를 나타내는 전역변수
BOOL g_fResourceFalse;

void Func1()
{
    // 리소스의 접근을 기다림
    // 이전값이 FALSE가 나올 때까지 루프를 돈다.
    // 이전값이 TRUE라면 다른 스레드가 사용중인 것
    while (InterlockeddExchange(&g_fResourceInUse, TRUE) == TRUE)
        Sleep(0);

    // 리소스에 접근함
    // ...

    // 리소스에 더 이상 접근할 필요가 없음
    InterlockedExchange(&g_fResourceInUse, FALSE);
}
~~~
* 스핀락과 같은 기법은 CPU 시간을 많이 낭비할 수 있기 때문에 세심한 주의가 필요하다.
* CPU는 일관된 방법으로 두개의 값을 비교하고, 다른 스레드에 의해 전역변수의 값이 변경된 경우에만 비교를 중지해야 한다.
* 스핀락을 수행하는 모든 스레드는 동일한 우선순위 레벨에 있는 것으로 가정
* 스레드 우선 순위 동적 상승 기능이 불가능하도록 설정

또한 락 변수와 락을 통해 보호받고자 하는 데이터는 서로 다른 캐시 라인에 있도록 하는 것이 좋다.
* 락 변수와 데이터가 동일한 캐시 라인에 있게 되면, 리소스를 사용하는 CPU는 동일 리소스에 접근하고자 하는 다른 CPU와 경쟁하게 될 것이다. - 성능에 나쁘다.

스핀락은 보호된 리소스가 매우 짧은 시간 동안만 사용될 것이라 가정한다.
* 수 회 스핀을 수행하고 그 후에도 리소스에 접근이 불가하면 커널 모드로 스레드를 대기시키는 것이 효과적이다.
* 이것이 크리티컬 섹션의 구현 방식

또 다른 부류의 인터락 함수
~~~C++
// 32비트 32 : 32
// 64비트 32 : 64
PVOID InterlockedCompareExchange(
    PLONG plDestination,
    LONG lExchange,
    LONG lComparand
);

PVOID InterlockedCompareExchangePointer(
    PVOID* ppvDestination,
    PVOID pvExchange,
    PVOID pvComparand
);
~~~

* plDestination 매개변수의 값을 lComparand 매개변수로 전달할 값과 비교해서
* 그 값이 동일하면 *plDestination 값을 lExchange 값으로 변경한다.
* 이 함수는 *plDestination의 이전 값을 반환한다.

추가적인 인터락 함수
~~~C++
LONG InterlockedIncrement(PLONG plAddend);
LONG InterlockedDecrement(PLONG plAddend);
~~~


인터락 싱글 링크드 리스트 : 스택에 값을 원자적으로 푸시 팝 할 수 있다.
* InitializeSListHead : 빈 스택 생성
* InterlockedPushEntrySList : 스택 상단에 값을 추가한다.
* InterlockedPopEntrySList : 스택 상단으로부터 값을 제거하고 그 값을 반환
* InterlockedFlushSList : 스택을 비운다.
* QueryDepthSList : 저장된 값의 개수를 가져온다.


## Section 02 캐시 라인
CPU가 메모리로부터 값을 가져올 때는 바이트 단위로 값을 가져오는게 아닌 캐시 라인을 가득 채울 만큼의 양을 가져온다.
* 32, 64, 128 바이트 크기로 구성(CPU에 따라 다르다.)
* 각기 32바이트 64바이트 128바이트 경계로 정렬되어 있다.
* 성능 향상을 위해 존재한다.
* 이미 인접한 바이트들이 캐시라인에 존재해 있다면, 메모리 버스에 CPU가 접근할 필요가 없게된다.

캐시 라인은 멀티 프로세서 환경에서 메모리 갱신을 매우 어렵게 만든다.
1. CPU1 이 메모리 상의 특정 위치에서 1바이트를 읽는다. 이때 읽고자 하는 바이트와 인접한 바이트들
도 같이 CPU1 의 캐시 라인으로 들어온다.
2. CPU2가 동일한 위치로부터 1 바이트를 읽는다. CPU1 의 캐시 라인에 존재하는 바이트와 동일한 내용
이 CPU2의 캐시 라인에도 들어온다.
3. CPU1 이 메모리의 내용을 변경한다. 이러한 변경 내용은 CPU1 캐시 라인 내의 내용을 변경하게 될
것이다. 하지만 실제 램에는 아직 변경된 내용이 쓰여지지 않았다.
4. CPU2가 동일한 위치로부터 1바이트를 다시 읽어오려고 시도한다. CPU2의 캐시 라인에 이미 읽고자
하는 바이트가 들어 있으므로 메모리에 추가적으로 접근할 필요가 없다. 하지만 CPU2는 CPU1 이 변
경한 내용을 알 수 없다.

CPU가 캐시 라인에 있는 정보를 변경하면 다른 CPU가 쓰려할 때 이러한 사실을 알아내고 정보를 무효화 시킨다.
* 같은 캐시 라인에 있는 경우
* 32비트 컴파일러에서는 long long이 캐시라인 경계에 서지 않을수도 있다.
* 만약 캐시 라인의 경계에 같은 변수가 서있는 경우에는 무효화되지 않는다.

이러한 특성 때문에 프로세스가 사용하는 데이터는 캐시 라인의 크기와 그 경계 단위로 묶어서 다루는 것이 좋다.
* 이렇게 함으로 적어도 하나 이상의 캐시라인 경계로 분리된 서로 다른 메모리 블록에 각각의 CPU가 독립적으로 접근하는 것을 보장할 수 있다.
* 읽기 전용의 데이터와
* 읽고 쓰는 데이터를 분리하는 것이 좋다.

~~~c++
#define CACHE_ALIGN 64

// 구조체의 인스턴스가 각기 다른 캐시 라인에 들어갈 수 있도록 한다.
struct __declspec(align(CACHE_ALIGN)) CUSTINFO
{
    DWORD dwCustomerID;         // 거의 읽기 전용으로 사용
    wchar_t szName[100];        // 거의 읽기 전용으로 사용

    // 아래 필드는 다른 캐시 라인에 들어갈 수 있도록 한다.
    int nBalanceDue;            // 읽고 쓰기용으로 사용
    FILETIME ftLastOrderData;   // 읽고 쓰기용으로 사용
}
~~~

### Section 03 고급 스레드 동기화 기법
인터락 계열의 함수들은 하나의 값에 원자적으로 접근해야 하는 경우 훌륭하게 동작한다.
* 대부분의 프로그램은 32비트 값이나 64비트 값보다는 훨씬 더 복잡한 자료구조를 다루는 것이 보통이다.
* 이러한 복잡한 자료 구조에 대해 원자적 접근을 수행해야 한다면, 인터락 함수는 고려 대상이 될 수 없으며, 윈도우가 제공하는 다른 기능을 이용해야 한다.

스핀락의 단점
* CPU 시간의 낭비가 크다.
* 따라서 CPU 시간을 낭비하지 않을 수 있는 추가적인 메커니즘이 필요

스레드가 공유 리소스를 기다리거나 특별한 이벤트의 통지를 대기하고자 하는 경우
* 대기하고자 하는 리소스나 이벤트를 나타내는 값을 매개변수로 운영체제가 제공하는 대기 함수를 호출하면 된다.
* 스레드가 대기하는 리소스가 가용 상태, 혹은 이벤트가 발생하면 대기 함수는 반환되고 스레드는 스케줄 가능 상태가 된다.

리소스가 가용하지 않거나 이벤트가 발생하지 않았다면
* 시스템은 스레드를 대기 상태로 두어 스케줄이 불가능하게 한다. - CPU 시간 낭비를 방지
* 스레드가 대기 상태인 동안에는 시스템이 스레드의 대리자 역할을 한다.
* 시스템은 스레드가 대기하는 리소스나 이벤트를 기억했다가 자원이 사용가능해지면 스레드를 대기 상태에서 빠져나오게 한다.

### 회피 기술
동기화 객체나 특별한 이벤트를 대기하는 기능을 운영체제가 제공하지 않는다 하더라도 자체적으로 동기화를 수행할 수 있다.
* 하지만 운영체제가 동기화 기법을 제공하면 절대로 사용해서는 안된다.

다수의 스레드에 의해 공유되고 있는 변수의 상태를 지속적으로 폴링(polling)하여 다른 스레드가 작업을 완료했는지의 여부를 확인하는 동기화 기법
~~~C++
volatile BOOL g_fFinishedCalculation = FALSE;

int WINAPI _tWinMain(...)
{
    CreateThread(..., RecalcFunc, ...);
    ...
    // 재연산이 완료될 때까지 대기 - 스핀락
    while(!g_fFinishedCalculation);

    ...
}

DWORD WINAPI RecalcFunc(PVOID pvParam)
{
    // 재연산을 수행한다.
    ...     g_fFinishedCalculation = TRUE;
    return 0;
}
~~~
* 주 스레드는 RecalcFunc 함수가 완료되기를 기다리기 위해 대기 상태로 전환되지 않는다. - 스핀락
* 이 코드의 문제점은 주 스레드가 RecalcFunc을 수행하는 스레드보다 우선순위가 높다면 절대 값이 TRUE가 되지 않을 수도 있다.
* 만약 주 스레드가 Sleep을 호출하면 RecalcFunc 함수를 수행하는 스레드가 스케줄링될 것이다.

위 방법은 매우 간편하지만 더 좋은 방법이 있다.
* 필요로 하는 자원이 가용 상태가 될 때까지 스레드를 대기시키는 것이다.

volatile 타입 한정자
* 컴파일러에게 이 변수가 운영체제나 하드웨어 혹은 동시에 수행 중인 다른 스레드와 같이 외부에서 그 내용이 변경될 수 있음을 알려주는 역할
* 컴파일러는 이 변수의 어떠한 최적화도 수행하지 않고, 변수의 값이 참조될 때 항상 메모리로부터 값을 다시 가져오도록 코드를 생성한다.

인터락 함수들은 변수 값 자체를 인자로 취하지 않고 변수 값이 저장되어 있는 주소를 인자로 받아들이기 때문에 함수는 항시 메모리로부터 값을 얻어오게 되며 최적화 기능은 영향을 주지 않는다.
* 인터락 함수에게는 volatile로 선언하여 전달하지 않는다.

## Section 04 크리티컬 섹션
공유 리소스에 대해 배타적으로 접근해야 하는 작은 코드의 집합을 의미
* 여러 줄의 코드를 원자적으로 수행하기 위한 방법

크리티컬 섹션을 사용하지 않으면 어떠한 문제가 발생하는지 보여주는 예제
~~~c++
const int COUNT = 1000;
int g_nSum = 0;

DWORD WINAPI FirstThread(PVOID pvParam)
{
    g_nSum = 0;
    for (int n = 1; n <= COUNT; n++)
    {
        g_nSum += n;
    }

    return g_nNum;
}


DWORD WINAPI SecondThread(PVOID pvParam)
{
    g_nSum = 0;
    for (int n = 1; n <= COUNT; n++)
    {
        g_nSum += n;
    }

    return g_nNum;
}
~~~
* 각 스레드가 독립적으로 수행된다면 두 개의 스레드 함수는 동일한 값을 반환
* 하지만 동시에 수행하면 공유변수에 동시에 접근하게 되어 기대한 결과를 얻을 수 없다.

Cretical Section을 이용한 동기화
~~~c++
const int COUNT = 1000;
int g_nSum = 0;
CRITICAL_SECTION g_cs;

DWORD WINAPI FirstThread(PVOID pvParam)
{
    EnterCriticalSection(&g_cs);
    g_nSum = 0;
    for (int n = 1; n <= COUNT; n++)
    {
        g_nSum += n;
    }

    LeaveCriticalSection(&g_cs);
    return g_nSum;
}


DWORD WINAPI SecondThread(PVOID pvParam)
{
    EnterCriticalSection(&g_cs);
    g_nSum = 0;
    for (int n = 1; n <= COUNT; n++)
    {
        g_nSum += n;
    }

    LeaveCriticalSection(&g_cs);
    return g_nSum;
}
~~~
* 다수의 스레드가 동시에 접근하는 공유리소스가 있다면 CRITICAL_SECTION 구조체를 먼저 생성해야 한다.
* 공유 리소스를 접근하기 전에는 EnterCriticalSection 함수를 호출해야 한다. - 리소스를 사용중이라고 표시
* 리소스를 사용한 후에는 LeaveCriticalSection 함수를 호출하여 - 리소스를 사용해도 된다고 표시

크리티컬 섹션은 내부적으로 인터락 함수를 사용하기 때문에 매우 빠르게 동작한다.


### 크리티컬 섹션 세부사항
CRITICAL_SECTION 구조체
* 전역, 힙, 지역, 클래스 멤버로 사용할 수 있다.
* 공유 리소스에 접근하는 모든 스레드는 반드시 해당 리소스를 보호하는 CRITICAL_SECTION 구조체의 주소를 알고 있어야 한다.
* 구조체를 사용하기 전에 반드시 초기화해야 한다.

CRITICAL_SECTION 구조체 초기화
* VOID InitializeCriticalSection(PCRITICAL_SECTION pcs);
* 단순히 멤버의 값을 세팅하는 역할, 절대 실패하지 않는다. 따라서 반환값은 VOID
* EnterCriticalSection 함수를 호출하기 전에 호출되어야 한다.

CRITICAL_SECTION 구조체 삭제
* VOID DeleteCriticalSection(PCRITICAL_SECTION pcs);
* 더 이상 크리티컬 섹션을 사용하지 않는다면 삭제해야 한다.

CRITICAL_SECTION 진입
* VOID EnterCriticalSection(PCRITICAL_SECTION pcs);

EnterCriticalSection 함수가 내부적으로 진행하는 일
* 만일 공유 리소스를 사용하는 스레드가 없다면 CRITICAL_SECTION 구조체 내의 멤버 변수를 갱신하여 호출 스레드의 접근 권한을 획득했음을 표시하고 스레드가 수행할 수 있도록 반환
* 이미 권한을 획득한 상태라면 이 함수를 몇번 호출했는지 기록 - Enter 연속해서 호출한 경우
* 이미 다른 스레드가 공유 리소스의 접근 권한을 획득했다면, 호출 스레드를 이벤트 커널 오브젝트를 사용하여 대기 상태로 만든다.
* 대기 상태인 스레드는 CPU 시간을 낭비하지 않는다.

EnterCriticalSection은 사실 복잡한 작업을 수행하지는 않고 단순한 테스트 작업을 수행한다.
* 이러한 테스트들은 모두 원자적으로 수행된다.
* 멀티 프로세서 머신에서 두 개의 스레드가 완전히 동일한 시점에 함수를 호출해도 단 하나의 스레드만 공유 리소스에 접근 권한을 얻는다.


EnterCriticalSection 대신 쓸 수 있는 함수
* BOOL TryEnterCriticalSection(PCRITICAL_SECTION pcs);
* 이 함수를 사용하면 호출한 스레드를 대기 상태로 전환하지 않는다.
* 대신 함수의 반환 값으로 접근 권한을 얻었는지 여부를 가져오게 된다.
* TRUE를 반환하는 경우 꼭 LeaveCriticalSection 함수를 호출해야 한다.

공유 리소스에 대한 사용을 마치면 LeaveCriticalSection 함수를 사용한다.
* 공유 리소스에 접근 횟수를 1 감소
* 0 보다 크면 LeaveCriticalSection은 아무런 작업도 하지 않고 반환
* 0이되면 접근 권한을 획득한 스레드가 없는 것으로 갱신
* 만일 대기 상태로 진입한 스레드가 1개 이상일 경우 하나를 선택하여 스케줄 가능 상태로 변경


### 크리티컬 섹션과 스핀락
다른 스레드가 이미 진입한 크리티컬 섹션에 특정 스레드가 진입을 시도하면 스레드는 바로 대기 상태로 변경
* 유저 모드 -> 커널 모드로의 전환 : 매우 값비싼 동작
* 만약 진입을 시도한 스레드를 커널모드로 완전히 전환하기도 전에 공유 리소스의 권한이 반환되면
* 상당한 CPU 시간이 낭비된다.

크리티컬 섹션의 성능을 개선하기 위해 마이크로소프트는 스핀락 메커니즘을 도입하였다.
* 스핀락을 수행하는 동안 공유 리소스 자원을 획득하지 못할 때만 대기 상태에 들어간다.

크리티컬 섹션에 스핀락을 사용하려면 초기화를 다음 함수로 진행해야 한다.
* BOOL InitializeCriticalSectionAndSpinCount(PCRITICAL_SECTION pcs, DWORD dwSpinCount);
* 두번 째 인자에 스핀락 루프의 횟수를 기입한다. 0 ~ 0x00FFFFFF

크리티컬 섹션의 스핀 횟수는 다음 함수를 호출하여 변경할 수 있다.
* DWORD SetCriticalSectionSpinCount(PCRITICAL_SECTION pcs, DWORD dwSpinCount);

프로세스 힙에 대한 접근을 보호할 때 사용하는 스핀 카운트는 4000


### 크리티컬 섹션과 에러 처리
드문 경우 InitializeCriticalSection 함수도 실패할 수 있다.
* 내부적으로 메모리를 할당하기 때문에 메모리 할당에 실패하면
* STATUS_NO_MEMORY 예외가 발생하는데
* 구조적 예외 처리를 사용하면 에러를 확인 가능하다.

InitializeCriticalSectionAndSpinCount 함수의 경우 이 문제를 더 쉽게 다룰 수 있다.
* 메모리 블록 할당에 실패한 경우 FALSE 반환


내부적으로 크리티컬 섹션은 둘 이상의 스레드가 동일 시간에 진입하려 하면 이벤트 커널 오브젝트를 사용하게 된다.
* 이벤트 커널 오브젝트 생성에 실패하는 경우가 생길 수 있다. - 아주 드물다.
* DeleteCriticalSection 호출 전까지 이벤트 커널오브젝트는 삭제되지 않는다.

윈도우 XP 부터는 가용 리소스가 매우 적은 상태에서 발생할 수 있는 문제를 해결하기 위해
* 키 이벤트라는 새로운 커널 오브젝트 추가 - 프로세스 당 하나
* 이벤트 커널 오브젝트 처럼 동작하지만, 일련의 스레드들 사이에서 유일하게 1개의 인스턴스만 사용 가능하다.
* 이벤트 커널 오브젝트 생성이 불가한 경우 크리티컬 섹션의 주소가 키 오브젝트의 키로 사용된다.
* 스레드가 진입하려 하면 키 오브젝트를 사용하여 블로킹 수행

## Section 05 슬림 리터-라이터 락
SRWLock(Slim Reader-Writer Lock)은 단순 크리티컬 섹션과 유사하게 다수의 스레드로부터 단일 리소스를 보호할 목적으로 사용된다.

크리티컬 섹션과 차이점은 읽기만 하는 스레드(리더) 수정하려는 스레드(라이터) 들이 완전히 구분되어 있을 때만 사용할 수 있다는 것이다.
* 읽기만 하는 스레드는 접근 하더라도 공유 리소스의 값을 수정하지 않기 때문에 동시에 수행되어도 무방하다.
* 동기화는 라이터 스레드가 수정하려고 하는 동안에만 어떠한 리더, 라이터 스레드도 공유 리소스에 접근해서는 안된다.

SRWLOCK 구조체 할당
* VOID InitializeSRWLock(PSRWLOCK SRWLock);

일단 SRWLock이 초기화되면 라이터 스레드는 SRWLock을 이용하여 보호하려는 공유 리소스의 권한을 획득하기 위해 다음 함수를 호출해야 한다.
* VOID AcquireSRWLockExclusive(PSRWLOCK SRWLock);

공유 리소스를 모두 사용하고 나면 락을 해제하기 위해 다음 함수를 호출한다.
* VOID ReleaseSRWLockExclusive(PSRWLOCK SRWLock);

리더 스레드의 경우 다음 함수를 호출한다.
* VOID AcquireSRWLockShared(PSRWLOCK SRWLock); // 진입
* VOID ReleaseSRWLockShared(PSRWLOCK SRWLock); // 해제

SRWLOCK 오브젝트를 파괴하는 함수는 없으며, 이는 시스템이 자동으로 수행한다.

크리티컬 섹션과 비교하면 SRWLock은 다음 기능을 지원하지 않는다.
* TryEnter SRWLock 함수가 없다. - Acquire 함수는 다른 스레드가 락을 설정하고 있는 경우 블로킹
* SRWLOCK 오브젝트를 반복적으로 획득할 수 없다.

크리티컬 섹션 대신 SRWLock을 사용하면 성능과 확장성이 증대된다.

각 동기화 메커니즘의 비교
* volatile 변수 읽기
  * 동기화가 필요없다. 
  * CPU 캐시에 값이 독립적으로 유지되기 때문에 가장 빠르다.
  * CPU나 스레드의 개수와 무관하다.
* volatile 변수 쓰기
  * CPU 사이에서 일관성있게 캐시를 유지하기 위해 상호통신을 수행한다.
  * 읽기에 비해 성능이 떨어진다.
* InterlockedIncrement
  * CPU가 배타적으로 메모리에 접근할 수 있도록 락을 설정하기 때문에 일반적인 읽기/쓰기에 비해 느리다.
  * 일관되게 캐시를 유지하기 위해 CPU 간 상호통신 또한 수행하기 때문에 CPU 개수가 늘 수록 더 느려진다.
* Critical Section
  * 앞 뒤로 Enter, Leave 과정을 추가 수행해야 하기 때문에 상대적으로 느리다.
  * 경쟁이 발생하면 성능이 더욱더 나빠진다.
  * 2개의 스레드보다 4개일 때 더 나빠졌다. - 스레드 간 경쟁 상태가 유발되어 컨텍스트 스위칭 횟수가 증가
* SRWLock 
  * 하나의 스레드를 사용할 경우 읽기와 쓰기 속도가 거의 동일하다.
  * 2개의 스레드인 경우 일기 속도가 쓰기 속도보다 조금 더 좋은 성능을 보인다.
* 뮤텍스 커널 오브젝트 동기화
  * 뮤텍스를 사용하면 테스트를 반복할 때마다 뮤텍스를 소유하고 해제해야 한다.
  * 이 과정에서 유저 모드와 커널 모드 사이의 전환이 계속해서 발생한다. - 가장 나쁜 성능
  * 유저 모드와 커널 모드의 전환은 매우 비싼 작업에 해당하고
  * 경쟁을 유발할 수도 있기 때문에 성능 결과는 가장 나쁘게 나타난다.

정리하면 최상의 성능으로 동작하는 프로그램을 작성하고 싶다면
* 공유 리소스를 사용하지 않도록 작성
* 불가능 하다면 인터락 API, SRWLock, 크리티컬 섹션 순으로 사용을 검토해야 한다.

## Section 06 조건변수
조건변수(condition variable)를 사용하면 스레드가 리소스에 대한 락을 해제하고
* SleepConditionVariableCS 혹은 SleepConditionVariableSRW 함수에서 지정한 상태가 될때까지 스레드를 블로킹한다.
* 이러한 동작이 원자적으로 수행되도록 설계되어서 개발 업무를 좀 더 간편하게 해준다.

~~~c++
BOOL SleepConditionVariableCS(
    PCONDITION_VARIABLE pConditionVariable,
    PCRITICAL_SECTION pCriticalSection,
    DWORD dwMilliseconds
);

BOOL SleepConditionVariableSRW(
    PCONDITION_VARIABLE pConditionVariable,
    PSRWLOCK pSRWLock,
    DWORD dwMilliseconds,
    ULONG Flags
);
~~~
* dwMilliseconds : 스레드가 조건변수가 시그널 상태가 될 때까지 얼만큼 기다릴지 결정하는 값, 0이라면 라이터 스레드를 위한 배타적인 락을 설정
* Flags : CONDITION_VARIABLE_LOCKMODE_SHARED를 전달하면 리더 스레드를 위한 공유 가능 락을 설정

두 개의 함수는 조건 변수가 시그널 상태가 되기 전에 타임아웃이 되면 FALSE, 그렇지 않으면 TRUE를 반환
* FALSE가 반환되는 경우 락을 수행하지 않으며, 크리티컬 섹션 또한 획득하지도 못한다.

특정 스레드가 SleepConditionVariable 함수를 호출하여 블로킹되어 있는 상태
* 리더 스레드가 읽어올 자료가 생겼다거나 라이터 스레드가 자료를 저장할 공간이 생긴 경우
* WakeConditionVariable이나 WakeAllConditionVariable 함수를 호출하면 된다.

~~~c++
VOID WakeConditionVariable(PCONDITION_VARIABLE ConditionVariable);
VOID WakeAllConditionVariable(PCONDITION_VARIABLE ConditionVariable);
~~~
* 동일한 조건변수를 인자로 SleepConditionVariable 함수를 호출한 스레드가 여러 개 있는 경우
* 이 중 하나만 락을 설정한 상태로 수행을 재개하게 된다.
* 만일 수행을 재개한 스레드가 락을 삭제해버리면 다른 스레드들은 깨어나지 못하게 된다.
* WakeAllConditionVariable을 호출하면 동일한 조건변수를 인자로 SleepConditionVariable을 호출한 모든 스레드가 수행을 재개한다.
* 단일의 라이터 스레드가 Flags 값을 0으로 배타적인 락을 요구하고
* 다수의 리더 스레드가 Flags 값으로 CONDITION_VARIABLE_LOCKMODE_SHARED를 전달하여 공유 가능 락을 요구하였다면 다수의 스레드가 동시에 깨어나는 상황은 아무런 문제가 되지 않는다.

### 유용한 팁과 테크닉
원자적으로 관리되어야 하는 오브젝트 집합당 하나의 락만을 사용해라.
* 여러 개의 오브젝트들이 항상 같이 사용되어 "논리적으로" 단일의 리소스처럼 다루어야 하는 경우
* 하나의 락만을 유지해야 한다. (배열, 배열 개수와 같은 것)

다수의 논리적 리소스들에 동시에 접근하는 방법
~~~C++
{ // 정상적인 방법
    EnterCriticalSection(&g_csResource1);
    EnterCriticalSection(&g_csResource2);

    // 리소스 1로부터 어떤 값을 가져와서
    // 리소스 2에 넣는다.

    LeaveCriticalSection(&g_csResource2);
    LeaveCriticalSection(&g_csResource1);
}

{ // 순서가 꼬인 경우 - 순서는 일관되게 유지해야 한다.
    EnterCriticalSection(&g_csResource2);
    EnterCriticalSection(&g_csResource1);

    // 리소스 1로부터 어떤 값을 가져와서
    // 리소스 2에 넣는다.

    LeaveCriticalSection(&g_csResource2);
    LeaveCriticalSection(&g_csResource1); // 데드락 상태가 발생할 수 있다.
}
~~~
* 스레드 1 : 1 획득
* 스레드 2 : 2 획득
* 서로 맞물려버리는 상태가 발생해서 - 데드락이 발생할 수 있다.

락을 장시간 점유하지 마라
* 락을 너무 오랜 시간 점유하고 있게 되면 다른 스레드들이 계속 대기 상태에 머물러있어 프로그램의 성능에 나쁜 영향을 미칠 수 있다. 