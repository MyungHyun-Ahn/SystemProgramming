# Chapter 10 동기 및 비동기 장치 I/O
이번 장에서 알아볼 것
* 고성능, 확장성, 응답성, 그리고 안정성 등을 고려하여 프로그램을 개발할 수 있는 마이크로소프트 윈도우의 기술들

확장성 있는 프로그램
* 적은 수의 동시 작업을 수행하는 것만큼이나 효율적으로 많은 수의 동시 작업을 처리할 수 있는 프로그램

스레드가 동기적인 장치 I/O를 요청하면 I/O 작업이 완료될 때까지 블로킹
* 이런 상황은 가능한 피하는 것이 좋다.

고성능의 확장성 있는 프로그램을 개발할 수 있게 해주는 I/O 컴플리션 포트(completion port)
* 장치에 대한 읽기와 쓰기를 수행할 때 응답을 대기할 필요가 없으므로 성능이 개선

## Section 01 장치 열기와 닫기
이번 장에서는 어떻게 하면 스레드가 장치로부터 응답을 대기하지 않으면서 이들과 통신을 수행할 수 있을지 논의

윈도우는 각 장치들 간의 차이점을 가능한 개발자에게 드러내지 않으려 함
* 장치를 열기만 하면 장치에 대한 읽기, 쓰기는 장치의 종류에 상관없이 사용될 수 있기 때문

각 함수들을 통해 장치 핸들을 생성 가능
* msdn 참고

장치에 대한 핸들을 알고 있을 때 장치의 타입을 알아내는 함수
~~~C++
DWORD GetFileType(HANDLE hDevice);
~~~
반환 값
* FILE_TYPE_UNKNOWN
* FILE_TYPE_DISK : 디스크에 저장된 파일
* FILE_TYPE_CHAR : LPT 장치나 콘솔과 같은 전형적인 문자 기반 장치
* FILE_TYPE_POPE : 파이프


### CreateFile에 대한 세부사항 검토
CreateFile 함수를 이용하면 디스크에 새로운 파일 생성 혹은 기존 파일 열기를 수행 가능
* 뿐만 아니라 파일이 아닌 다른 장치 또한 열기 작업이 가능하다.

~~~C++
HANDLE CreateFile(
    PCTSTR pszName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    PSECURITY_ATTRIBUTES psa,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hFileTemplate
);
~~~
* pszName : 특정 장치의 인스턴스 값, 장치의 타입을 구분할 수 있는 값 전달
* dwDesiredAccess : 장치와 데이터를 어떻게 주고받을지 결정
  * 0 : 장치에 대해 읽거나 쓰지 않음
  * GENERIC_READ : 읽기만 수행
  * GENERIC_WRITE : 쓰기만 수행
  * | 연산으로 2개 전달 가능
* dwSharedMode : 장치의 공유 특성 지정
  * 0 : 배타적 접근 - 이미 열려있는 장치에 시도하면 실패
  * FILE_SHARE_READ : 다른 장치가 변경 못하도록 염
  * FILE_SHARE_WRITE : 다른 장치가 못 열게 함
  * | 연산 2개 : 다른 장치가 읽던 쓰던 신경쓰지 않는다.
  * FILE_SHARE_DELETE : 파일이 삭제되거나 옮겨지는 것에 신경쓰지 않는다.
* psa : 보안 속성
* dwCreationDisposition : 파일 장치에 사용할 때 가장 큰 의미
  * CREATE_NEW : 새로운 파일 생성, 이미 있으면 실패
  * CREATE_ALWAYS : 새로운 파일 생성, 이미 있으면 덮어씀
  * OPEN_EXISTING : 기존에 존재하는 파일 열기, 없으면 실패
  * OPEN_ALWAYS : 기존 파일 열기, 없으면 생성
  * TRUNCATE_EXISTING : 기존 파일 열고, 크기 0으로, 없으면 실패
* dwFlagsAndAttributes
  * 세부적인 통신 플래그 설정
  * 파일 특성 설정

CreateFile 캐시 플래그
* FILE_FLAG_NO_BUFFERING
* FILE_FLAG_SEQUENTIAL and FILE_FLAG_RANDOM_ACCESS
* FILE_FLAG_WRITE_THROUGH

기타 CreateFile 플래그
* FILE_FLAG_DELETE_ON_CLOSE
* FILE_FLAG_BACKUP_SEMANTICS
* FILE_FLAG_POSIX_SEMANTICS
* FILE_FLAG_OPEN_REPARSE_POINT
* FILE_FLAG_OPEN_NO_RECALL
* FILE_FLAG_OVERLAPPED

## Section 02 파일 장치 이용
파일 장치를 이용하는 방법
* 파일 포인터 위치 변경 방법
* 파일 크기 변경 방법

윈도우 운영체제는 최초 설계 시부터 64비트 값을 이용
* 16EB(엑사바이트) 까지 저장 가능

파일 크기 얻기
~~~C++
BOOL GetFileSizeEx(
    HANDLE hFile,
    PLARGE_INTEGER pliFileSize
);
~~~
* hFile : 파일 핸들
* pliFileSize : 64비트 공용체

LARGE_INTEGER 구조체
* ULARGE_INTEGER도 있다. unsigned 버전

또 다른 함수
~~~C++
DWORD GetCompressedFileSize(
    PCTSTR pszFileName,
    PDWORD pdwFileSizeHigh
);
~~~
* GetFileSizeEx 함수는 논리적인 크기를 반환
* 이 함수는 실제 디스크를 점유한 크기를 반환
* pszFileName : 실제 파일 경로를 입력
* 하위 32비트 값은 pdwFileSizeHigh, 상위 32비트 값은 반환 값


### 파일 포인터 위치 지정
CreateFile을 호출하면 파일 커널 오브젝트를 생성
* 내부적으로 파일 포인터를 가지고 있다.

파일을 열면 파일 포인터 0
* ReadFile을 호출하면 파일 포인터 위치부터 읽는다.

파일의 임의 위치에 접근하려는 경우 파일 포인터 값을 변경하면 된다.
~~~C++
BOOL SetFilePointerEx(
    HANDLE hFile,
    LARGE_INTEGER liDistanceToMove,
    PLARGE_INTEGER pliNewFilePointer,
    DWORD dwMoveMethod
);
~~~
* liDistanceToMove : 얼마나 이동할건지 바이트 단위 - 음수 지정 가능
* pliNewFilePointer : 갱신된 파일포인터 값 반환
* dwMoveMethod : 이동의 기준 플래그
  * FILE_BEGIN : liDistanceToMove로 전달된 값으로 파일포인터 설정
  * FILE_CURRENT : 기존 파일 포인터에 더하기
  * FILE_END : 논리적 파일 크기에 파일 포인터 만큼 더한 값

### 파일의 끝 설정
일반적으로 시스템은 파일을 닫을 때 파일의 끝을 설정하는 작업을 수행
* 때로는 파일을 닫기 전에 파일을 더 작거나 크게 변경할 필요가 있다.
* 이 작업을 SetEndOfFile 함수가 처리한다.

~~~C++
BOOL SetEndOfFile(HANDLE hFile);
~~~
* 파일 커널 오브젝트의 파일 포인터가 가리키는 현재 위치를 파일의 끝으로 설정
* 기존 파일의 크기를 더 크게 확장 혹은 작게 줄일 수 있다.

## Section 03 동기 장치 I/O 수행
여기서 장치는 파일, 메일슬롯, 파이프, 소켓 등을 말하는 것이다.
* 어떤 장치던 동일한 함수를 통해 I/O 작업을 수행할 수 있다.

~~~C++
BOOL ReadFile(
    HANDLE hFile,
    PVOID pvBuffer,
    DWORD nNumBytesToRead,
    PDWORD pdwNumBytes,
    OVERLAPPED *pOverlapped
);

BOOL WriteFile(
    HANDLE hFile,
    CONST VOID *pvBuffer,
    DWORD nNumBytesToWrite,
    PDWORD pdwNumBytes,
    OVERLAPPED *pOverlapped
);
~~~
* 장치를 열 때 FILE_FLAG_OVERLAPPED 플래그를 사용하면 시스템은 비동기 I/ O로 인식한다.
* 성공 시 return TRUE
* ReadFile은 GENERIC_READ 플래그를 포함하여 장치가 열려야 성공적 수행
* WriteFile은 GENERIC_WRITE 플래그를 포함하여 장치가 열려야 성공적 수행 

### 장치로 데이터 플러시하기
캐시된 데이터를 장치로 플러시하기 위해서는 FlushFileBuffers 함수를 사용할 수 있다.

~~~C++
BOOL FlushFileBuffers(HANDLE hFile);
~~~
* 캐시된 데이터를 강제적으로 쓰게 한다.
* 이 작업을 수행하려면 GENERIC_WRITE 플래그를 포함하여 장치를 열어야 한다.
* 성공 시 return TRUE

### 동기 I/O의 취소
동기 I/O를 수행하는 함수는 사용하긴 쉽지만 요청한 I/O가 성공할 때까지 스레드가 정지된다.
* 동기 I/O의 동작을 대기한다.


응답성이 좋은 프로그램을 만들기 위해서는 비동기 I/O를 수행하는 것이 좋다.
* 스레드 개수 줄일 수 있고, 스레드 커널 오브젝트, 스택과 같은 리소스를 절약

CreateFile과 같은 일부 윈도우 API는 비동기적으로 함수를 호출하는 방법을 제공하지 않는다.
* 타임아웃으로 반환되그는 한다.
* 동기 I/O를 강제로 취소하여 스레드가 계속해서 수행될 수 있도록 해주는 API가 최상의 방법

특정 스레드의 동기 I/O를 취소할 수 있는 함수
~~~C++
BOOL CancelSynchronousIo(HANDLE hThread);
~~~
* hThread 매개변수는 동기 I/O로 인해 정지된 스레드의 핸들을 전달
* THREAD_TERMINATE 접근 권한이 부여되어 있어야 한다.
* 그러지 않은 경우 함수는 실패하고 GetLastError의 결과로 ERROR_ACCESS_DENIED가 반환
* 일반적으로 스레드를 생성하는 경우 THREAD_ALL_ACCESS 권한으로 이미 포함되어 있다.

## Section 04 비동기 장치 I/O의 기본
장치에 비동기적으로 접근하려면 장치를 열 때(CreateFile)의 플래그 값으로 FILE_FLAG_OVERLAPPED 플래그를 전달해야 한다.
* 시스템에 이 장치에 비동기 접근을 할 것이라 알림

### OVERLAPPED 구조체
비동기 장치 I/O를 수행하려면 pOverlapped 매개변수를 통해 초기화된 OVERLAPPED 구조체를 가리키는 주소를 전달해야 한다.
* overlapped (중첩)의 의미는 동일 스레드가 다른 작업을 수행하는 동안 또 다시 새로운 I/O 작업을 시작하는 것을 의미한다.

OVERLAPPED 구조체
~~~C++
typedef struct _OVERLAPPED
{
    DWORD Internal;         // [OUT] 에러 코드
    DWORD InternalHigh;     // [OUT] 전송된 바이트 수
    DWORD Offset;           // [IN]  32 비트 하위 파일 오프셋
    DWORD OffsetHigh;       // [IN]  32 비트 상위 파일 오프셋
    HANDLE hEvent;          // [IN]  이벤트 핸들이나 데이터
} OVERLAPPED, *LPOVERLAPPED;
~~~
* Offset, OffsetHigh, hEvent 멤버는 WriteFile 등의 함수를 호출하기 전에 초기화되어야 한다.
* 나머지 2개의 멤버는 디바이스 드라이버에 의해 설정된다.
* I/O 작업이 완료되었는지 여부를 확인하기 위해 사용 가능
* Offset, OffsetHigh : 어디부터 파일을 읽을 것인지 위치
  * 파일 외에 다른 장치를 사용하는 경우 0을 전달하지 않으면 INVALID_PARAMETER
* hEvent : 완료 통지를 수신하는데 네 가지 방법 중 1가지
  * 임의의 C++ 오브젝트를 담을 수 있다.
* Internal : 이미 처리된 I/O의 에러 코드를 담는데 사용
* InternalHigh : 비동기 I/O 작업이 완료되면 실제로 송수신된 바이트 수 저장

### 비동기 장치 I/O 사용 시 주의사항
비동기 I/O를 수행할 때 유념해야할 사항
* 디바이스 드라이버가 비동기 I/O를 선입선출 방식으로만 처리하지 않는다.
  * ex Read -> Write 순서로 요청을 해도 Write가 먼저 끝날 수도 있다.
  * 수행 성능을 개선하기 위해 I/O 요청을 순서대로 수행하지 않는다.
* 에러 확인을 수행하는 적당한 방법에 대해 알아야 한다.
  * ReadFile 등의 함수는 비동기 적으로 실행되면 FALSE를 반환
  * GetLastError 함수를 호출하여 결과 값의 의미를 다시 확인해야 한다.
  * ERROR_IO_PENDING 이외의 값이라면 진짜 문제인 상황
* 비동기 I/O 요청을 수행할 때 사용되는 데이터 버퍼와 OVERLAPPED 구조체는 I/O 요청이 완료될 때까지 옮겨지거나 삭제되지 않아야 한다.
  * 주소만 전달하기 때문


### 요청된 장치 I/O의 취소
* CancelIo 함수를 호출하여 이 함수가 호출한 스레드가 삽입한 모든 I/O 요청을 삭제 가능
  * 핸들이 I/O 컴플리션 포트와 연계되지 않았다면
* 스레드를 고려하지 않고 해당 장치의 요청을 모두 닫고 싶다면 장치 핸들을 닫으면 된다.
* 컴플리션 포트와 연계되어 있는 경우를 제외하면 스레드가 종료될 때 스레드가 삽입된 모든 I/O 요청이 삭제
* 특정 장치에 대해 하나의 I/O 요청만을 취소하고 싶다면
  * CancelIoEx 함수 호출

I/O 요청이 취소되면 ERROR_OPERATION_ABORTED 에러 코드를 받게 된다.


## Section 05 I/O 요청에 대한 완료 통지의 수신
I/O 완료 통지 수신 방식 4가지
* 디바이스 커널 오브젝트의 시그널링
  * 단일의 장치에 대해 다수의 I/O 요청을 수행하는 경우 적합하지 않다.
  * 특정 스레드가 I/O 요청을 삽입하고 다른 스레드가 완료 통지를 수신할 수 있다.
* 이벤트 커널 오브젝트의 시그널링
  * 단일의 장치에 대해 다수의 I/O 요청을 할 수 있다.
  * 특정 스레드가 I/O 요청을 삽입하고 다른 스레드가 완료 통지를 수신할 수 있다.
* 얼러터블 I/O
  * 단일의 장치에 대해 다수의 I/O 요청을 수행할 수 있다.
  * 항상 I/O 요청을 삽입한 스레드가 완료 통지를 수신한다.
* I/O 컴플리션 포트
  * 단일의 장치에 대해 다수의 I/O 요청을 수행할 수 있다.
  * 특정 스레드가 I/O 요청을 삽입하고 다른 스레드가 완료 통지를 수신할 수 있다.
  * 이 방법이 가장 확장성이 뛰어나고 유연성이 있다.

### 디바이스 커널 오브젝트의 시그널링
스레드가 비동기 I/O 요청을 시도하면 스레드는 멈추지 않고 계속 작업을 수행한다.\
그러나 어느 순간 장치에서 버퍼로 데이터를 완전히 읽기 전까지 기다려야 하는 상황이 생긴다.


디바이스 커널 오브젝트 또한 시그널 상태를 가지므로 스레드 동기화에 사용 가능하다.
* I/O 요청을 삽입되기 전 : 논시그널
* I/O 요청의 처리를 마치면 : 시그널

비동기 요청의 완료 여부를 WaitForSingleObject나 WaitForMultipleObject 함수를 통해 알 수 있다.

~~~C++
HANDLE hFile = CreateFile(..., FILE_FLAG_OVERLAPPED, ...);
BYTE bReadBuffer[100];
OVERLAPPED o = { 0 };
o.Offset = 345;

BOOL bReadDone = ReadFile(hFile, bBuffer, 100, NULL, &o);
DWORD dwError = GetLastError();

if (!bReadDone && (dwError == ERROR_IO_PENDING))
{
  // I/O 요청이 비동기적으로 수행되고 있으며
  // 작업이 완료될 때까지 대기
  WaitForSingleObject(hFile, INFINITE);
  bReadDone = TRUE;
}

if (bReadDone)
{
  // o.Internal은 I/O 에러 코드를 가지고 있다.
  // o.InternalHigh는 읽은 데이터의 크기
  // bBuffer에 읽은 데이터가 있다.
}
else
{
  // 에러 발생 dwError 값 확인
}
~~~

중요한 개념
* 비동기 I/O를 위한 디바이스를 열 때는 반드시 FILE_FLAG_OVERLAPPED
* OVERLAPPED 구조체의 Offset, OffsetHigh, hEvent는 반드시 초기화
* ReadFile의 반환 값은 bReadDone에 저장, 이 값은 동기적으로 수행되었는지 여부를 반환
* ERROR_IO_PENGING이 아니면 비동기적으로 수행 여부
* 데이터가 모두 읽혀질 때까지 대기 WaitForSingleObject
* 읽기 작업이 완료
  * bBuffer : 읽은 데이터
  * o.Internal : 에러코드
  * o.InternalHigh : 읽은 데이터 크기
* 에러가 발생했다면 GetLastError

### 이벤트 커널 오브젝트의 시그널링
디바이스 커널 오브젝트 시그널링 방법은 간단하긴 하지만 다수의 I/O 요청에 대한 처리를 수행할 수 없다.
* 여러번 수행하면 어떤 작업이 완료되었는지 알지 못한다.
* 다른 대안이 필요함

OVERLAPPED 구조체의 hEvent 멤버는 이벤트 커널 오브젝트의 핸들을 저장할 수 있다.
* 여기에 저장할 핸들은 CreateEvent

비동기 I/O 요청이 완료되면 가장 먼저 hEvent의 멤버가 NULL인지 체크한다.
* 만일 NULL이 아니면 SetEvent 호출
* 물론 디바이스 오브젝트 또한 시그널 상태로 만들어주기도 한다.
* 이때 I/O 작업의 완료 여부를 hEvent 오브젝트로 수행하면 된다.

여러 번의 비동기 장치 I/O 요청을 동시에 수행하기를 원하면
* 각 요청마다 서로 다른 이벤트 커널 오브젝트를 생성해야 한다.

성능을 조금이라도 향상시키기를 원한다면.
~~~C++
BOOL SetFileCompletionNotificationModes(HANDLE hFile, UCHAR uFlags);
~~~
* uFlags 값으로 FILE_SKIP_SET_EVENT_ON_HANDLE 플래그를 전달하면 I/O 작업이 완료돼도 파일 핸들을 시그널 상태로 변경하지 않는다.

I/O 완료 요청에 대한 동기화가 필요해지면 I/O 작업을 수행할 때 사용했던 이벤트 핸들들을 인자로 WaitForMultipleObjects 함수를 호출하면 된다.

~~~C++
HANDLE hFile = CreateFile(..., FILE_FLAG_OVERLAPPED, ...);

BYTE bReadBuffer[10];
OVERLAPPED oRead = { 0 };
oRead.Offset = 0;
oRead.hEvent = CreateEvent(...);
ReadFile(hFile, bReadBuffer, 10, NULL, &oRead);

BYTE bWriteBuffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
OVERLAPPED oWrite = { 0 };
oWrite.Offset = 0;
oWrite.hEvent = CreateEvent(...);
ReadFile(hFile, bWriteBuffer, 10, NULL, &bWriteBuffer);

...

HANDLE h[2];
h[0] = oRead.hEvent;
h[1] = oWrite.hEvent;

DWORD dw = WaitForMultipleObjects(2, h, FALSE, INFINITE);
switch (dw - WAIT_OBJECT_0)
{
case 0: // 읽기 완료
  break;
case 1: // 쓰기 완료
  break;
}
~~~

### GetOverlappedResult
마이크로소프트가 최초에 OVERLAPPED 구조체를 설계했을 당시
* Internal, InternalHigh 멤버에 대해 문서화를 하지 않았다.

결국 I/O 작업이 수행된 후 얼마만큼의 데이터가 송수신되었는지 I/O 과정에서 에러가 발생하지 않았는지를 알아내기 위한 다른 방법이 있어야 함을 의미

GetOverlappedResult 함수 제공
~~~C++
BOOL GetOverlappedResult(
  HANDLE hFile,
  OVERLAPPED *pOverlapped,
  PDWORD pdwNumBytes,
  BOOL bWait
);
~~~

GetOverlappedResult의 내부적인 구현 내용
~~~C++
BOOL GetOverlappedResult(
  HANDLE hFile,
  OVERLAPPED *po,
  PDWORD pdwNumBytes,
  BOOL bWait)
{
  if (po->Internal == STATUS_PENDING)
  {
    DWORD dwWaitRet = WAIT_TIMEOUT;
    if (bWait)
    {
      dwWaitRet = WaitForSingleObject(
        (po->hEvent != NULL) ? po->hEvent : hFile, INFINITE
      );
    }
    if (dwWaitRet == WAIT_TIMEOUT)
    {
      // I/O 작업이 완료되지 않았고, 대기하지 않을 것이라면
      SetLastError(ERROR_IO_INCOMPLETE);
      return FALSE;
    }

    if (dwWaitRet != WAIT_OBJECT_0)
    {
      // WaitForSingleObject 호출에 실패
      return FALSE;
    }
  }

  *pdwNumBytes = po->InternalHigh;

  if (SUCCEEDED(po->Internal))
  {
    return TRUE;
  }

  // I/O 에러를 마지막 에러로 설정
  SetLastError(po->Internal);
  return FALSE;
}
~~~

### 얼러터블 I/O
스레드가 생성되면 시스템은 각 스레드별로 APC 큐라고 불리는 큐를 하나씩 생성
* 비동기 I/O 요청을 전달하는 함수를 호출할 때 디바이스 드라이버에거 I/O 작업 완료 통지를 스레드의 APC 큐에 삽입해 줄 것을 요청할 수 있다.
* 이를 위해 ReadFileEx, WriteFileEx 함수를 사용하면 된다.

~~~C++
BOOL ReadFileEx(
  HANDLE                            hFile,
  PVOID                             pvBuffer,
  DWORD                             nNumBytesToRead,
  OVERLAPPED                        *pOverlapped,
  LPOVERLAPPED_COMPLETION_ROUTINE   pfnCompletionRoutine
);

BOOL WriteFileEx(
  HANDLE                            hFile,
  CONST VOID                        *pvBuffer,
  DWORD                             nNumBytesToWrite,
  OVERLAPPED                        *pOverlapped,
  LPOVERLAPPED_COMPLETION_ROUTINE   pfnCompletionRoutine
);
~~~
* 위 함수는 호출 즉시 반환된다.
* I/O 작업이 수행된 바이트 수를 돌려받지 않는다. - 콜백 함수를 통해 획득
* 콜백함수의 주소를 필요로 한다.

~~~C++
VOID WINAPI CompletionRoutine(
  DWORD dwError,
  DWORD dwNumBytes,
  OVERLAPPED *po;
);
~~~
* 컴플리션 루틴은 반드시 위와 같이 구현되어야 한다.

WriteFileEx, ReadFileEx 함수를 이용하여 비동기 I/O를 수행하면
* 위 함수들이 디바이스 드라이버에게 컴플리션 루틴의 주소 값을 전달한다.
* 디바이스 드라이버가 I/O 요청을 마치면 스레드 APC 큐에 완료 통지를 나타내는 항목을 추가한다.
* 이 항목에는 컴플리션 루틴의 주소와 최초 I/O 요청 시 사용되었던 OVERLAPPED 구조체의 주소가 포함된다.

얼러터블 I/O를 사용하는 경우 디바이스 드라이버가 이벤트 커널 오브젝트에 대한 시그널링을 전혀 시도하지 않는다.
* hEvent 멤버를 다른 용도로 사용할 수 있다.

스레드가 얼러터블 상태가되면 시스템은 APC 큐의 내용을 확인하여 큐에 삽입된 모든 항목에 대해 컴플리션 루틴을 호출한다.

APC 큐는 시스템에 의해 내부적으로 관리된다.
* 어떤 순서로 완료될지 모른다.

APC 큐의 각 항목
* 컴플리션 루틴의 주소
* 컴플리션 루틴에 전달할 값

I/O 요청이 완료되면 이러한 항목들이 스레드의 APC 큐에 삽입
* APC 큐에 항목이 추가되도 바로 호출되지는 않는다.
* APC 큐의 항목을 처리하려면 스레드가 얼러터블 상태가 되어야 한다.

스레드를 얼러터블 상태로 바꿀 수 있는 함수
~~~C++
DWORD SleepEx(
  DWORD dwMilliseconds,
  BOOL bAlertable
);

DWORD WaitForSingleObjectEx(
  HANDLE hObject,
  BOOL bAlertable
);

DWORD WaitForMultipleObjectsEx(
  DWORD cObjects,
  CONST HANDLE *phObjects,
  BOOL bWaitAll,
  DWORD dwMilliseconds,
  BOOL bAlertable
);

BOOL SingalObjectAndWait(
  HANDLE hObjectToSignal,
  HANDLE hObjectToWaitOn,
  DWORD dwMilliseconds,
  BOOL bAlertable
);

BOOL GetQueuedCompletionStatusEx(
  HANDLE hCompPort,
  LPOVERLAPPED_ENTRY pCompPortEntries,
  ULONG ulCount,
  PULONG pulNumEntriesRemoved,
  DWORD dwMilliseconds,
  BOOL bAlertable
);

DWORD MsgWaitForMultipleObjectsEx(
  DWORD nCount,
  CONST HANDLE *pHandles,
  DWORD dwMilliseconds,
  DWORD dwWakeMask,
  DWORD dwFlags
);
~~~
* 1~5 번의 함수는 마지막 인자로 스레드를 얼러터블 상태로 변경할 것인지 부울 값을 받는다.
* MsgWaitForMultipleObjectsEx의 경우 MWMO_ALERTABLE 플래그를 이용하여 스레드를 얼러터블 상태로 변경할 수 있다.
* 내부적으로 Ex 가 붙지 않은 함수들은 bAlertable 매개변수로 FALSE를 지정하여 Ex 붙은 함수를 호출한다.

스레드가 얼러터블 상태가 되면
* 시스템은 스레드의 APC 큐에 항목이 존재하는지 판단
* 큐에 하나 이상의 항목이 있다면
* 스레드를 대기 상태로 전환하지 않고 APC 큐에 있는 항목을 하나씩 꺼내 처리
* 이때 완료된 I/O 요청에 대한 에러코드와 송수신 바이트 수, OVERLAPPED 구조체의 주소가 같이 전달된다.

APC 큐가 완전히 비워지면 얼러터블 상태로 변경하기 위해 호출했던 함수가 반환된다.
* 스레드가 APC 큐에 항목이 존재하는 경우 위 함수들을 호출하면 스레드가 대기 상태로 전환되지 않는다.

스레드의 APC 큐에 어떠한 항목도 없는 경우에 함수 호출
* 커널 오브젝트가 시그널 상태가 되거나 APC 큐에 항목이 삽입될 때 수행을 재개한다.

6개의 함수 반환 값
* WAIT_IO_COMPLETION : 적어도 한 개 이상의 항목이 APC 큐에 존재했고 처리했다는 것


### 얼러터블 I/O의 장단점
얼러터블 I/O가 사용하기 어려운 이유
1. 콜백함수
2. 스레딩 문제 : I/O를 요청한 스레드가 완료 통지도 함께 처리
   * 부하 분산을 제대로 처리하지 못한다.
   * 확장성 있는 프로그램을 만들기 어렵다.

I/O 컴플리션 포트 메커니즘을 이용하면 이러한 문제를 해결할 수 있다.


사용자가 임의로 APC 큐에 항목을 추가할 수 있는 함수
~~~C++
DWORD QueueUserAPC(
  PAPCFUNC pfnAPC,
  HANDLE hThread,
  ULONG_PTR dwData
);
~~~

* 첫 번째 매개변수로 APC 콜백함수를 가리키는 포인터를 전달
* hThread는 시스템 내의 어떤 스레드도 가능하다.
* dwData : 콜백함수로 전달할 값

APC 콜백 함수의 원형
~~~C++
VOID WINAPI APCFunc(ULONG_PTR dwParam);
~~~

QueueUserAPC는 스레드를 대기 상태에서 강제로 빠져나오게 할 때도 사용가능하다.
* 만약 WaitForSingleObject를 호출하여 커널 오브젝트가 시그널될 때까지 기다리는 상태라면
* QueueUserAPC를 사용하면 이에 대한 답을 찾을 수 있다.

스레드를 대기 상태로부터 빠져나오게 하는 코드
~~~C++
// APC 콜백 함수는 아무런 작업도 하지 않는다.
VOID WINAPI APCFunc(ULONG_PTR dwParam) {

}

UINT WINAPI ThreadFunc(PVOID pvParam)
{ 
  HANDLE hEvent = (HANDLE) pvParam; // 오브젝트 핸들 값 전달

  // 스레드가 대기 상태에서 벗어날 수 있도록 얼러터블 상태를 유지한다.
  DWORD dw = WaitForSingleObjectEx(hEvent, INFINITE, TRUE);
  if (dw == WAIT_OBJECT_0)
  {
    // 오브젝트 시그널
  }

  if (dw == WAIT_IO_COMPLETION)
  {
    // QueueUseAPC가 사용되어 스레드가 대기 상태에서 빠져나온다.
    return 0; // 스레드 종료
  }

  return 0;
}

void main()
{
  HANDLE hEvent = CreateEvent(...);
  HANDLE hThread = (HANDLE) _beginthreadex(...);

  QueueUserAPC(APCFunc, hThread, NULL);
  WaitForSingleObject(hThread, INFINITE);
  CloseHandle(hThread);
  CloseHandle(hEvent);
}
~~~
* 썩 훌륭한 방법은 아니다.
* APC 큐에 항목을 추가하고 두 번째 스레드가 항목을 비우면 WAIT_IO_COMPLETION 값을 반환하고 스레드를 종료시킬 수 있다.


### I/O 컴플리션 포트
윈도우는 수천 명의 사용자에게 서비스를 제공하는 프로그램을 수행할 수 있는 안전하고 견고한 운영체제로 설계

전통적으로 서비스 어플리케이션은 다음 두 가지 아키텍처 중 하나의 형태로 설계
* 시리얼 모델 (serial model) : 하나의 스레드가 사용자의 요청 대기, 사용자의 요청이 들어오면 대기하던 스레드가 깨어나 클라 요청을 처리
* 컨커런트 모델 (concurrent model) : 하나의 스레드가 사용자 요청 대기, 사용자의 요청을 처리하기 위해 새로운 스레드 생성, 이 스레드가 요청을 처리하는 동안, 원래의 스레드는 다른 사용자 요청 대기

시리얼 모델 단점
* 다수의 동시 사용자 요청을 효과적 처리 불가
* 멀티프로세서 머신의 성능 활용 불가

컨커런트 모델
* 각각의 요청을 처리하기 위해 새로운 스레드 생성
* 요청을 기다리는 스레드는 비교적 최소한의 작업만 수행하면 된다. - 대부분 대기 시간으로 보내게 됨
* 확장성도 용이하고, 멀티프로세서 머신의 장점도 활용 가능하다.

그런데 윈도우 개발팀은 컨커런트 모델을 사용하는 프로그램이 윈도우에서 구현했을 때 기대한 만큼 성능이 나오지 않는 것을 파악
* 너무 많은 컨텍스트 전환을 수행 - 윈도우 커널 시간 낭비
* 각각의 스레드가 충분한 CPU 시간을 받지 못함
* 그 결과로 탄생한 것이 I/O 컴플리션 포트 커널 오브젝트

### I/O 컴플리션 포트 생성
I/O 컴플리션 포트를 구현한 이론적 배경은 동시 수행할 수 있는 스레드 개수의 상한을 설정할 수 있어야 한다는 것.
* 500명? 500개 스레드 안된다.
* 수행 가능한 스레드가 CPU의 스레드 수보다 많으면 시스템은 스레드 컨텍스트 전환을 위해 더 많은 CPU 시간을 필요하게 된다.
* CPU 자원 낭비 -> 컨커런트 모델의 약점

컨커런트 모델의 또 다른 약점
* 클라이언트의 요청이 있을 때마다 새로운 스레드를 생성해야 한다.
* 상당한 비용이 든다.
* 스레드 풀을 생성하여 종료 시까지 유지한다면 성능을 상당부분 개선할 수 있다.

실제로 I/O 컴플리션 포트는 커널 오브젝트 중 가장 복잡한 오브젝트이다.
* 스레드 풀을 이용하도록 설계되었다.

I/O 컴플리션 포트 생성 함수
~~~C++
HANDLE CreateIoCompletionPort(
  HANDLE hFile,
  HANDLE hExistingCompletionPort,
  ULONG_PTR CompletionKey,
  DWORD dwNumberOfConcurrentThreads
);
~~~

이 함수는 두 가지 역할을 한다.
* 컴플리션 포트 생성
* 장치와 컴플리션 포트를 연계

따라서 필자는 2개의 함수를 분리하여 구현했다고 한다.
~~~C++
HANDLE CreateNewCompletionPort(DWORD dwNumberOfConcurrentThreads) {
  return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);
}
~~~
* 생성하는 역할만
* 앞쪽의 3개의 매개변수는 장치와 컴플리션 포트를 연계하는 역할
* dwNumberOfConcurrentThreads 매개변수는 동일 시간에 동시에 수행할 수 있는 최대 개수를 알려주는 역할을 한다.
* 0을 전달 시 머신에 설치된 CPU 개수만큼
* 유일하게 SECURITY_ATTRIBUTES 구조체의 포인터를 받지 않는 커널 오브젝트 생성 함수
* 단일 프로세스 내에서만 수행될 수 있도록 하기 위함

### 장치와 컴플리션 포트의 연계
I/O 컴플리션 포트를 생성하면 윈도우 커널은 내부적으로 5개의 서로 다른 데이터 구조를 생성한다.
* 장치 리스트
  * 각 레코드의 구성
    * hDevice
    * dwCompletionKey
  * 항목 추가 시점
    * CreateIoCompletionPort가 호출되었을 때
  * 항목 제거 시점
    * 장치 핸들이 닫혔을 때
* I/O 컴플리션 큐
  * 각 레코드의 구성
    * dwBytesTransferred
    * dwCompletionKey
    * pOverlapped
    * dwError
  * 항목 추가 시점
    * I/O 요청이 완료되었을 때
    * PostQueuedCompletionStatus가 호출되었을 때
  * 항목 제거 시점
    * I/O 컴플리션 포트가 대기 스레드 큐의 항목을 가져올 때
* 대기 스레드 큐
  * 각 레코드의 구성
    * dwThreadId
  * 항목 추가 시점
    * 스레드가 GetQueuedCompletionStatus를 호출하였을 때
  * 항목 제거 시점
    * I/O 컴플리션 큐가 비어 있지 않고 수행 중인 스레드의 개수가 동시 수행 가능한 스레드 수를 초과하지 않을 경우
    * I/O 컴플리션 큐로부터 항목이 제거되고, 동시에 dwThreadId는 릴리즈 스레드 리스트로 이동한다. GetQueuedCompletionStatus가 호출되면 다시 돌아온다.
* 릴리즈 스레드 리스트
  * 각 레코드의 구성
    * dwThreadId
  * 항목 추가 시점
    * I/O 컴플리션 포트가 대기 스레드 큐에 있는 스레드를 깨우는 경우
    * 일시 정지되었던 스레드가 다시 깨어났을 경우
  * 항목 제거 시점
    * 스레드가 다시 GetQueuedCompletionStatus를 호출하였을 때(dwThreadId값은 대기 스레드 큐로 돌아간다.)
    * 스레드가 정지되는 함수를 호출하였을 때(dwThreadId는 일시 정지 스레드로 이동한다.)
* 일시 정지 스레드 리스트
  * 각 레코드의 구성
    * dwThreadId
  * 항목 추가 시점
    * 수행 중이던 스레드가 스레드를 정지시키는 함수를 호출하였을 때
  * 항목 제거 시점
    * 일시 정지되었던 스레드가 깨어났을 경우(dwThreadId는 릴리즈 스레드 리스트로 이동한다)


장치 리스트
* I/O 컴플리션 포트와 연계된 장치를 관리하기 위한 리스트
* CreateIoCompletionPort를 사용하여 연계

연계 전용 CreateIoCompletionPort
~~~C++
BOOL AssociateDeviceWithCompletionPort(
  HANDLE hCompletionPort, HANDLE hDevice, DWORD dwCompletionKey)
  {
    HANDLE h = CreateIoCompletionPort(hDevice, hCompletionPort, dwCompletionKey, 0);

    return (h == hCompletionPort);
  }
~~~
* AssociateDeviceWithCompletionPort는 장치 리스트에 새로운 항목을 추가한다.
* 생성한 컴플리션 포트 핸들, 장치 핸들, 컴플리션 키를 전달하면 된다.


I/O 컴플리션 큐
* 비동기 요청이 완료되면 시스템은 장치와 연계된 컴플리션 포트가 있는지 확인
* 연계된 컴플리션 포트가 있으면 I/O 컴플리션 큐에 완료 통지를 삽입한다.
* 컴플리션 포트를 연계할 때 지정한 컴플리션 키 값, OVERLAPPED 구조체 포인터, 에러 코드를 가지고 있다.


완료 통지를 받지 않도록 비동기 I/O 작업을 요청하는 방법
~~~C++
Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
Overlapped.hEvent = (HANDLE) ((DWORD_PTR)Overlapped.hEvent | 1);
ReadFile(... , &Overlapped);
~~~
* Event 핸들에 1을 비트 OR 연산해서 넘기면 된다.


### I/O 컴플리션 포트를 이용한 아키텍처 설계
스레드를 몇개로 유지할까?
* 자세한 분석은 다음 장에서
* 보통의 경우 머신의 CPU 개수에 2를 곱한 수준으로 생성

풀 내의 모든 스레드들은 동일한 스레드 함수를 수행하도록 구성하는 것이 좋다.
* 초기화 작업을 거친 후 루프 진입
* 프로그램이 종료될 때 루프를 탈출

루프 내에서 하는 일
* 비동기 장치 I/O 작업이 완료되어 I/O 컴플리션 포트를 통해 완료 통지가 전달될 때 이를 곧바로 처리할 수 있도록 스레드를 대기상태로 유지시켜야 함
* GetQueuedCompletionStatus 함수를 사용하면 된다.

~~~C++
BOOL GetQueuedCompletionStatus(
  HANDLE        hCompletionPort,
  PDWORD        pdwNumberOfBytesTransferred,
  PULONG_PTR    pCompletionKey,
  OVERLAPPED    **ppOverlapped,
  DWORD         dwMilliseconds
);
~~~
* hCompletionPort : 어떤 컴플리션 포트를 대기할 것인지
  * 대부분의 서비스 프로그램에서 단 하나의 I/O 컴플리션 포트만을 사용
* 이 함수를 호출한 스레드를 컴플리션 큐에 새로운 항목이 삽입될 때까지 대기 상태 유지
  * dwMilliseconds : 적절한 타임아웃 값을 지정할 수 있다.


대기 스레드 큐(Waiting Thread Queue)
* 스레드 풀 내의 여러 개의 스레드들이 각기 GetQueuedCompletionStatus 함수를 호출하면 이 함수를 호출한 스레드의 ID 값이 대기 스레드 큐에 삽입
* 이를 통해 어떤 스레드들이 비동기 I/O 요청에 대한 완료 통지를 처리할 것인지 알 수 있다.
* I/O 컴플리션 큐에 항목이 추가되면 I/O 컴플리션 포트는 대기 스레드 큐에 있는 스레드 중 하나를 깨우게된다.
* 이 스레드는 컴플리션 큐로부터 송수신 바이트 수, 컴플리션 키, OVERLAPPED 구조체의 주소를 가져오게 된다.
* GetQueuedCompletionStatus 함수의 인자로 전달된다.

GetQueuedCompletionStatus 함수의 반환 원인 파악하기
~~~C++
DWORD dwNumBytes;
ULONG_PTR CompletionKey;
OVERLAPPED *pOverlapped;

// hIOCP는 프로그램의 다른 부분에서 이미 초기화
BOOL bOk = GetQueuedCompletionStatus(hIOCP, &dwNumBytes, &CompletionKey, &pOverlapped, 1000);
DWORD dwError = GetLastError();

if (bOk)
{
  // 성공적으로 수행된 I/O 완료 통지에 대한 처리
}
else
{
  if (pOverlapped != NULL)
  {
    // 실패한 I/O 완료 통지에 대한 처리
    // dwError 변수는 실패의 이유를 담고 있다.

  }
  else
  {
    if (dwError == WAIT_TIMEOUT)
    {
      // I/O 컴플리션 큐 대기 중에 대기 시간 만료가 발생
    }
    else
    {
      // GetQueuedCompletionStatus를 잘못 호출하였다.
      // dwError는 잘못된 호출의 이유를 나타내는 값을 담고 있다.
    }
  }
}
~~~
* I/O 컴플리션 큐는 선입선출 방식으로 항목을 삽입하고 제거
* GetQueuedCompletionStatus를 호출하는 스레드는 후입선출 방식으로 깨어남
* 성능 향상을 위한 동작 방식

완료 통지가 매우 느리게 도달하게 되면 단일의 스레드가 모든 완료 통지를 처리할 수도 있을 것이다.
* 다른 스레드들은 대기 상태를 유지하게 둔다.
* 후입선출 알고리즘을 사용하면 스케줄되지 않는 메모리를 swap out할 수 있으며
* 프로세서의 캐시를 비울 수 있다.
* 여러 스레드가 I/O 컴플리션 포트를 대기한다 하더라도 나쁘지 않다.
* 대기하는 스레드의 메모리를 swap out할 수 있기 때문

상당량의 I/O 요청이 지속적으로 수행될 것으로 예측되어 I/O 컴플리션 포트를 대기하는 스레드의 개수를 증가시키게 되면 컨텍스트 스위칭 비용이 함께 증가하는 문제가 발생한다.
* GetQueuedCompletionStatusEx와 같은 함수를 호출하여 완료 통지를 한꺼번에 가져올 수 있다.

~~~C++
BOOL GetQueuedCompletionStatusEx(
  HANDLE hCompletionPort,
  LPOVERLAPPED_ENTRY pCompletionPortEntries,
  ULONG ulCount,
  PULONG pulNumEntriesRemoved,
  DWORD dwMilliseconds,
  BOOL bAlertable
);
~~~
* pCompletionPortEntries : 각 항목들은 해당 배열을 통해 전달된다.
* ulCount : 몇개의 항목을 가져올 것인지
* pulNumEntriesRemoved : 실제로 몇개 가져왔는지
* bAlertable : TRUE로 설정되면 I/O 컴플리션 큐에 어떠한 완료 통지도 존재하지 않으면 얼러터블 상태로 전환

LPOVERLAPPED_ENTRY의 각 요소들
~~~C++
typedef struct _OVERLAPPED_ENTRY
{
  ULONG_PTR lpCompletionKey; // I/O 요청의 결과 코드
  LPOVERLAPPED lpOverlapped; // 구조체의 주소
  ULONG_PTR Internal;
  DWORD dwNumberOfBytesTransferred; // 송수신된 바이트 수
} OVERLAPPED_ENTRY, *LPOVERLAPPED_ENTRY;
~~~
* Internal 필드는 공개되지 않았으며 사용하면 안된다.


### I/O 컴플리션 포트의 스레드 풀 관리 방법
I/O 컴플리션 포트를 생성할 때 동시에 수행 가능한 스레드의 개수를 지정할 수 있다.
* 일반적으로 머신의 CPU 개수와 동일하게 설정

완료 통지가 삽입되면 대기중인 스레드를 깨우는데, 설정한 개수 이상을 깨울 수 없다.

그럼 왜 이보다 더 많은 스레드를 스레드 풀로 관리해야 하는 것일까?
* 만약 머신의 CPU 수가 2개인 경우 2개의 스레드를 컴플리션 포트를 사용하는 경우 
* 4개의 스레드를 생성한다고해도 나머지 2개의 스레드는 어떠한 일도 하지 않을 것처럼 보인다.

I/O 컴플리션 포트는 상당히 지능적으로 동작한다.
* 특정 스레드의 수행을 재개시키는 경우 릴리즈 스레드 리스트에 깨어난 스레드의 ID를 기록
* I/O 컴플리션 포트는 어떤 스레드가 깨어났는지 알 수 있으며 수행 상황을 지속적으로 확인할 수 있다.
* 릴리즈 스레드 리스트의 스레드가 대기상태로 진입 하였다면 스레드 ID 값을 빼내어 일시 정지 스레드 리스트로 옮긴다.


I/O 컴플리션 포트는 자신을 생성할 때 지정한 스레드 개수만큼 릴리즈 스레드 리스트의 항목 수를 유지하려 한다.
* 릴리즈 스레드 리스트에 있던 스레드가 대기 상태로 전환되면 대기 상태에 있는 스레드 중 하나를 릴리즈 리스트로 옮겨온다.
* 또한 대기 상태로 전환되어 일시 정지 스레드 리스트에 있던 스레드가 다시 수행을 재개하는 경우에도 일시 정지 스레드 리스트로부터 릴리즈 스레드 리스트로 그 항목을 옮겨오게 된다.
* 이 상황에는 릴리즈 스레드 리스트는 컴플리션 포트에 설정한 수를 일시적으로 초과하는 항목을 가지게 된다.

어떤 스레드가 GetQueuedCompletionStatus를 호출하면 : I/O 컴플리션 포트에 할당된 스레드라고 부른다.

I/O 컴플리션 포트에 할당된 스레드가 할당 해제되는 경우
* 스레드가 종료되는 경우
* 다른 I/O 컴플리션 포트의 핸들을 인자로 GetQueuedCompletionStatus를 호출한 경우
* 스레드가 할당된 I/O 컴플리션 포트가 종료되는 경우

2개의 CPU를 가진 머신에서의 동작 방식
* I/O 컴플리션 포트 스레드 수 2개 설정
* 4개의 스레드를 생성하여 I/O 컴플리션 포트로 전달되는 완료 통지를 대기

만약 3개의 완료 통지가 컴플리션 큐에 삽입되면
* 2개의 스레드만이 깨어나서 삽입된 완료통지를 처리
* 하나의 스레드가 Sleep, WaitForSingleObject 등의 함수나 동기 I/O 함수를 호출하거나 스레드를 수행 상태로 유지할 수 없는 함수를 호출하게 되면
* I/O 컴플리션 포트는 스레드가 수행 상태가 아님을 발견하는 즉시 세 번째 스레드를 깨운다.
* 즉, I/O 컴플리션 포트의 사용 목적은 CPU가 계속해서 작업을 수행하도록 상태를 유지하는 것

대기 상태로 전환되었던 스레드가 다시 수행을 재개하면 동시에 수행되는 스레드의 개수가 일시적으로 시스템이 가지고 있는 CPU의 개수를 초과하게 된다.
* 이때 I/O 컴플리션 포트는 동시에 수행 중인 스레드의 개수가 CPU 개수 이하로 떨어질 때까지 새로운 스레드를 깨우지 않는다.
* I/O 컴플리션 포트 아키텍처는 실제 수행 중인 스레드의 개수가 동시에 수행 가능한 스레드의 개수를 초과한 상황을 가능한 짧게 가져가기 위해 스레드가 GetQueuedCompletionStatus를 재호출하는 즉시 스레드를 대기 상태로 전환한다.

즉, 이러한 동작 방식을 고려하면 I/O 컴플리션 포트 내의 스레드 풀에 존재하는 스레드의 개수는 동시에 수행 가능한 스레드의 개수보다 큰 값으로 유지하는 것이 좋다.


### 스레드 풀에 몇 개의 스레드를 유지할 것인가?
2가지를 고혀해야 한다.
* 서비스 프로그램이 초기화되는 시점에는 가능한 적은 수의 스레드만을 생성하기를 원할 것이다.
  * 스레드를 생성하고 파괴하는 것은 CPU 시간을 소비하는 작업이라는 사실을 잊으면 안된다. - 최소화 해야 한다.
* 너무 많은 스레드를 생성하면 시스템 자원이 낭비될 수 있으므로 가능한 스레드의 게수를 제한하고 싶을 것이다.
  * 스레드가 사용하는 대부분의 리소스들은 램으로부터 페이지 파일로 스왑 아웃 될 것이기 때문에
  * 페이지 파일이 존재하는 디스크 공간 정도만이 낭비되겠지만 그렇다 하더라도 적절히 관리하고 싶을 것이다.

대부분의 서비스도 실험 결과를 근간으로 스레드 풀을 관리하기 때문에 독자들도 실험을 수행하길 바란다.

~~~C++
LONG g_nThreadsMin;     // 풀 내의 최소 스레드 개수
LONG g_nThreadsMax;     // 풀 내의 최대 스레드 개수
LONG g_nThreadsCrnt;    // 풀 내의 현재 스레드 개수
LONG g_nThreadsBusy;    // 풀 내의 스레드 중 수행 중인 스레드의 개수

// 실험을 위해 애플리케이션 초기화 시에는 g_nThreadsMin 개수만큼의 스레드만을 생성하고,
// 모든 스레드들이 동일한 스레드 풀 함수를 사용하도록 구성하였다.
// 스레드 풀 함수에서 어떤 작업을 수행해야 하는지에 대한 예
DWORD WINAPI ThreadPoolFunc(PVOID pv)
{
  // 스레드가 풀 내로 진입하였다.
  InterlockedIncrement(&g_nThreadsCrnt);
  InterlockedIncrement(&g_nTHreadsBusy);

  for (BOOL bStayInPool = TRUE; bStayInPool;)
  {
    // 스레드가 수행을 멈추고 작업을 수행하기 위해 대기하고 있다.
    InterlockedDecrement(&m_nThreadsBusy);
    BOOL bOk = GetQueuedCompletionStatus(...);
    DWORD dwIOError = GetLastError();

    // 스레드가 작업을 수행 중에 있다.
    int nThreadsBusy = InterlockedIncrement(&m_nThreadsBusy);

    // 추가적인 스레드를 풀에 삽입해야 하는가?
    if (nThreadsBusy == m_nThreadsCrnt) // 모든 스레드가 수행 중이며
    {
      if (nThreadsBusy < m_nThreadsMax) // 풀이 꽉 차지 않았으며
      {
        if (GetCPUUsage() < 75) // CPU 사용률이 75% 미만이라면
        {
          // 새로운 스레드를 풀에 추가한다.
          CloseHandle(chBEGINTHREADEX(...));
        }
      }
    }
    if (!bOk && (dwIOError == WAIT_TIMEOUT)) // 대기 중이던 스레드가 타임 아웃을 유발하였다.
    {
      // 서버가 수행하는 작업에 비해 필요 이상의 스레드가 존재하므로
      // 비약적으로 I/O 요청이 늘어나지 않는 한 이 스레드는 종료해도 된다.
      bStayInPool = FALSE;
    }

    if (bOk || (po != NULL))
    {
      // 스레드가 작업 수행을 마쳤다.
      ...
      if (GetCPUUsage() > 90) // CPU 사용률이 90%를 초과하고
      {
        if (g_nThreadsCrnt > g_nThreadsMin) //  현재 스레드가 풀의 최소 스레드 개수보다 많으면
        {
          bStayInPool = FALSE; // 스레드를 풀로부터 제거한다.
        }
      }
    }
  }

  // 스레드가 풀로부터 제거된다.
  InterlockedDecrement(&g_nThreadsBusy);
  InterlockedDecrement(&g_nThreadsCurrent);
  return 0;
}
~~~
* 이 슈도코드는 I/O 컴플리션 포트를 얼마나 독창적으로 사용할 수 있는지를 보여주는 예라고 할 수 있다.
* GetCPUUsage 함수는 윈도우 API가 아니기 때문에 직업 구현해야 한다.
* 스레드 풀에는 적어도 하나 이상의 스레드가 존재하도록 하여 클라이언트가 정상적으로 수행될 수 있도록 해 주어야 한다.
* 슈도코드는 단지 예시일 뿐이므로 서비스의 구조에 맞추어 좀 더 적절히 수정되어야 할 것이다.

대부분의 서비스들은 스레드 풀의 동작 방식을 제어할 수 있도록 관리 도구들을 제공해 준다.
* 예를들어 최소, 최대 스레드 개수나 CPU 사용률 임계값, I/O 컴플리션 포트를 생성할 때의 동시 수행 가능 스레드의 개수 등을 제어

### I/O 완료 통지 흉내 내기
I/O 컴플리션 포트는 장치 I/O를 수행하는 경우에만 활용될 수 있는 기술이다.
* 스레드간 통신을 수행할 때에도 동일하게 활용될 수 있으며,
* 특히 I/O 컴플리션 포트 커널 오브젝트는 스레드간 통신을 수행할 때 상당히 유용하게 사용될 수 있다.

얼러터블 I/O에서 QueueUserAPC 함수를 사용하면 다른 스레드로 APC 항목을 삽입할 수 있다.
* 이와 비슷하게 I/O 컴플리션 포트의 경우에도 PostQueuedCompletionStatus 함수가 존재한다.

~~~C++
BOOL PostQueuedCompletionStatus(
  HANDLE hCompletionPort,
  DWORD dwNumBytes,
  ULONG_PTR CompletionKey,
  OVERLAPPED *pOverlapped
);
~~~
* hCompletionPort : 완료 통지를 삽입할 컴플리션 포트의 핸들
* 나머지 값들은 GetQueuedCompletionStatus를 호출한 스레드에게 전달할 값
* PostQueuedCompletionStatus를 호출하여 삽입한 항목을 가져오는 경우에도
* GetQueuedCompletionStatus는 I/O 요청이 성공적으로 완료된 것처럼 TRUE 값을 반환한다.

위 함수는 스레드 풀에 존재하는 스레드들과 통신을 수행해야 하는 경우 유용하게 사용될 수 있다.
* 예를 들어 종료 요청

이러한 스레드 종료 기법을 사용하고자 할 때는 상당한 주의가 필요하다.
* 만약 하나의 스레드가 종료 중에 GetQueuedCompletionStatus를 재호출하게 되면
* 이 스레드가 여러 번의 완료 통지를 수신할 수도 있다.
* 이 경우 프로그램에 부가적인 스레드 동기화 방법을 추가해야 한다.

윈도우 비스타에서는 I/O 컴플리션 포트의 핸들을 인자로 CloseHandle을 호출하게 되면 GetQueuedCompletionStatus를 호출한 모든 스레드가 깨어나게 되고 FALSE 값을 반환하게 된다.
* 이 경우 GetLastError를 호출해보면 ERROR_INVALID_HANDLE 값을 얻게 되는데
* 스레드는 이 값이 반환되었을 때 지금이 종료해야 하는 시점임을 알 수 있다.