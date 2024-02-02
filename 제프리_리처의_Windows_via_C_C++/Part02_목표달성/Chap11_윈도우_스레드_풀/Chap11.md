# Chapter 11 윈도우 스레드 풀
I/O 컴플리션 포트는 완료 통지를 대기하고 있는 스레드들에게 작업을 분배하는 역할까지만 담당하기 떄문에 스레드를 생성하고 파괴하는 작업은 여전히 사용자가 직접 해야만 한다.

스레드를 생성하고 파괴하는 방법은 사용자별로 서로 다르게 구현할 수 있는 부분이다.
* 윈도우는 개발자들이 좀 더 쉽게 개발을 수행할 수 있도록 자체적인 스레드 풀 메커니즘을 제공하고 있다.
* 이를 이용하면 스레드의 생성, 파괴, 관리 작업을 좀 더 쉽게 구현할 수 있다.

새로운 스레드 풀 함수들을 이용하면 다음과 같은 작업을 수행할 수 있다.
* 비동기 함수 호출
* 시간 간격을 두고 함수 호출
* 커널 오브젝트 시그널되면 함수 호출
* 비동기 I/O 요청이 완료되면 함수 호출

마이크로소프트는 스레드 풀 API를 윈도우 2000부터 제공했다.
* 윈도우 비스타에서 스레드 풀의 구조를 완전히 재설계하였으며
* 이에 따라 새로운 스레드 풀 API를 제공하고 있다.
* 윈도우 비스타 이전의 운영체제에서 수행할 필요가 없는 프로그램을 개발하는 경우라면 가능한 새로운 API를 사용할 것을 추천한다.

일반적으로 프로세스가 초기화될 때는 스레드 풀 컴포넌트와 관련된 어떠한 부하도 발생하지 않는다.
* 새로운 스레드 풀 함수를 호출하게 되면 그 즉시 스레드 풀 운용에 필요한 커널 리소스들이 새로 생성되며
* 이러한 리소스들은 프로세스 종료 시까지 유지된다.

## Section 01 시나리오 1: 비동기 함수 호출
스레드 풀을 이용하여 비동기적으로 함수가 호출되도록 하려면 먼저 다음과 같은 원형의 사용자 정의 함수를 구현해야 한다.
~~~C++
VOID CALLBACK SimpleCallback(
    PTP_CALLBACK_INSTANCE pInstance, // 469쪽 콜백 종료 동작 절을 볼 것
    PVOID pvContext
);
~~~

스레드 풀에 의해 관리되는 스레드가 사용자 정의 함수를 수행하도록 작업 요청을 전달하려면
* TrySubmitThreadpoolCallback 함수를 호출한다.

~~~C++
BOOL TrySubmitThreadpoolCallback(
    PTP_SIMPLE_CALLBACK pfnCallback,
    PVOID pvContext,
    PTP_CALLBACK_ENVIRON pcbe // 471쪽 스레드 풀 커스터마이징 절을 볼 것
);
~~~
* 이 함수는 내부적으로 작업 항목을 생성하여 스레드 풀의 큐에 삽입해 준다. (PostQueuedCompletionStatus를 이용하여)
* 성공 시 TRUE, 실패 시 FALSE
* pfnCallback : 사용자 정의 함수를 전달
* pvContext : 사용자 정의 콜백 함수의 매개변수로 보낼 값, 단순히 NULL을 전달해도 된다.
* PTP_CALLBACK_ENVIRON : NULL을 전달해도 된다. - 추후 설명

CreateThread 함수를 전혀 호출할 필요가 없다.
* 기본 스레드 풀과 스레드 풀 내의 스레드는 자동으로 생성되고, 이렇게 생성된 스레드에 의해 콜백함수가 호출된다.
* 이 스레드는 클라이언트의 요청을 처리한 후에도 바로 종료되지 않고 스레드 풀 내로 돌아가서 다른 작업 항목이 삽입될 때까지 대기한다.
* 스레드 풀은 작업 항목이 삽입될 때마다 매번 스레드를 생성하고 파괴하지 않고 앞서 생성해둔 스레드를 재사용함으로 스레드의 생성과 파괴에 소요되는 CPU 시간을 절약하여 프로그램의 성능을 향상시킨다.
* 스레드 풀은 수행에 있어 더 많은 스레드를 생성하는 것이 효과적이다고 판단되면 스레드를 생성하고
* 아닌 경우에는 파괴하기도 한다.
* 스레드 풀을 이용하여 어떤 작업을 수행하게 될지 정확하게 알기 힘든 상황이라면 스레드 풀의 내부 알고리즘을 신뢰하는 것이 최선의 방법

### 명시적 작업 항목 제어
메모리 부족이나 메모리 할당 제한 등으로 인해 TrySubmitThreadpoolCallback 함수는 실패할 수 있다.
* 여러 개의 작업이 모여서 단일의 작업을 구성하는 경우 그 중 하나의 실패도 용납되면 안된다.

TrySubmitThreadpoolCallback 함수를 호출하면 내부적으로 작업 항목이 새로 생성된다.
* 많은 작업 항목을 한꺼번에 큐에 삽입하고자 하는 경우에 성능과 메모리 점유 상황을 고려하여 작업 항목을 하나씩 생성한 후 큐에 여러 번에 걸쳐 작업 항목을 삽입하는 것이 좋다.

작업 항목을 생성하는 함수
~~~C++
PTP_WORK CreateThreadpoolWork(
    PTP_WORK_CALLBACK pfnWorkHandler,
    PVOID pvContext,
    PTP_CALLBACK_ENVIRON pcbe
);
~~~
* 이 함수는 매개변수로 전달하는 세 개의 값을 저장하는 구조체를 생성하고 그 포인터를 반환한다.
* pfnWorkHandler : 스레드가 작업 항목을 처리하기 위해 호출해야 하는 콜백함수

pfnWorkHandler 매개변수를 통해 전달하는 함수는 다음과 같이 구현되어야 한다.
~~~C++
VOID CALLBACK WorkCallback(
    PTP_CALLBACK_INSTANCE pInstance,
    PVOID pvContext,
    PTP_WORK pWork
);
~~~

작업 항목을 스레드 풀의 큐에 삽입하려면 SubmitThreadpoolWork 함수를 호출하면 된다.
~~~C++
VOID SubmitThreadpoolWork(PTP_WORK pWork);
~~~
* 이제 작업 항목이 스레드 풀의 큐에 정상적으로 삽입되었다고 확신해도 된다.

삽입된 작업 항목을 취소하거나 작업 항목이 완전히 처리될 때까지 특정 스레드를 대기 상태로 두고자 한다면
* WaitForThreadpoolWorkCallbacks 함수를 호출한다.

~~~C++
VOID WaitForThreadpoolWorkCallbacks(
    PTP_WORK pWork,
    BOOL bCancelPendingCallbacks
);
~~~
* pWork : 이전에 스레드 풀 큐에 삽입한 작업 항목을 전달
* 작업 항목이 큐에 삽입되기 전에 함수를 호출했다면 어떤 동작도 수행되지 않고 반환
* bCancelPendingCallbacks : 매개변수를 TRUE로 전달하면 작업 항목을 취소하려 한다.
  * 이미 처리 중이라면 취소하지 않는다.
  * 큐에 존재하고 처리가 되지 않았다면 해당 작업 항목에 취소 표시를 한다.
  * I/O 컴플리션 포트의 컴플리션 큐로부터 이러한 작업 항목을 가져오면 해당 항목을 처리할 필요가 없다는 것을 알 수 있으므로 콜백함수는 시작되지 않는다.
* FALSE를 전달하면
  * 지정한 작업 항목이 완전히 처리된 후
  * 스레드 풀 스레드가 다른 작업 항목을 처리하기 위해 스레드 풀로 반환될 때까지 대기한다.


앞서 생성한 작업 항목이 더 이상 필요하지 않다면 CloseThreadpoolWork 함수에 작업 항목을 가리키는 포인터를 전달하여 해당 작업 항목을 삭제해야 한다.
~~~C++
VOID CloseThreadpoolWork(PTP_WORT pwk);
~~~

## Section 02 시나리오 2: 시간 간격을 두고 함수 호출
윈도우는 시간을 근간으로 한 통지를 수행하기 위한 방법으로 대기 타이머 커널 오브젝트를 제공한다.
* 서로 다른 시간 주기를 필요로 하는 작업에 대해 독립적인 대기 타이머 오브젝트를 생성하는데
* 이는 필요하지 않은 시스템 리소스를 낭비하는 꼴이다.
* 단일의 대기 타이머를 생성하여 주기를 재설정하거나, 시간을 재설정하는 방법이 좋다.
* 하지만 이렇게 하기 위해서는 코드를 복잡하게 작성해야 한다는 단점이 있다.

이런 경우 스레드 풀의 함수를 활용하면 좀 더 쉽게 코드를 구현할 수 있다.

지정한 시간에 특정 작업 항목이 수행될 수 있도록 스케줄 하기 위해서는 다음과 같은 형태의 함수를 구현해야 한다.
~~~C++
VOID CALLBACK TimeoutCallback(
    PTP_CALLBACK_INSTANCE pInstance,
    PVOID pvContext,
    PTP_TIMER pTimer
);
~~~
* pTimer의 값으로 CreateThreadpoolTimer의 반환 값을 전달

그런 다음 스레드 풀에게 언제 이 함수를 호출할지를 알려주기 위해 CreateThreadpoolTimer 함수를 호출
~~~C++
PTP_TIMER CreateThreadpoolTimer(
    PTP_TIMER_CALLBACK pfnTimerCallback,
    PVOID pvContext,
    PTP_CALLBACK_ENVIRON pcbe
);
~~~

스레드 풀 타이머를 스레드 풀에 등록하는 방법
* SetThreadpoolTimer 함수 호출
~~~C++
VOID SetThreadpoolTimer(
    PTP_TIMER pTimer,
    PFILETIME pftDueTime,
    DWORD msPeriod,
    DWORD msWindowLength
);
~~~
* pTimer : CreateThreadpoolTimer의 반환 값
* pftDueTime : 콜백함수를 최초로 호출해야 하는 시간, 음수값을 지정하면 상대적인 소요 시간
  * -1을 전달하면 콜백 함수가 바로 호출된다.
  * 절대 시간을 지정하려면 1600년 1월 1일부터 100나노초 단위를 전달하면 된다.
* msPeriod : 단 한번만 호출되길 원하면 0, 주기적으로 호출되길 원한다면 100밀리초 단위 지정
* msWindowLength : 콜백함수를 호출할 시간을 임의로 변경
  * pftDueTime의 시간부터 msWindowsLenth 값을 더한 시간 범위 내에서 임의 호출
  * 이 값은 여러 개의 타이머가 거의 동일한 주기를 가질 경우 동일 시간에 여러 개의 타이머 통지가 발생하지 않도록 하기 위해 사용 가능 

타이머에 대한 설정은 한 번 설정한 후에 가능한 한 변경하지 않는 것이 좋다.
* SetThreadpoolTimer를 호출할 때 앞서 사용했던 pTimer 매개변수 값으로
* pfnDueTime, msPeriod, msWindowLength 매개변수 값을 달리하면 타이머 설정을 변경할 수 있다.
* pftDueTime 매개변수로 NULL을 전달하면 타이머는 콜백함수를 더 이상 호출하지 않게 된다.

타이머가 현재 동작 중인지의 여부를 확인(pfnDueTime 값이 NULL이 아닌지)하기 위해
* IsThreadpoolTimerSet 함수를 호출할 수 있다.
~~~C++
BOOL IsThreadpoolTimerSet(PTP_TIMER pti);
~~~

타이머 작업이 완료될 때까지 스레드를 대기 시키는 함수
* WaitForThreadpoolTimerCallbacks

타이머를 삭제하는 함수
* CloseThreadpoolTimer

## Section 03 시나리오 3: 커널 오브젝트가 시그널되면 함수 호출
상당히 많은 프로그램들이 단지 커널 오브젝트가 시그널되기를 기다릴 목적으로 새로운 스레드를 생성하여 사용하고 있음을 발견하였다.
* 오브젝트가 시그널 상태가되면 다른 스레드에게 그 사실을 통지하고 다시 오브젝트가 시그널될 때까지 대기하기를 반복한다.
* 배치 예제에도 적용 가능한 시나리오다.


배치 예제의 경우
* 실제 작업이 완료되었는지를 확인하기 위한 특수한 작업 항목이 필요
* 이 작업 항목을 처리하기 위해 콜백함수가 호출되었을 때 앞서 요청한 모든 작업 항목들이 완료되었는지 여부를 확인하기 위해 특정 이벤트 커널 오브젝트가 시그널될 때까지 대기하도록 할 수 있다.

여러 개의 스레드가 단일의 오브젝트에 대해 시그널링을 대기하도록 구현하는 경우
* 시스템 리소스를 지나치게 낭비하는 결과를 가져옴
* 스레드를 생성하는 것이 프로세스보다는 비교적 적은 리소스를 사용하지만
* 스레드는 자신만의 스택을 가져야 하며, 스레드를 생성하고 파괴하는 과정에서 상당량의 CPU 명령을 수행해야만 한다.
* 개발자라면 항상 시스템 리소스의 낭비를 최소화할 수 있도록 노력해야 한다.

만약 특정 커널 오브젝트가 시그널될 때 처리되어야 하는 작업 항목을 등록하려 한다면

먼저 콜백함수를 구현해야 한다.
~~~C++
VOID CALLBACK WaitCallback(
    PTP_CALLBACK_INSTANCE pInstance,
    PVOID Context,
    PTP_WAIT Wait,
    TP_WAIT_RESULT WaitResult
);
~~~

스레드 풀 대기 오브젝트를 생성한다.
~~~C++
PTP_WAIT CreateThreadpoolWait(
    PTP_WAIT_CALLBACK pfnWaitCallback,
    PVOID pvContext,
    PTP_CALLBACK_ENVIRON pcbe
);
~~~

커널 오브젝트와 스레드 풀 대기 오브젝트를 연계시킨다.
~~~C++
VOID SetThreadpoolWait(
    PTP_WAIT pWaitItem,
    HANDLE hObject,
    PFILETIME pftTimeout
);
~~~
* pWaitItem : CreateThreadpoolWait의 반환 값
* hObject : 커널 오브젝트, 이 커널 오브젝트가 시그널 상태가되면 콜백함수 호출
* pftTimeout : 커널 오브젝트가 시그널될 때까지 얼마나 기다릴 것인지
  * 0을 입력하면 전혀 기다리지 않음
  * 음수 값을 전달하면 함수 호출로부터 상대적인 시간
  * NULL을 전달하면 무한 대기

내부적으로 스레드 풀은 WaitForMultipleObjects 함수를 호출하는 단일의 스레드를 가지고 있어\
SetThreadpoolWait 함수 호출 시마다 인자로 전달된 커널 오브젝트 핸들을 WaitForMultipleObjects의 인자로 사용하며\
bWaitAll 매개변수 값을 FALSE로 유지하여 다수의 커널 오브젝트 중 하나라도 시그널 상태가 되면 스레드가 깨어날 수 있도록 구현하고 있다.
* WaitForMultipleObjects의 함수가 64개의 커널 오브젝트만 대기하는 것이 가능하기 때문에
* 스레드 풀 또한 64개의 커널 오브젝트 핸들만을 다룰 수 있는 제약이 있다.


WaitForMultipleObjects를 호출할 때 동일 커널 오브젝트를 동시에 여러 번 사용할 수 없는 제약이 있는 것과 같이 SetThreadpoolWait 또한 동일 커널 오브젝트를 여러번에 걸쳐 등록할 수 없다.
* DuplicateHandle을 이용하여 핸들을 복사한 뒤 등록하는 것은 가능하다.

커널 오브젝트가 시그널(signal)되거나 대기 시간이 만료되면 스레드 풀 내의 스레드는 WaitCallback 함수를 호출하게 된다.
* 마지막 매개변수 WaitResult
  * TP_WAIT_RESULT 형 인자로, 왜 WaitCallback 함수가 호출되었는지 나타내는 값

WaitResult 매개변수가 가질 수 있는 값
* WAIT_OBJECT_0 : 대기 시간 만료 전에 시그널링되었다.
* WAIT_TIMEOUT : 대기 시간 만료
* WAIT_ABANDONED_0 : 뮤텍스 버림이 발생

스레드 풀의 스레드가 콜백함수를 호출하게 되면 해당 커널 오브젝트 핸들 항목은 비활성화된다.
* 비활성화 : 동일 커널 오브젝트가 시그널되었을 때 계속 콜백함수가 호출되려면 SetThreadpoolWait 함수를 통해 커널 오브젝트 핸들을 다시 등록해야 함


마지막으로 WaitForThreadpoolWaitCallbacks 함수로 특정 커널 오브젝트가 호출된 콜백함수가 작업을 마치고 반환될 때까지 대기

CloseThreadpoolWait을 호출하여 스레드 풀 대기 오브젝트를 삭제할 수 있다.


## Section 04 시나리오 4: 비동기 I/O 요청이 완료되면 함수 호출 
윈도우의 I/O 컴플리션 포트를 대기하는 스레드를 포함하는 스레드 풀을 어떻게 만들 수 있는지
* 내부적으로 스레드 풀에서 생성된 스레드들은 I/O 컴플리션 포트를 사용하기 때문에
* 파일이나 장치를 열었을 경우 그냥 스레드 풀의 I/O 컴플리션 포트와 연계시키고,
* 비동기 작업을 완료하였을 때 스레드 풀의 스레드가 어떤 함수를 호출할지를 지정하면 된다.

사용자 콜백함수 정의
~~~C++
VOID CALLBACK OverlappedCompletionRoutine(
    PTP_CALLBACK_INSTANCE pInstance,
    PVOID pvContext,
    PVOID pOverlapped,
    ULONG IoResult,
    ULONG_PTR NumberOfBytesTransferred,
    PPT_IO pIo
);
~~~
* I/O 작업이 완료되면 이 함수가 호출된다.
* OVERLAPPED 구조체를 가리키는 포인터를 인자로 받아온다.
* 작업의 수행결과는 IoResult 매개변수로 가져온다. 성공 시 NO_ERROR
* 송수신된 바이트 수는 NumberOfBytesTransferred 매개변수로 받는다.
* 스레드 풀의 I/O 오브젝트는 pIo 매개변수를 통해 전달된다.

스레드 풀 I/O 오브젝트 생성
* 이 함수를 통해 전달하는 파일/장치 핸들(FILE_FLAG_OVERLAPPED 플래그) 값은 스레드 풀 내부 I/O 컴플리션 포트와 연계된다.
~~~C++
PTP_IO CreateThreadpoolIo(
    HANDLE hDevice,
    PTP_WIN32_IO_CALLBACK pfnIoCallback,
    PVOID pvContext,
    PTP_CALLBACK_ENVIRON pcbe
);
~~~

I/O 오브젝트와 스레드 풀의 내부 I/O 컴플리션 포트와 연계시킨다.
~~~C++
VOID StartThreadpoolIo(PTP_IO pio);
~~~
* ReadFile이나 WriteFile을 호출하기 전에 반드시 StartThreadpoolIo를 호출해야 한다.
* 이 함수가 실패하면 콜백함수는 호출되지 않는다.

I/O 작업을 요청한 이후 콜백함수가 호출되는 것을 중단하는 함수
~~~C++
VOID CancelThreadpoolIo(PTP_IO pio);
~~~
* ReadFile이나 WriteFile 함수를 호출하고 FALSE가 발생했는데 ERROR_IO_PENDING이 아닌 다른 값을 반환하면 CancelThreadpoolIo 함수를 반드시 호출해주어야 한다.

핸들과 스레드의 연계성을 끊는 함수
~~~C++
VOID CloseThreadpoolIo(PTP_IO pio);
~~~

요청된 I/O 작업이 완료될 때까지 다른 스레드를 대기시키는 함수
~~~C++
VOID WaitForThreadpoolIoCallbacks(
    PTP_IO pio,
    BOOL bCancelPendingCallbacks
);
~~~
* bCancelPendingCallbacks : TRUE로 전달하면 처리가 시작되지 않은 요청들은 모두 취소
  * 이에 대한 완료 통지는 발생하지 않는다.

## Section 05 콜백 종료 동작
스레드 풀은 사용자가 작성한 콜백함수가 반환될 때 반드시 수행해야 하는 작업을 인자를 통해 지정할 수 있어 매우 편리하다.
* 사용자가 작성한 콜백 함수는 그 내용이 공개되지 않은 pInstance 매개변수를 갖는데
* 이 값으로 다음의 함수들 중 하나를 호출할 수 있다.

pInstance 함수들
~~~C++
VOID LeaveCriticalSectionWhenCallbackReturns(PTP_CALLBACK_INSTANCE pci, PCRITICAL_SECTION pcs);
VOID ReleaseMutexWhenCallbackReturns(PTP_CALLBACK_INSTANCE pci, HANDLE mut);
VOID ReleaseSemaphoreWhenCallbackReturns(PTP_CALLBACK_INSTANCE pci, HANDLE sem, DWORD crel);
VOID SetEventWhenCallbackReturns(PTP_CALLBACK_INSTANCE pci, HANDLE evt);
VOID FreeLibraryWhenCallbackReturns(PTP_CALLBACK_INSTANCE pci, HMODULE mod);
~~~
* pInstance 매개변수는 그 이름이 말해주듯 스레드 풀 내의 스레드가 현재 처리하고 있는 작업, 타이머, 대기 또는 I/O의 인스턴스를 대표하는 값이다.

콜백 종료 함수들과 각 함수의 종료 동작
* LeaveCriticalSectionWhenCallbackReturns : LeaveCriticalSection
* ReleaseMutexWhenCallbackReturns : ReleaseMutex
* ReleaseSemaphoreWhenCallbackReturns : ReleaseSemaphore
* SetEventWhenCallbackReturns : SetEvent
* FreeLibraryWhenCallbackReturns : FreeLibrary
  * 콜백함수가 반환된 이후 DLL 파일을 언로드

각각의 콜백 인스턴스별로 하나의 종료 동작만을 지정할 수 있다.

종료 함수와 더불어 pInstance를 사용하는 함수가 두 개 더 있다.
~~~C++
BOOL CallbackMayRunLong(PTP_CALLBACK_INSTANCE pci);
VOID DisassociateCurrentThreadFromCallback(PTP_CALLBACK_INSTANCE pci);
~~~

CallbackMayRunLong
* 스레드 풀이 항목을 어떻게 처리해야 할지 알려주는 역할
* 만일 콜백함수가 처리 시간이 오래걸릴 것으로 예상되는 경우 호출하는 것이 좋다.
* TRUE를 반환하면 스레드 풀은 큐에 삽입된 항목을 처리할 수 있는 다른 스레드가 있다는 것
* FALSE는 없다는 것
* 이 경우 스레드 풀을 좀 더 효과적으로 운영하기 위해서는 수행 시간이 오래 걸리는 작업을 여러개의 항목으로 나누고, 현재 콜백함수를 호출한 스레드로는 그 중 하나의 항목만을 수행하도록 하는 것이 좋다.

DisassociateCurrentThreadFromCallback
* 스레드 풀에게 논리적으로는 이미 작업이 종료되었음을 알려 줄 수 있다.
* 콜백함수가 반환되기만을 기다리던 스레드가 즉각 깨어날 수 있게 한다.

### 스레드 풀 커스터마이징
스레드 풀을 생성하는 함수를 호출할 때 PTP_CALLBACK_ENVIRON 형의 매개 변수를 전달할 수 있다.
* 대부분은 이 매개변수를 NULL로 전달하여 기본 설정을 유지한다.
* 하지만 스레드 풀의 기본 구성 정보를 프로그램의 특성에 맞추어 적절히 변경하고 싶을 수 있다.
  * 스레드의 최소, 최대 개수, 다수의 스레드 풀을 독립적으로 생성, 파괴

새로운 스레드 풀을 생성하는 함수
~~~C++
PTP_POOL CreateThreadpool(PVOID reserved);
~~~
* reserved 매개변수는 예약되어 있으므로 NULL 전달
* 새로운 스레드 풀을 참조하는 PTP_POOL 값 반환

스레드 풀의 최소, 최대 스레드 개수 설정
~~~C++
BOOL SetThreadpoolThreadMinimum(PTP_POOL pThreadPool, DWORD cthrdMin);
BOOL SetThreadpoolThreadMaximum(PTP_POOL pThreadPool, DWORD cthrdMost);
~~~
* 스레드 풀은 최소 스레드 개수로 지정된 수만큼 스레드 개수를 유지하고
* 최대 스레드 개수로 지정된 수만큼 스레드의 개수를 증가시킬 수 있다.
* 기본 스레드 풀은 최소 1, 최대 500

스레드 풀은 수행 효율을 증대시키기 위해 스레드를 생성하고 파괴할 수 있다.
* 스레드 풀 내의 스레드가 RegNotifyChangeKeyValue 함수를 호출한 이후에 파괴될 수 있으며
* 이 경우 윈도우는 더 이상 애플리케이션에게 레지스트리의 값이 변경되었음을 알리지 않게 된다.

위 문제를 해결하는 방법은
* RegNotifyChangeKeyValue 함수를 호출하는 스레드를 CreateThread 함수를 호출하여 독립적으로 생성하여 스레드가 파괴되지 않게 하는 것
* 최소, 최대 스레드 개수를 동일한 값으로 지정하여 스레드 풀 내의 어떤 스레드도 파괴되지 않도록 하는 것

스레드 풀 삭제 함수
~~~C++
VOID CloseThreadpool(PTP_POOL pThreadPool);
~~~
* 이 함수를 호출하고 나면 스레드 풀에 작업 항목을 더 이상 추가할 수 없다.
* 작업 항목을 수행 중이던 스레드가 있다면 현재 수행 중인 작업을 완료한 즉시 종료된다.
* 시작되지 않은 작업 항목들도 모두 취소된다.

사용자가 추가적인 스레드 풀을 생성하고 최소, 최대 스레드 개수를 설정한 경우 작업 항목에 적용할 수 있는 추가적인 설정 정보와 환경 정보를 저장하기 위해 콜백 환경을 구성할 수 있다.

WinNT.h 헤더 파일에는 이러한 콜백 환경을 표현하기 위해 다음과 같은 구조체를 정의하고 있다.
~~~C++
typedef struct _TP_CALLBACK_ENVIRON
{
    TP_VERSION                          Version;
    PTP_POOL                            Pool;
    PTP_CLEANUP_GROUP                   CleanupGroup;
    PTP_CLEANUP_GROUP_CANCEL_CALLBACK   CleanupGroupCancelCallback;
    PVOID                               RaceDll;
    struct _ACTIVATION_CONTEXT          *ActivationContext;
    PTP_SIMPLE_CALLBACK                 FinalizationCallback;
    union
    {
        DWORD                           Flags;
        struct
        {
            DWORD                       LongFunction    : 1;
            DWORD                       Private         : 31;
        } s;
    } u;
} TP_CALLBACK_ENVIRON, *PTP_CALLBACK_ENVIRON;
~~~
* 각각의 필드에 대해 내용을 변경할 수는 있지만 그렇게 하지 않는 것이 좋다.
* WinBase.h 헤더 파일에서 정의하고 있는 함수들을 이용하여 각 필드의 값을 변경하는 것이 바람직하다.

구조체가 적절한 초기 값을 가지도록 하는 함수
~~~C++
VOID InitializeThreadpoolEnvironment(PTP_CALLBACK_ENVIRON pcbe);
~~~
* Version 필드를 1로, 나머지 필드는 모두 0으로 초기화

스레드 풀 콜백 환경이 더 이상 필요하지 않다면 정리작업
~~~C++
VOID DestroyThreadpoolEnvironment(PTP_CALLBACK_ENVIRON pcbe);
~~~

Pool 멤버에 작업 항목을 수행할 스레드 풀 지정
~~~C++
VOID SetThreadpoolCallbackPool(PTP_CALLBACK_ENVIRON pcbe, PTP_POOL pThreadPool);
~~~
* 이 함수를 호출하지 않으면 작업 항목이 프로세스의 기본 스레드 풀에 의해 처리될 것임을 의미

작업 항목이 처리하는데 오랜 시간이 걸릴 것임을 알려주도록 콜백 환경을 변경하는 함수
* 좀 더 빠른 시간 안에 여러 개의 스레드들을 생성하여 각 작업 항목을 좀 더 효율적으로 서비스할 수 있게 된다.
~~~C++
VOID SetThreadpoolCallbackRunsLong(PTP_CALLBACK_ENVIRON pcbe);
~~~

스레드 풀 내에 작업 항목이 존재하는 동안 특정 DLL을 프로세스의 주소 공간에 로드한 상태를 유지하도록 하는 함수
~~~C++
VOID SetThreadpoolCallbackLibrary(PTP_CALLBACK_ENVIRON pcbe, PVOID mod);
~~~
* 데드락을 유발할 수 있는 경합 상태를 제거하기 위해 존재하는 함수기도 한다.
* 고급 기능 - 플랫폼 SDK 문서를 확인


### 스레드 풀을 우아하게 삭제하는 방법: 삭제 그룹
스레드 풀은 삽입된 작업 항목들이 언제 스레드 풀에 의해 처리되었는지를 알아내 스레드 풀을 우아하게 삭제하는 것은 어렵다.

스레드 풀을 좀 더 우아하게 삭제하기 위해 스레드 풀은 삭제 그룹(cleanup group)이라는 기능을 제공한다.
* 기본 스레드 풀에 대해서는 적용되지 않는다.
* 기본 스레드 풀은 프로세스가 수행되고 있는 동안 유지되며, 프로세스 종료 시점에 삭제되기 때문

먼저 CreateThreadpoolCleanupGroup을 호출하여 삭제 그룹을 생성
~~~C++
PTP_CLEANUP_GROUP CreateThreadpoolCleanupGroup();
~~~

이렇게 생성된 삭제 그룹을 TP_CALLBACK_ENVIRON 구조체와 연계
~~~C++
VOID SetThreadpoolCallbackCleanupGroup(
    PTP_CALLBACK_ENVIRON pcbe,
    PTP_CLEANUP_GROUP ptpcg,
    PTP_CLEANUP_GROUP_CANCEL_CALLBACK pfng
);
~~~
* TP_CALLBACK_ENVIRON 구조체의 CleanupGroup, CleanupGroupCancelCallback 필드의 값을 설정하게 된다.
* pfng : 삭제 그룹이 취소될 때 호출할 콜백함수의 주소 지정

NULL을 전달하거나 다음 함수를 구현
~~~C++
VOID CALLBACK CleanupGroupCancelCallback(
    PVOID pvObjectContext,
    PVOID pvCleanupContext
);
~~~

스레드 풀 생성 함수를 호출할 때 마지막 매개변수로 NULL 값 대신 PTP_CALLBACK_ENVIRON을 전달할 수 있으며 이 경우 생성된 작업 항목은 스레드 풀이 속한 삭제 그룹에 삽입되게 된다.

스레드 풀을 삭제하는 함수를 호출 시 해당 삭제 그룹으로부터 작업 항목을 삭제하게 된다.

사용자가 생성한 스레드 풀을 삭제하기 위한 함수
~~~C++
VOID CloseThreadpoolCleanupGroupMembers(
    PTP_CLEANUP_GROUP ptpcg,
    BOOL bCancelPendingCallbacks,
    PVOID pvCleanupContext
);
~~~
* 작업 항목이 남아있는 한 대기
* 처리되지 않은 작업 항목들을 취소하려면 bCancelPendingCallbacks을 TRUE로 전달
* FALSE를 전달하면 모든 작업 항목을 처리하고 반환 - CleanupContext : NULL로 전달

삭제 그룹 리소스 반환
~~~C++
VOID WINAPI CloseThreadpoolCleanupGroup(PTP_CLEANUP_GROUP ptpcg);
~~~

마지막으로 DestroyThreadpoolEnvironment와 CloseThreadpool 함수를 호출하여 스레드 풀을 우아하게 종료시킬 수 있다.