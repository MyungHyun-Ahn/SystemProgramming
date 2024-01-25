# Chapter 07 스레드 스케줄링, 우선순위 그리고 선호도
이번 주제는 마이크로소프트 윈도우가 사용하는 스케줄링 알고리즘에 대해 알아본다.

스레드의 컨텍스트 구조체에 스레드가 마지막으로 수행되었을 때의 CPU 레지스터들의 정보를 가지고 있다.

윈도우는 매 20ms 정도마다 모든 스레드 커널 오브젝트 중 스케줄 가능 상태에 있는 스레드 커널 오브젝트를 검색하고, 하나를 선택하여 CPU 레지스터로 로드한다.
* 이러한 작업을 컨텍스트 스위칭이라고 한다.

이와 같이 컨텍스트 스위칭이 일어나면 CPU 시간을 할당받은 스레드는 프로세스 주소 공간 내에 위치한 코드를 수행하고 데이터를 사용한다.

다시 20ms가 지나면 CPU 레지스터 정보를 스레드의 컨텍스트로 저장하고 스레드는 수행이 정지된다.

위 작업을 반복하며 스케줄링을 하게 된다.

윈도우는 언제라도 특정 스레드를 정지하고 다른 스레드를 수행할 수 있기 때문에
* 선점형 멀티스레드 기반 운영체제(preemptive multithreaded operating system)라고 불린다.

## Section 01 스레드의 정지와 계속 수행
스레드 커널 오브젝트 내에는 정지 카운트(suspend count)라는 값이 저장되어 있다.
* CreateProcess와 CreateThread를 호출하면 스레드 커널 오브젝트가 생성되고 정지 카운트가 1로 초기화된다.
* 이러면 스레드는 스케줄 불가능 상태가 된다.
* 스레드가 완전히 초기화되고 준비될 때까지 대기하는 것


스레드가 완전히 초기화되면
* CREATE_SUSPENDED 플래그가 전달되었는지 확인한다.
* 아니라면 정지 카운트를 0으로 감소시킨다. (스케줄 가능한 상태)

스레드를 정지 상태로 생성하면 스레드가 수행되기 전에 스레드의 수행 환경(우선순위 등)을 변경할 수 있다.
* 변경 후 반드시 스케줄 가능상태로 변경해주어야 스레드가 수행된다.
* 스레드 핸들 값을 인자로 ResumeThread 함수를 호출하면 된다.
  * 성공하면 이전 정지 카운트 값이 반환, 실패하면 0xFFFFFFFF 반환
* 스레드는 여러 번 정지될 수 있다. 3번 정지 -> 3번 수행 명령
* SuspendThread 함수를 호출하여 스레드를 정지시킬 수 있다.
  * 자신을 제외한 다른 스레드도 정지 가능
  * 이전 정지 카운트를 반환
  * 정지 카운트의 최대값은 MAXIMUM_SUSPEND_COUNT - 127
  * SuspendThread를 호출하면 커널 모드에서 수행중인 코드는 비동기적으로 수행
  * 유저 모드에서 수행 중인 코드는 즉시 정지되어 수행을 재개할 때까지 수행되지 않는다.

SuspendThread 함수를 사용할때는 세심한 주의가 필요하다.
* 수행 중인 스레드가 어떤 작업을 하는 중에 정지될지 알 수 없기 때문
* 만일 스레드가 힙으로부터 메모리를 할당하던 중에 정지되면 힙이 lock 된다.
* 다른 스레드가 힙에 접근하려 시도하면 정지된 스레드가 다시 수행되기 전까지는 수행이 정지된다.
* SuspendThread 함수는 스레드가 정지될 때 발생할 수 있는 문제들과 데드락을 피할 수 있는 명확한 방법이 있을 때만 사용되어야 한다.

## Section 02 프로세스의 정지와 계속 수행
프로세스는 CPU 시간을 할당받는 대상이 아니기 때문에 프로세스의 정지와 계속 수행이라는 개념은 존재하지 않는다.
* 매우 특수한 경우이긴 하지만 디버거의 경우 WaitForDebugEvent 함수가 반환한 디버그 이벤트를 처리하고 ContinueDebugEvent를 호출하기 전까지 디버거 프로세스 내의 모든 스레드를 정지상태로 만든다.
* Sysinternals가 제공하는 SuspendProcess 기능을 이용하면 프로세스 내의 모든 스레드를 정지시킬 수 있다.

윈도우는 경합 상태 (race condition)을 유발할 가능성이 있기 때문에 디버깅 메커니즘 외에는 모든 프로세스를 정지시키는 것을 제공하지 않는다.


SuspendProcess 유사 구현
~~~C++
VOID SuspendProcess(DWORD dwProcessID, BOOL fSuspend)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessID);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te = { sizeof(te) };
        BOOL fOk = Thread32First(hSnapshot, &te);

        for (; fOk; fOk = Thread32Next(hSnapshot, &te))
        {
            if (te.th32OwnerProcessID == dwProcessID)
            {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);

                if (hThread != NULL)
                {
                    if (fSuspend)
                    {
                        SuspendThread(hThread);
                    }
                    else
                    {
                        ResumeThread(hThread);
                    }

                    CloseHandle(hThread);
                }
            }
        }

        CloseHandle(hSnapshot);
    }
}
~~~


먼저 지정된 ThreadID에서 OpenThread를 호출하여 핸들값을 얻어낸다.
~~~c++
HANDLE OpenThread(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    DWORD dwThreadID
);
~~~
* 이 과정에서 스레드 커널 오브젝트의 사용카운트가 올라간다.
* 위 코드에서는 이 핸들을 이용하여 SuspendThread 함수를 호출하고 있다.

왜 SuspendProcess가 100% 확실하게 동작하지 않는가?
* 스레드 목록을 순회하는 동안에 새로운 스레드가 생성, 파괴될 수 있기 때문이다.
* 추후 정지된 스레드들이 다시 수행될 수 있도록 SuspendProcess를 호출하게 되면 정지되지 않았던 스레드를 다시 수행하게 된다.
* 더 나쁜 경우는 스레드 ID를 순회하는 동안 기존의 스레드가 종료되고 새로운 스레드가 생성되는 경우다. 이 두 개의 스레드가 같은 값을 가질수도 있다.

## Section 03 슬리핑
스레드는 Sleep 함수를 호출하여 일정 시간 동안 자신을 스케줄하지 않도록 운영체제에게 명령을 내릴 수 있다.
~~~C++
VOID Sleep(DWORD dwMilliseconds);
~~~
* 위 함수를 호출하면 매개변수로 전달된 시간만큼 스레드의 수행을 정지시킨다.

Sleep 함수에서 주목할만한 사항
* Sleep을 호출하면 스레드는 자발적으로 남은 타임슬라이스를 포기한다.
* 지정된 시간 동안 스레드를 스케줄 불가 상태로 만든다.
* INFINITE를 전달할 수 있다.
* 0을 전달할 수 있다. - 타임 슬라이스를 포기

## Section 04 다른 스레드로의 전환
윈도우는 스케줄 가능 상태에 있는 다른 스레드를 수행하기 위한 SwitchToThread 함수를 제공한다.
~~~C++
BOOL SwitchToThread();
~~~

* 이 함수를 호출하면 시스템은 일정 시간 동안 CPU 시간을 받지 못하여 수행되지 못하고 있던 스레드가 있는지 확인한다.
* 그러한 스레드가 없다면 바로 반환, 있다면 해당 스레드를 스케줄 한다.
* CPU 시간을 할당받은 스레드는 단일 퀀텀 시간 동안만 수행되며 이후에는 이전과 동일하게 스케줄링

이 함수를 이용하면 우선순위가 낮은 스레드가 빨리 리소스를 사용하고 반환할 수 있도록 한다.
* 수행할 스레드가 없다면 FALSE, 아니면 0이 아닌 값

SwitchToThread 함수는 Sleep과 유사하다.
* 단 SwitchToThread는 함수를 호출한 스레드보다 낮은 우선순위의 스레드도 수행될 수 있다.

## Section 05 하이퍼스레드 CPU 상에서 다른 스레드로의 전환

### CPU 파이프라인
1. 패치
2. 해석
3. 실행
4. 쓰기

하나씩 순차적으로 실행
* 각 단계에 한번만 중첩

### 슈퍼 파이프라인
* 파이프라인을 더욱 세분화하여 수행시간을 단축하는 것
* 하드웨어 장치의 연속적인 사용을 위해 몇 가지 동작을 명령어 수행과정에서 각 단계에 중첩하되 엇갈리게 중첩하는 기술
* 이전 명령어 사이클이 끝나기 전에 다음 명령어 사이클이 시작

### 하이퍼스레딩
* 하이퍼스레딩이 적용된 CPU는 다수의 논리적 CPU를 가지며 각기 다른 스레드를 수행할 수 있다.
* 자신만의 레지스터 정보를 가지고 있긴 하지만 CPU 캐시와 같은 주요 수행 자원은 공유하게 된다.

** 이해 잘 안됨


## Section 06 스레드 수행 시간
때로는 특정 작업을 완료하기 위해 스레드가 얼마만큼의 시간을 사용했는지 알아야할 필요가 있다.

GetTickCount64() 함수를 이용하여 수행시간 구하기
~~~c++
ULONGLONG qwStartTime = GetTickCount64();

// 알고리즘

ULONGLONG qwElapsedTime = GetTickCount64();
~~~

이 코드가 올바른 값을 얻으려면 코드 수행 중 인터럽트가 발생하지 않아야 한다.
* 선점형 운영체제에서는 언제 스레드가 CPU에 의해 수행될지 모르며, 현재 코드를 수행하는 중에 얼마든지 다른 작업을 진행할 수 있다.

실제로 CPU가 수행한 시간을 얻어오는 함수

~~~C++
BOOL GetThreadTimes(
    HANDLE hThread,
    PFILETIME pftCreationTimr,
    PFILETIME pftExitTime,
    PFILETIME pftKernelTime,
    PFILETIME pftUserTime
);
~~~

GetThreadTimes는 서로 다른 4개의 시간 값을 반환한다.
1. 생성 시간 : 1601년 1월 1일 자정으로부터 스레드가 생성된 시점까지의 시간을 100나노초 단위로 반환
2. 종료 시간 : 1601년 1월 1일 자정으로부터 스레드가 종료된 시점까지의 시간을 100나노초 단위로 반환
3. 커널 시간 : 커널 모드에서 운영체제가 제공하는 코드를 수행하는데 소요된 CPU 시간을 100나노초 단위로 반환
4. 유저 시간 : 애플리케이션 코드를 수행하는 데 소요된 CPU 시간을 100나노초 단위로 반환

~~~C++
BOOL GetProcessTimes(
    HANDLE hProcess,
    PFILETIME pftCreationTimr,
    PFILETIME pftExitTime,
    PFILETIME pftKernelTime,
    PFILETIME pftUserTime
);
~~~
* 프로세스 내의 모든 스레드들이 소요한 시간의 합을 얻어낼 수 있다.
* 종료된 스레드에 대해서도 계산된다.

윈도우 비스타 이전의 운영체제는 클록 타이머를 기반으로 10에서 15밀리초 단위로 CPU 단위를 계산하였다. - 이런 시간 값을 계산하기 위해 ClockRes라는 도구를 사용


하지만 윈도우 비스타 이후부터는 머신이 기동된 후부터 얼마만큼의 CPU 사이클이 수행되었는지를 저장하고 있는 64비트 값인 타임스탬프 카운터를 이용하여 CPU 시간을 계산한다.

스레드가 스케줄러에 의해 정지되면 현재의 TSC 값과 스레드가 재시작되었던 시점에 획득된 TCS 값과의 차를 계산한 후 스레드의 수행 시간에 더해준다.
* 인터럽트 시간은 고려되지 않는다.

WinNT.h 내에 정의된 ReadTimeStampCounter 매크로를 활용하여 TSC값을 얻어오는 것도 좋다.
* __rdtsc라는 내장 함수를 가리키도록 정의되어 있다.

좀 더 정밀한 시간 측정이 필요하다면 다음과 같은 함수를 사용 가능하다.
~~~c++
BOOL QueryPerformanceFrequency(LARGE_INTEGER *pliFrequency);
BOOL QueryPerformanceCounter(LARGE_INTEGER *pliCount);
~~~
* 이 함수들은 윈도우 스케줄러가 해당 스레드를 선점하지 않을 때 정확하게 시간 측정이 가능하다.
* 대부분의 경우는 코드 상의 일부분일 가능성이 많으므로 문제되지 않는다.

## Section 07 컨텍스트 내의 CONTEXT 구조체
스레드 스케줄링이 동작하는데 CONTEXT 구조체가 얼마나 중요한 역할을 하는지 정확하게 이해해야 한다.

CONTEXT 구조체는 시스템이 저장하는 스레드의 상태 정보로, 다음번에 CPU가 스레드를 수행할 때 어디서부터 수행을 시작해야 할지를 알려주는 역할을 담당한다.

~~~C++
typedef struct _C0NTEXT {
    //
    // 이 멤버의 플래그 값은 CONTEXT 레코드의 내용에 대한 제어를 수행한다.
    //
    // 컨텍스트 레코드가 입력 매개변수로 사용되는 경우 컨텍스트 레코드의
    // 각 영역은 어떤 플래그 값이 설정되는지에 따라 영역 내의 멤버를
    // 유효한 값으로 설정해야 한다. 만일 스레드의 컨텍스트 내용을
    // 변경하기 위해 CONTEXT 구조체를 사용하는 경우라면, 플래그 값에 따라
    // 해당 영역의 멤버 값만 유효하게 설정하면 된다.
    //
    // 만일 컨텍스트 레코드가 스레드 컨텍스트의 내용을 획득하기 위해
    // IN OUT 매개변수로 사용되면 플래그 값에 따라 지정된 영역의 값만
    // 반환된다.
    //
    // CONTEXT 구조체는 OUT 매개변수로만 사용되는 경우는 없다.
    //
    DWORD ContextFlags;

    //
    // 이 영역은 ContextFlags 내에 CONTEXT_DEBUG_REGISTERS가 포함되어 있는
    // 경우에만설정되거나 획득된다. CONTEXT_DEBUG_REGISTERS는
    // CONTEXT_FULL 플래그 내에 포함되어 있지 않음에 주의하라.
    //
    DWORD DrO;
    DWORD Drl;
    DWORD Dr2;
    DWORD Dr3;
    DWORD Dr6;
    DWORD Dr7;
    //
    // 이 영역은 ContextFlags 내에 CONTEXT_FLOATING_POINT가 포함되어 있는
    // 경우에만설정되거나 획득된다.
    //
    FLOATING_SAVE_AREA FloatSave;
    //
    // 이 영역은ContextFlags 내에 CONTEXT_SEGMENTS가 포함되어 있는
    // 경우에만설정되거나획득된다.
    //
    DWORD SegGs;
    DWORD SegFs;
    DWORD SegEs;
    DWORD SegDs;
    //
    // 이 영역은 ContextFlags 내에 CONTEXT_INTEGER가포함되어 있는
    // 경우에만설정되거나획득된다.
    //
    DWORD Edi;
    DWORD Esi；
    DWORD Ebx;
    DWORD Edx;
    DWORD Ecx;
    DWORD Eax;
    //
    // 이 영역은 ContextFlags 내에 CONTEXT_CONTROL이 포함되어 있는
    // 경우에만설정되거나획득된다.
    //
    DWORD Ebp;
    DWORD Eip;
    DWORD SegCs; //깨끗하게 초기화되어이2한다.
    DWORD EFlags; // 깨끗하게 초기화되어야한다.
    DWORD Esp;
    DWORD SegSs;
    //
    // 이 영역은 ContextFlags 내에 CONTEXT_E〉〈TENDED_REGISTERS가 포함되어 있는
    // 경우에만설정되거나 획득된다.
    // 그 형태와 내용은 프로세스별로 다르다.
    //
    BYTE ExtendedRegisters[ MAXIMUM_SUPPORTED_EXTENSION] ;
} CONTEXT;
~~~
CONTEXT 구조체는 몇 개의 영역으로 나뉘어 있다.
* CONTEXT_CONTROL 영역
  * 인스트럭션 포인터
  * 스택 포인터
  * 플래그
  * 함수 반환 주소
  * 등의 CPU 제어 레지스터 값을 가지고 있다.
* CONTEXT_INTEGER
  * CPU의 정수 레지스터 값
* CONTEXT_FLOATING_POINT
  * CPU의 부동소수점 레지스터
* CONTEXT_SEGMENTS
  * CPU의 세그먼트 레지스터
* CONTEXT_EXTENDED_REGISTERS
  * CPU의 확장 레지스터

윈도우에서는 이 컨텍스트 정보를 가져올 수 있도록
* GetThreadContext 함수를 제공하고 있다.
* 이 함수는 SuspendThread 함수를 먼저 호출하고 호출해야 한다.
* 수행중에 호출하면 얻고자 하는 값과 다른 값이 나올 수 있다.
* 유저 모드 컨텍스트를 가져오는 함수다.

## Section 08 스레드 우선순위
실제로 수행되는 스레드들은 다양한 우선순위를 가지고 있고, 이는 스케줄러가 다음에 수행할 스레드를 선택하는 과정에 영향을 준다.

모든 스레드는 0(가장 낮은) - 31(가장 높은) 범위내의 우선순위 번호를 가진다.
* 시스템은 라운드 로빈 방식으로 스케줄을 수행한다.

31번 우선순위의 스레드가 스케줄 가능한 상태에 있는 동안에는 하위 우선순위는 수행되지 못한다.
* 기아 상태
* 멀티 프로세스 머신에서는 비교적 적게 발생한다.

시스템 내의 대부분의 스레드들은 스케줄 불가능한 상태를 유지한다.

시스템이 부팅되면 제로 페이지 스레드라고 불리는 스레드가 생성된다.
* 어떠한 스레드도 스케줄 가능 상태가 아닐 때 램의 사용되지 않는 페이지를 0으로 만드는 작업을 수행한다.

## Section 09 우선순위의 추상적인 의미

윈도우는 6개의 우선순위 클래스
* 실시간 : 반드시 필요한 경우에만, 안하는 것이 좋다
* 높음
* 보통 이상
* 보통 : 99퍼 이상의 애플리케이션이 보통 우선순위로 수행된다.
* 보통 이하
* 유휴 상태 : 시스템이 아무것도 하지 않는 경우에만 수행되어야 하는 애플리케이션에 적합

우선순위 클래스를 선택하고나면 애플리케이션 내의 스레드에 대해 집중하는 것이 좋다.

윈도우는 7개의 상대 스레드 우선순위를 제공한다.
* 타임 크리티컬
* 가장 높음
* 보통 이상
* 보통
* 보통 이하
* 가장 낮음
* 유휴 상태

대부분의 우선순위 레벨은 8로 설정된다. (보통, 보통)
* 17 이상의 우선순위 레벨은 유저모드에서 사용될 수 없다.

## Section 10 우선순위 프로그래밍
프로세스에 우선순위를 설정하는 방법

CreateProcess 함수의 매개변수인 fdwCreate 매개변수로 우선순위 클래스를 전달
* 실시간(Real-time) : REALTIME_PRIORITY_CLASS
* 높음(High) : HIGH_PRIORITY_CLASS
* 보통 이상(Above normal) : ABOVE_NORMAL_PRIORITY_CLASS
* 보통(Normal) : NORMAL_PRIORITY_CLASS
* 보통 이하(Mow normal) : BELOW_NORMAL_PRIORITY_CUASS
* 유휴 상태 (Idle) : IDLE_PRIORITY_CLASS

프로세스가 수행된 이후에 변경하는 방법
~~~c++
BOOL SetPriorityClass (
    HANDLE hProcess,
    DWORD fdwPriority
);
~~~

프로세스가 최초로 생성되면 상대 스레드 우선순위는 항상 보통으로 설정된다.
~~~c++
BOOL SetThreadPriority (
    HANDLE hThread,
    int nPriority
);
~~~

nPriority에 올수 있는 인자
* 타임 크리티컬(Time—critical) : THREAD_PRIORITY_TIME_CRITICAL
* 가장 높음(Highest) : THREAD_PRIORITY_HIGHEST
* 보통 이상(Above normal) : THREAD_PRIORITY_ABOVE_NORMAL
* 보통(Normal) : THREAD_PRIORITY_NORMAL
* 보통 이하(B이ow normal) : THREAD_PRIORITY_BELOW_NORMAL
* 가장 낮음(Lowest) : THREAD_PRIORITY_LOWEST
* 유휴 상태 (Idle) : THREAD-PRIORITYJDLE

### 동적인 우선순위 레벨 상승
시스템은 스레드의 우선순위 레벨을 스레드의 상대 우선순위와 스레드가 속한 프로세스의 우선순위 클래스를 결합하여 산출한다. - 스레드의 기본 우선순위 레벨
* I/O 이벤트 응답 혹은 윈도우 메시지나 디스크를 읽기 위해 스레드의 우선순위 레벨을 상승시키기도 한다.

스레드는 하나의 타임 슬라이스 시간 동안만 15 우선순위 레벨에서 수행되도록 스케줄된다.\
타임 슬라이스가 만료되면 스레드의 우선순위 레벨을 1만큼 감소시켜 다음 타임 슬라이스가 주어질 때 14 우선순위 레벨로 작업한다.

우선순위 레벨이 1~15 사이인 스레드에 대해서 우선순위 레벨 상승을 시도한다.

오랫동안 스케줄 받지 못하면 우선순위를 15까지 상승시켜서 두 번의 퀀텀 시간 동안 스레드가 수행될 수 있도록 한다.


### 포그라운드 프로세스를 위한 스케줄러 변경
포그라운드 프로세스를 위해 윈도우는 일반적으로 사용하는 퀀텀 시간에 비해 좀 더 긴 시간의 퀀텀 시간을 제공할 수 있도록 한다.

## Section 11 선호도
스레드를 전체 CPU 중 일부 CPU에서만 수행되도록 설정하는 것
* SetProcessAffinityMask
