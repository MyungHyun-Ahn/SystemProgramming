# 7장 프로세스간 통신(IPC)
## 들어가기 전 18장 몇몇 함수
* Windows의 파일 관리 함수는 ANSI 표준 입출력 함수와 비교해서 파악

**파일 열고, 닫기**
CreateFile 함수


HANDLE CreateFile (\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpFileName,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwDesiredAccess,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwSharedMode,\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpSecurityAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwCreationDisposition,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwFlagsAndAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hTemplateFile\
)

* 실패 시 INVALID_HANDLE_VALUE return

1. lpFileName : Open할 파일 이름
2. dwDesiredAccess : 읽기 쓰기 모드 지정
   * GENERIC_READ : 읽기 모드 지정
   * GENERIC_WRITE : 쓰기 모드 지정
3. dwSharedMode : 파일 공유 방식을 지정한다.
   * 0 : 다른 프로세스에 절대 공유 불가
   * FILE_SHARED_READ : 다른 프로세스에서 이 파일 동시 읽기 접근 가능
   * FILE_SHARED_WRITE : 다른 프로세스에서 이 파일 동시 쓰기 접근 가능
4. lpSecurityAttributes : 보안 속성 지정, 디폴트 NULL
5. dwCreationDisposition : 파일 생성 방법 지정
   * CREATE_ALWAYS : 항상 새 파일
   * CREATE_NEW : 새 파일 생성, 파일 이름 중복 시 실패
   * OPEN_ALWAYS : 기존 파일 열기, 없으면 생성
   * OPEN_EXISTING : 파일이 있다면 열기, 없으면 실패
   * TRUNCATE_EXISTING : 기존 파일의 내용 지우고 열기, 없으면 실패 
6. dwFlagsAndAttributes : 파일 특성 정보 설정, 기본적으로 FILE_ATTRIBUTE_NORMAL
7. hTemplateFile : 기존 파일 템플릿, 일반적으로 NULL

파일 닫기는 CloseHandle

**파일 읽기, 쓰기**
ReadFile 함수

BOOL ReadFile (\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hFile,\
&nbsp;&nbsp;&nbsp;&nbsp;LPVOID lpBuffer,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nNumberOfBytesToRead,\
&nbsp;&nbsp;&nbsp;&nbsp;LPDWORD lpNumberOfBytesRead,\
&nbsp;&nbsp;&nbsp;&nbsp;LPOVERLAPPED lpOverrapped\
)

* 실패 시 리턴 0

1. hFile : 읽을 파일 핸들
2. lpBuffer : 데이터를 저장할 버퍼의 주소
3. nNumberOfBytesToRead : 파일에 저장하고자 하는 데이터의 크기 (바이트 단위)
4. lpNumberOfBytesRead : 실제로 읽어들인 데이터 크기를 저장할 변수 주소
5. lpOverrapped : 추후 설명

WriteFile 함수

BOOL WriteFile (\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hFile,\
&nbsp;&nbsp;&nbsp;&nbsp;LPVOID lpBuffer,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nNumberOfBytesToWrite,\
&nbsp;&nbsp;&nbsp;&nbsp;LPDWORD lpNumberOfBytesWritten,\
&nbsp;&nbsp;&nbsp;&nbsp;LPOVERLAPPED lpOverrapped\
)

* 실패 시 리턴 0

1. hFile : 쓸 파일 핸들
2. lpBuffer : 데이터를 저장하고 있는 버퍼의 주소
3. nNumberOfBytesToRead : 파일에 저장하고자 하는 데이터의 크기 (바이트 단위)
4. lpNumberOfBytesRead : 실제로 저장된 데이터 크기를 저장할 변수 주소
5. lpOverrapped : 추후 설명

## Section 01 - 프로세스간 통신(IPC)의 의미
### 프로세스 사이에서 통신이 이뤄지기 위한 조건
* IPC : 통신수단 (전화나 메신저 같은)
* 통신하고자 하는 프로세스가 서로 만날 수 있는 조건이 되면 통신이 쉬워진다.
* 예를들어 공유하는 메모리 영역이 존재
* 조건이 없다면 보조 수단이 필요하다.

### 프로세스들이 서로 만날 수 없는 이유
**프로세스들은 서로 만나 데이터를 주고 받는 것이 불가능**
* 프로세스마다 서로 다른 메모리 구조를 가지고 있기 때문
* 즉, 프로세스는 자신에게 할당된 메모리 공간 이외에는 접근이 불가능하다.

### 프로세스들이 서로 만나지 못하게 디자인한 이유
**안정성을 높이기 위함**
* 서로 다른 프로세스가 메모리를 공유하면 원치 않는 결과가 나타날 수도 있음
* 저장하지 않은 파일이 수정되는 이슈

## Section 02 - 메일슬롯 방식의 IPC
### 메일슬롯의 원리
메일슬롯은 편지를 넣을 수 있는 가느다란 우체통의 입구를 의미
* 선형적인 메모리 구조의 특성을 반영하기 위해 메일슬롯이라고 이름을 붙인 것 같다.

메일슬롯의 기본적인 원리는 다음과 같다.
* 데이터를 주고 받기 위해 프로세스가 우체통을 마련하는 것

데이터를 전달하는 프로세스 : Sender\
데이터를 받는 프로세스 : Receiver 
1. Reciver는 메일슬롯을 준비한다.
2. Sender는 Reciver에 메일슬롯에 데이터를 보낸다.
3. Reciver는 메일슬롯에 도착한 데이터를 확인한다.

### 메일슬롯 구성을 위해 필요한 것
**Receiver가 준비해야할 것**

메일슬롯을 생성한다.

HANDLE CreateMailslot (\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpName,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nMaxMessageSize,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD lReadTimeout,\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpSecurityAttributes\
)

* 실패 시 INVALID_HANDLE_VALUE 반환
* 성공하면 메일슬롯의 핸들 반환

1. lpName : 생성할 메일슬롯의 이름을 결정한다.
    * 기본형식 : \\\\computername\\mailslot\\[path]name
2. nMaxMessageSize : 메일슬롯의 버퍼 크기를 지정한다. 0이라면 시스템이 허용하는 최대 크기
3. lReadTimeout : 최대 Blocking 시간을 결정한다.
4. lpSecurityAttributes : 핸들을 상속하는데 사용하기도 함

**Sender가 준비해야할 것**
1. Receiver의 메일슬롯을 연다. CreateFile
2. 데이터(메일)을 쓴다. WriteFile

메일슬롯의 통신에 파일입출력 함수를 사용하는 이유
* 메일슬롯은 Windows의 파일 시스템을 기반으로 구현되어 있음

메일슬롯의 주소 규칙
* 기본형식 : \\\\computername\\mailslot\\[path]name
* computername에는 .이 들어간다. (로컬 컴퓨터를 의미)
* [path]name은 실질적인 메일슬롯의 이름이다.
* 계층적인 구조로도 표현 가능하다.
* \\\\.\\mailslot\\abc\\cdf\\mailbox

### 메일슬롯 고찰과 앞으로의 전개
* 메일슬롯은 한쪽으로만 메시지를 전달할 수 있다.
* 메시지를 주고 받으려면 두 개의 메일슬롯을 생성
* 양방향으로 사용가능한 IPC 기법은 파이프
* Anonymous 파이프와 Named 파이프
* Named는 기본적으로 양방향을 지원
* 메일슬롯은 브로드 캐스팅 방식의 통신을 지원
* 같은 이름의 메일슬롯을 생성하면 Sender가 보냈을 때 모두에게 보내진다.

## Section 03 Signaled vs Non-Signaled
커널 오브젝트의 상태에 관한 주제이다.
### 커널 오브젝트의 두 가지 상태(State)
* Windows 운영체제에서 생성되는 커널 오브젝트는 두 가지 상태를 지닌다.
* 리소스에 특정 상황이 발생했음을 의미한다.

### 상태에 대한 이해
상태라는 용어를 사용하는 이유는 변하기 때문
* ex) 자동차가 엑셀을 밟으면 주행 상태

커널 오브젝트는 두 가지 상태를 가진다.
1. Signaled 상태 : 신호를 받은 상태
2. Non-Signaled 상태 : 신호를 받지 않은 상태

커널 오브젝트의 상태 정보는 어떻게 표현?
* 커널 오브젝트를 구성하는 멤버 변수 한 개가 표현
* TRUE(Signaled) 혹은 FALSE(Non-Signaled)

### 프로세스 커널 오브젝트의 상태에 대한 이해
프로세스 커널 오브젝트는 프로세스가 생성될 때 생성
1. 처음 생성되면 Non-Signaled 상태로 설정된다.
2. 프로세스가 종료되면 Signaled 상태로 변한다.

이 상태를 통해 프로세스가 종료되었음을 알 수 있다.

반대로 실행이 재개되면 Non-Signaled 상태로 변한다.\
그런데 종료된 프로세스는 다시 재게될 수 없으므로 Signaled 상태에서 변하지 않는다.

### 커널 오브젝트의 두 가지 상태를 확인하는 용도의 함수
커널 오브젝트의 상태를 확인하는 대표적인 함수\
WaitForSingleObject

DWORD WaitForSingleObject (\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwMilliseconds\
);

* 실패 시 WAIT_FAILED 반환

1. hHandle : 상태 확인을 원하는 커널 오브젝트 핸들
2. dwMilliseconds : Signaled 상태가 될 때까지 기다릴 수 있는 최대 시간

WaitForSingleObject 함수가 반환되는 상황
1. WAIT_OBJECT_0 : Signaled 상태가 되었을 때 반환
2. WAIT_TIMEOUT : dwMilliseconds에 설정한 시간이 다 된 경우 반환
3. WAIT_ABANDONED : 소유 관계와 관련하여 함수가 정상적이지 못한 오류 발생에 의해 반환되는 경우 반환 - 13장에서 자세히 설명

WaitForMultipleObjects(\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nCount,\
&nbsp;&nbsp;&nbsp;&nbsp;const HANDLE *lpHandles,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bWaitAll,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwMilliseconds\
)

* 실패 시 WAIT_FAILED 반환

1. nCount : 배열에 저장된 핸들의 수
2. lpHandles : 핸들을 저장한 배열의 주소
3. bWaitAll : 관찰 대상이 모두 Signaled일 때 반환할지 혹은 한 개라도 Signaled 상태일 때 반환할지 여부
4. dwMilliseconds : 타임아웃 시간

### 커널 오브젝트의 상태 확인이 필요한 상황의 연출
1. 부모 프로세스가 자식 프로세스 A에게 1~5까지 합 계산 요청
2. 부모 프로세스가 자식 프로세스 B에게 6~10까지 합 계산 요청
3. 부모 프로세스에서 두 결과를 합 연산

WaitForSingleObject를 사용하지 않을 경우
* 프로세스가 STILL_ACTIVE 상태코드를 반환하여 518이라는 원치않은 결과 확인
* 프로세스가 끝날 때까지 기다려야 한다.

## 이것만은 알고 갑시다.
1. 프로세스간 통신 기법이 별도로 존재하는 이유

2. 메일슬롯의 특성
3. 커널 오브젝트의 두 가지 상태가 지니는 의미
4. WaitForSingleObject, WaitForMultipleObjects 함수와 커널 오브젝트의 관계
5. 종료코드 (Exit Code)