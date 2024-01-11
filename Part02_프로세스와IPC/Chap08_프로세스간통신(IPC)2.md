# 8장 프로세스간 통신 (IPC) 2

## Section 01 - 핸들 테이블과 오브젝트 핸들의 상속
**"유능한 Windows 프로그래머는 프로세스 핸들 테이블이 어떻게 관리되는지를 이해하고 있어야 한다."**
* 실제로 Windows가 어떻게 핸들 테이블이 관리되는지는 공개되지 않았다.
* 가장 일반적인 방법으로 어떻게 프로세스 핸들 테이블이 관리하는지 알아보자.
* 실질적인 방법과는 차이가 있을수도 있다.

### 프로세스의 커널 오브젝트 핸들 테이블
* 프로세스 핸들은 숫자(int)이다.
* 그런데 어떻게 CloseHandle을 호출했을 때 커널 오브젝트의 주소를 찾아서 Usage Count를 1개 줄일까?

### 프로세스의 핸들 테이블 도입
핸들 주소\
256 0x2400

* 핸들 테이블은 핸들 정보를 저장하고 있는 테이블로서 프로세스 별로 독립적이다.

### 즉, 핸들은 프로세스 별로 독립적이다.

### 핸들의 상속
CreateProcess 함수를 호출하면 새로운 자식 프로세스가 생성된다.\
이 때 5번째 인자에 따라 부모 프로세스 핸들 테이블에 등록되어 있는 핸들 정보는 자식 프로세스 핸들에 상속될 수 있다.

**핸들의 상속에 대한 이해**

부모 프로세스 핸들 테이블  | 자식 프로세스 핸들 테이블\
핸들 주소 상속여부 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| 핸들 주소 상속여부\
127 0x1200 Y ~~~~~~~~~ 127 0x1200 Y\
168 0x1680 N\
256 0x2400 Y ~~~~~~~~~ 256 0x2400 Y


* 실제 핸들 테이블에는 핸들의 상속 여부를 결정짓기 위한 컬럼이 존재한다.
* 추가로 상속여부에 대한 정보도 변경 없이 그대로 상속되었다.
* 따라서 자식 프로세스가 또 다른 자식 프로세스를 생성할 경우에도 이 핸들에 대한 정보는 계속해서 상속된다.

**앞에서 소개되었던 CreateProcess 함수의 5번 째 인자**
* bInheritHandles : 자식 프로세스에게 핸들 테이블에 등록되어 있는 핸들 정보를 상속해 줄 것인지 결정하는 요소
* TRUE일 경우 부모 프로세스가 소유하고 있는 핸들 테이블의 상속 여부를 결정하는 것이다.
* 물론 상속 여부가 TRUE로 설정된 핸들만 상속된다.

### 핸들의 상속과 커널 오브젝트의 Usage Count
핸들이 자식 프로세스로 상속되면 Usage Count는 증가한다.

### 상속이 되기 위한 핸들의 조건
핸들의 상속 여부는 핸들이 생성될 때 결정된다.

CreateMailslot 함수를 예시로
* LPSECURITY_ATTRIBUTES 인자는 SECURITY_ATTRIBUTES 구조체의 포인터이다.
* SECURITY_ATTRIBUTES 구조체를 설정한 후 전달하면 상속이 가능하다.

SECURITY_ATTRIBUTES 구조체

typedef struct _SECURITY_ATTRIBUTES\
{\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nLength;\
&nbsp;&nbsp;&nbsp;&nbsp;LPVOID lpSecurityDescriptor;\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInheritHandle;\
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES;

1. nLength : 구조체 변수 크기를 바이트 단위로 전달
2. lpSecurityDescriptor : NULL, 핸들의 상속과는 관계없는 인자
3. bInheritHandle : TRUE, 핸들의 상속 여부를 설정하는 인자

부모 프로세스가 자식 프로세스에게 핸들을 상속하면\
핸들의 번호를 통해 해당 커널 오브젝트를 사용가능하다.

### Pseudo 핸들과 핸들의 중복(Duplicate)
현재 실행 중에 있는 프로세스 자신의 핸들을 얻는 방법으로써 GetCurrentProcess 함수를 사용
* 그런데 이렇게 얻은 함수는 가짜 핸들 (Pseudo Handle)이라고 한다.
* 그럼 진짜 핸들을 어떻게 얻을까?

BOOL DuplicateHandle (\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hSourceProcessHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hSourceHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hTargetProcessHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;LPHANLDE lpTargetHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwDesiredAccess,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInheritHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwOptions\
)

1. hSourceProcessHandle : 복제할 핸들을 소유하는 프로세스를 지정
2. hSourceHandle : 복제할 핸들을 지정
3. hTargetProcessHandle : 복제된 핸들을 소유할 프로세스를 지정
4. lpTargetHandle : 복제된 핸들값을 저장할 변수의 주소를 지정
5. dwDesiredAccess : 복제된 핸들의 접근권한 , 우리는 0
6. bInheritHandle : 복제된 핸들의 상속 여부
7. dwOptions : 옵션
   * DUPLICATE_SAME_ACCESS : 원본 핸들과 동일한 접근권한
   * DUPLICATE_CLOSE_SOURCE : 원본 핸들을 종료
   * | 연산자를 통해 동시 전달 가능

DuplicateHandle의 설명
1. 부모 프로세스가 있고 자식 프로세스가 있다.
2. 부모 프로세스가 자식 프로세스에게 자신의 핸들 테이블에 있는 핸들을 자식 프로세스에게 복사해주고 싶을 때
3. DuplicateHandle(부모, 핸들번호, 자식, &등록된 핸들 값, ...);
4. 함수를 호출하게 되면 자식 프로세스의 핸들 테이블에 등록한다.

핸들의 복사
1. DuplicateHandle(프로세스 핸들 A, 256, 프로세스 핸들 B, &val, ...); 
2. 핸들의 커널 오브젝트 주소는 동일한데, 다른 핸들 번호가 생성된다.
3. 당연히 Usage Count도 증가한다.
4. 복사한 핸들에도 CloseHandle 함수로 반환해주어야 한다.

## Section 02 - 파이프 방식의 IPC
### 메일슬롯에 대한 회고와 파이프의 이해
Windows 파이프 메커니즘 두 가지
1. 이름없는 파이프(Anonymous Pipe)
2. 이름있는 파이프(Named Pipe)

메일슬롯은 서로 관련 없는 프로세스들 사이에서 통신할 때 사용하는 IPC 기법이다.\
이름없는 파이프는 지극히 관계가 있는 (부모자식, 혹은 형제) 프로세스들 사이에서 통신하는 경우 유용하다.

이름있는 파이프는 서로 관련이 없어도 통신이 가능하다.\
브로드캐스트를 지원하지 않는 대신 양방향 통신이 가능하다.

### 이름없는 파이프(Anonymous Pipe)
물이 옥상에서 배수관을 타고 흐르듯 데이터를 한쪽방향으로 전송 가능

BOOL CreatePipe (\
&nbsp;&nbsp;&nbsp;&nbsp;PHANDLE hReadPipe,\
&nbsp;&nbsp;&nbsp;&nbsp;PHANDLE hWritePipe,\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpPipeAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nSize\
)

1. hReadPipe : 파이프의 한쪽 끝 읽기 파이프
2. hWritePipe : 반대편 끝 쓰기 파이프
3. lpPipeAttributes : 보안관련 옵션 상속 옵션 등에 사용
4. nSize : 파이프의 버퍼 사이즈


입력용 혹은 출력용 핸들을 자식 프로세스에게 상속시키면 부모 자식 프로세스 간 메시지 전송이 가능하다.


### 이름있는 파이프(Named Pipe)
1. 서버 측에서 CreateNamedPipe 함수 호출로 파이프 생성
2. ConnectNamedPipe 함수에 의해 연결 대기 상태로 전환
3. 클라이언트의 CreateFile 함수 호출에 의한 파이프 오픈(연결)

파이프 오브젝트 생성 함수

HANDLE CreateNamedPipe (\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpName,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwOpenMode,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwPipeMode,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nMaxInstances,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nOutBufferSize,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nInBufferSize,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nDefaultTimeOut,\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpSecurityAttributes\
)

1. lpName : 파이프 이름을 지정
   * \\\\.\\pipe\\pipename 과 같은 형태로 구성
2. dwOpenMode : 파일 읽기 쓰기 모드와 비슷한 개념
   * PIPE_ACCESS_DUPLEX : 읽기, 쓰기가 모두 가능하도록 설정
   * PIPE_ACCESS_INBOUND : 읽기만 가능 - 서버 입장
   * PIPE_ACCESS_OUTBOUND : 쓰기만 가능 - 서버 입장
3. dwPipeMode : 데이터 전송 타입, 수신 타입, 블로킹 모드 설정
4. nMaxInstances : 생성할 수 있는 파이프 최대 개수
5. nOutBufferSize : 출력 버퍼 사이즈, 0 Windows 디폴트 값
6. nInBufferSize : 입력 버퍼 사이즈, 0 Windows 디폴트 값
7. nDefaultTimeOut : WaitNamedPipe 함수에 지정할 기본 만료 시간
8. lpSecurityAttributes : 보안 속성


dwPipeMode의 매개변수 값
* 데이터 전송 방식
    1. PIPE_TYPE_BYTE(바이트)
    2. PIPE_TYPE_MESSAGE(메시지)
* 데이터 수신 방식
    1. PIPE_READMODE_BYTE(바이트)
    2. PIPE_READMODE_MESSAGE(메시지)
* 함수 리턴방식
    1. PIPE_WAIT(블로킹) : 이것이 전달된다고 기억
    2. PIPE_NOWAIT(논 블로킹)

생성한 파이프를 연결 요청 대기 상태로 변경시킬 때 사용하는 함수

BOOL ConnectNamedPipe (\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hNamedPipe,\
&nbsp;&nbsp;&nbsp;&nbsp;LPOVERLAPPED lpOverlapped\
)

1. hNamedPipe : CreateNamedPipe 함수 호출을 통해서 생성한 파이프 핸들
2. lpOverlapped : 중첩 I/O를 위한 인자

## Section 03 - 프로세스 환경변수
* 프로세스 생성 시 자식에게 정보를 전달할 때 파일보다 매개변수를 사용하는 것이 안정적이다.
* 추가로 다른 방법은 프로세스 환경변수를 활용하는 방법이다.

프로세스 환경변수
* 프로세스 별 별도의 메모리 공간에 문자열 데이터를 저장하고 관리
* 문자열의 구조 : key = value
* [key, value]의 형태로 둘 이상의 데이터를 관리하기가 좋다.
* 자식 프로세스 생성 시 환경변수 등록 가능
* 부모의 환경변수 또한 상속 가능

프로세스 환경변수 등록 함수
BOOL SetEnvirmentVariable (\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpName,\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpValue,\
);

1. lpName : key
2. lpValue : value

환경변수 참조 함수
DWORD GetEnvironmentVariable (\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR\
&nbsp;&nbsp;&nbsp;&nbsp;LPTSTR lpBuffer\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nSize\
);

* 성공 시 문자열 길이 반환

1. lpName : key 값 전달
2. lpBuffer : value 값 저장위한 공간
3. nSize : 버퍼 크기

## Section 04 - 명령 프롬프트 프로젝트 기능 추가
1. 프로세스 종료 기능
2. 현재 프로세스 출력 기능

### 구현을 위해 알아야할 함수
프로세스 조회
1. CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
   1. 두번째 인자 0 : 모든 프로세스, 프로세스 ID로 지정 가능
2. Process32First
   1. 구조체 멤버 얻어오기
3. Process32Next
   1. 다음 프로세스

프로세스 KILL
1. BOOL TerminateProcess (HANDLE hProcess, UINT uExitCode);
2. HANDLE OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
    * dwDesiredAccess : 접근 권한 - 종료시킬 것이므로 PROCESS_TERMINATE
    * bInheritHandle : 핸들 상속 여부
    * dwProcessId : 핸들을 얻어올 프로세스 ID

프로세스 KILL 하는 과정
1. 프로세스 이름
2. 프로세스 ID
3. 프로세스 핸들


## 이것만은 알고 갑시다.
1. 핸들 테이블

2. 핸들과 핸들 테이블
3. 핸들의 상속
4. 가짜 핸들 (Pseudo 핸들)
5. 파이프
