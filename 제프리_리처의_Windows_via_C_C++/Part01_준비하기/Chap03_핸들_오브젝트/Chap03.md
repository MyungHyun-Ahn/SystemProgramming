# Chapter 03 핸들 오브젝트

윈도우 소프트웨어 개발자라면 반드시 커널 오브젝트에 대해 완벽하게 이해해야 한다.

윈도우는 수많은 리소스를 관리하기 위해 커널 오브젝트를 사용한다.

## Section 01 - 커널 오브젝트란 무엇인가?
윈도우 소프트웨어 개발자는 항시 커널 오브젝트를 생성하고, 열고, 조작하는 등의 작업을 수행한다.

커널 오브젝트들은 다양한 종류의 함수들을 통해 만들어지는데,
* 함수의 이름에 포함된 오브젝트 명칭이 커널 오브젝트와 반드시 일치하는 것은 아니다.

커널 오브젝트의 데이터 구조체는 커널에 의해서만 접근이 가능하기 때문에 직접 접근하는 것은 불가능하다.
* 이러한 제약사항은 커널 오브젝트가 일관되게 관리되기 위해 만들어졌다.
* 다른 애플리케이션에 영향을 미치지 않기 위해

마이크로소프트는 구조체의 내용에 접근할 수 있는 함수들을 제공하고 있다.

커널 오브젝트를 생성하면 핸들 값을 반환해준다.
* 32비트 : 4바이트
* 64비트 : 8바이트
* 커널 오브젝트를 구분하는 숫자이다.
* 핸들 값은 프로세스 내의 모든 스레드에 유효하다.

이러한 핸들값은 운영체제를 견고하게 하기 위해 프로세스 별로 독립적으로 유지된다.
* 프로세스 별로 핸들 테이블을 가지고 있고 같은 핸들 번호라도 다른 커널 오브젝트를 참조할 수도 있다.

### 사용 카운트
* 커널 오브젝트는 프로세스가 아니라 커널에 의해 소유된다.
* 프로세스가 종료되도 커널 오브젝트는 삭제되지 않을 수도 있다는 것이다.

각 커널 오브젝트는 내부적으로 사용 카운트(Usage count) 값을 유지하고 있기 때문에 이 값을 통해 몇개의 프로세스가 사용하고 있는지 알 수 있다.
* 최초로 생성되면 1이다.
* 커널 오브젝트에 접근 권한을 획득하면 1이 늘어나고
* 프로세스가 종료되면 1을 감소시킨다.
* 이 값이 0이 되면 커널 오브젝트는 사라진다.

### 보안
커널 오브젝트는 보안 디스크립터를 통해 보호될 수 있다.
* 누가 커널 오브젝트를 소유하고 있으며
* 어떤 그룹과 사용자들에 의해 접근되거나 사용될 수 있는지,
* 혹은 어떤 그룹과 사용자들이 제한되어 있는지에 대한 정보를 가지고 있다.

보안 디스크립터는 서버 애플리케이션을 개발할 때 주로 사용된다.

커널 오브젝트를 생성하는 거의 대부분의 함수들은 SECURITY_ATTRIBUTES 구조체의 포인터를 인자로 받는다.


대부분의 애플리케이션에서는 현재 프로세스의 보안 토큰을 근간으로 하는 기본 보안 디스크립터를 사용
* NULL 전달

typedef struct _SECURITY_ATTRIBUTES {\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nLength;\
&nbsp;&nbsp;&nbsp;&nbsp;LPVOID lpSecurityDescriptor;\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInheritHandle;\
} SECURITY_ATTRIBUTES;

* lpSecurityDescriptor 만이 보안과 관련있는 속성이다.

다음과 같이 초기화하고 사용해야 한다.

SECURITY_ATTRIBUTES sa;\
sa.nLength = sizeof(sa); // 버전 확인을 위한 정보\
sa.lpSecurityDescriptor = pSD; // 초기화된 SD 주소\
sa.bInheritHandle = FALSE; // 핸들 상속 옵션\
HANDLE hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, 1024, TEXT("MyFileMapping));


이미 존재하는 커널 오브젝트를 이용하려면 먼저 오브젝트를 이용하여 어떤 작업을 할지 알려주어야 한다.

데이터를 읽는 작업을 하려면 다음과 같이 한다.

HANDLE hFileMapping = OpenFileMapping(FILE_MAP_READ, FALSE, TEXT("MyFileMapping));
* 읽는 작업만 수행할 것을 나타냄
  
유효한 핸들을 반환하기 전 보안 권한을 먼저 확인한다.
* 접근이 거부될 경우 NULL이 반환
* GetLastError를 호출하면 5(ERROR_ACCESS_DENIED)가 반환
* READ 외에 다른 동작을 하려하면 ACCESS_DENIED 발생

만약 애플리케이션이 기동될 때 레지스트리 키로부터 값을 읽어오고 싶다면
* Reg-OpenKeyEx 함수를 호출할 때 KEY_QUERY_VALUE를 전달하는 것이 좋다.

옛날 어플리케이션들은 KEY_ALL_ACCESS를 전달하곤 했는데
* HKLM 하부에 존재하는 키들에 대해서는 관리자가 아닌 경우 읽기는 가능하지만 쓰기가 불가능하다는 것이 문제가 된다.
* 윈도우 비스타에서는 KEY_ALL_ACCESS를 전달하면 함수 호출에 실패한다.

HKLM : HKEY_LOCAL_MACHINE

개발자가 저지르는 가장 큰 실수 중의 하나가 알맞은 보안 접근 플래그를 쉽게 간과한다는 것이다.
* 올바른 플래그를 사용하면 다른 윈도우 버전으로 포팅이 더욱 간편해진다.

유저 오브젝트와 GDI 오브젝트와 커널 오브젝트는 서로 구분된다.
* PSECURITY_ATTRIBUTES 형의 매개변수를 받으면 커널 오브젝트다.

## Section 02 - 프로세스의 커널 오브젝트 핸들 테이블
프로세스가 초기화되면 운영체제는 프로세스를 위해 커널 오브젝트 핸들 테이블을 할당한다.
* 커널 오브젝트만을 위한 것이다.

오브젝트 핸들 테이블은 데이터 구조체의 배열로 이루어 있다.
1. 커널 오브젝트의 메모리 블록을 가리키는 포인터
2. 엑세스 마스크(각 비트별 플래그 값을 가지는 DWORD)
3. 플래그

### 커널 오브젝트 생성하기
* 프로세스가 최초로 초기화되면 프로세스의 핸들 테이블은 비어 있다.
* 프로세스의 스레드가 CreateFileMapping과 같은 함수를 호출하면
* 커널은 커널 오브젝트를 위한 메모리 블록을 할당하고 초기화한다.
* 이후 커널은 프로세스의 핸들 테이블을 조사하여 비어 있는 공간을 찾아낸다.
* 핸들 테이블에서 비어있는 인덱스를 찾아내고 초기화를 진행한다.
* 포인터 멤버는 커널 오브젝트의 자료 구조를 가리키는 내부적인 메모리 주소로 할당되고
* 액세스 마스크는 풀 액세스
* 플래그는 설정 상태로 초기화된다.

커널 오브젝트를 생성하는 모든 함수는 프로세스별로 고유한 핸들 값을 반환한다.
* 프로세스 내의 모든 스레드가 사용할 수 있다.
* 핸들 값 << 2 = 커널 오브젝트의 핸들 테이블 인덱스

커널 오브젝트를 인자로 취하는 함수들은 Create* 류의 함수를 호출하여 얻은 값을 전달
* 핸들 값은 실제로 프로세스 별 고유한 값
* 다른 프로세스와 공유할 수 없다.

커널 오브젝트 생성 함수가 실패하면 반환되는 값
1. 보통은 0(NULL)
2. 유효한 커널 오브젝트 핸들 값 4부터 시작
3. -1(INVALID_HANDLE_VALUE) 몇몇 함수의 경우 (CreateFile)

### 커널 오브젝트 삭제하기
종류에 상관없이 CloseHandle 함수를 호출한다.

BOOL CloseHandle(HANDLE hObject);
* 전달 받은 핸들값으로 핸들 테이블을 조회한다.
* 실제 커널 오브젝트에 접근 가능한지 확인한다.
* 핸들이 유효하고 시스템이 커널 오브젝트의 자료구조를 획득하면
* Usage Count 멤버를 감소시킨다. 0이 되면 커널 오브젝트는 소멸된다.

유효하지 않은 핸들을 CloseHandle 함수에 전달하면 발생하는 일
* 프로세스는 정상 수행 : CloseHandle FALSE 리턴 : GetLastError 호출 시 ERROR_INVALID_HANDLE
* 프로세스가 디버깅 중인 경우 : 0xC0000008 ("유효하지 않은 핸들이 지정되었습니다.") 예외 발생

CloseHandle 함수는 반환되기 직전에 프로세스 핸들 테이블에서 해당 항목을 삭제한다.
* 이렇게 되면 핸들은 유효하지 않은 값이 되고 작업 또한 수행 불가능
* 커널 오브젝트 자체는 삭제되지 않았을 수도 있다.


즉, 모든 프로세스가 해당 커널 오브젝트를 사용하지 않게 되면 그 때 파괴된다.

CloseHandle을 호출한 후에는 NULL로 변수를 밀어주자.


CloseHandle을 호출하지 않으면 오브젝트 누수가 발생할까?
* 프로세스가 수행 중이라면 그럴 수 있다.
* 프로세스가 종료되면 테이블 상에 유효한 항목이 있는 경우 해당 오브젝트 핸들을 삭제한다.
* 이 과정에서 Usage Count가 0이 되면 삭제된다.

즉, 애플리케이션 수행 중에는 커널 오브젝트에 대한 누수가 발생할 수 있지만, 프로세스가 종료될 때 적절히 정리하는 것을 보장한다.


오브젝트 누수를 작업관리자에서 확인할 수 있다.
* 핸들 항목의 값이 계속해서 증가하면 누수가 발생한 것이다.

## Section 03 - 프로세스간 커널 오브젝트의 공유
서로 다른 프로세스에서 수행되는 스레드 간에 동일 커널 오브젝트를 공유해야 하는 경우
* 파일-매핑 오브젝트
* 메일슬롯과 Named 파이프
* 뮤텍스, 세마포어, 이벤트

### 오브젝트 핸들의 상속을 이용하는 방법
프로세스들이 Parent-Child 관계를 가질 때만 사용될 수 있다.
* Parent 프로세스가 Child 프로세스를 생성할 때 Child 프로세스가 Parent 프로세스에 접근할 수 있게 해주는 방법이다.

Parent 프로세스에서 해야하는 작업
1. 커널 오브젝트가 생성될 때 핸들이 상속될 수 있음을 시스템에 알려야 한다.
2. SECURITY_ATTRIBUTES 구조체의 상속 옵션을 TRUE로 켜서 생성한다.


프로세스 핸들 테이블에 저장된 플래그 정보
* 플래그 비트 : 핸들이 상속 가능한지 여부를 가리킴
  * 상속 불가능 = 0, 상속 가능 = 1

다음 단계는 Parent 프로세스가 Child 프로세스를 생성하는 것

BOOL CreateProcess(\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszApplicationName,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszCommandLine,\
&nbsp;&nbsp;&nbsp;&nbsp;PSECURITY_ATTRIBUTES psaProcess,\
&nbsp;&nbsp;&nbsp;&nbsp;PSECURITY_ATTRIBUTES psaThread,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInheritHandles,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwCreationFlags,\
&nbsp;&nbsp;&nbsp;&nbsp;PVOID pvEnvironment,\
&nbsp;&nbsp;&nbsp;&nbsp;PCTSTR pszCurrentDirectory,\
&nbsp;&nbsp;&nbsp;&nbsp;LPSTARTUOINFO pStart
)

* bInheritHandles에 TRUE를 전달하면 상속 가능한 핸들을 상속한다.

1. TRUE가 전달되면 Parent 프로세스의 핸들 테이블을 조사하여 상속가능한 핸들을 찾아낸다.
2. 시스템은 찾아낸 핸들 테이블의 항복을 Child 핸들 테이블에 복사한다.
3. 이때 Child 핸들 테이블 내의 복사 위츠는 Parent 핸들 테이블과 정확히 일치한다.

즉, 핸들 값이 Parent와 Child가 모두 같은 값을 가지게 되는 것

두 개의 프로세스가 동일한 커널 오브젝트를 사용하게 되므로, 복사 작업과 Usage Count를 증가시킨다.

오브젝트 핸들의 상속은 Child 프로세스를 새로 생성할 때만 적용이 가능하다.

이때 이상한 점은 Child 프로세스는 어떤 핸들이 상속되었는지 알 수 없다.
* Parent 프로세스에서 Child 프로세스로 명령행 인자로 핸들 값을 전달해야 한다.
* 이렇게 획득한 값은 Parent 프로세스와 동일한 권한을 가지게 된다.

프로세스 간 통신을 이용해서 전달할 수도 있다.\
또다른 방법으로는 환경변수에 핸들 값을 추가하는 것이다.
  * 단순히 핸들 값을 얻기위해 GetEnvironmentVariable 함수를 호출하면 된다.

### 핸들 플래그를 변경하는 방법
SetHandleInformation 함수를 이용하면 된다.

BOOL SetHandleInformation(\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hObject,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwMask,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwFlags);

1. hObject : 유효한 핸들 값
2. dwMask : 변경하고자하는 플래그
   * HANDLE_FLAG_INHERIT
   * HANDLE_FLAG_PROTECT_FROM_CLOSE : 운영체제에게 이 핸들은 삭제할 수 없음을 알림.
     * CloseHandle을 호출하면 디버거가 예외를 발생시킨다.
   * | 연산자로 여러개를 전달 가능
3. dwFlags

핸들 오브젝트의 정보를 얻어오는 함수

BOOL GetHandleInformation(HANDLE hObject, PDWORD pdwFlags);
* pdwFlags에 flag 값을 얻어온다.

DWORD dwFlags;\
GetHandleInformation(hObj, &dwFlags);\
BOOL fHandleIsInheritable = (0 != (dwFlags & HANDLE_FLAG_INHERIT));
* 핸들의 상속 여부를 체크하는 코드


### 명명된 오브젝트를 사용하는 방법
대부분의 커널 오브젝트는 이름을 가질 수 있다.
* pszName 매개변수를 받는 경우

명명된 오브젝트를 사용하지 않더라도 핸들 상속이나 DuplicateHandle을 이용하여 프로세스간 커널 오브젝트를 공유할 수 있다.


pszName에 NULL 대신 문자열을 전달하여 오브젝트의 이름을 지정할 수 있다.
* MAX_PATH : 260 : 최대 길이

그런데 이미 이름이 존재할 수도 있다.
* 동일한 이름이 있는 경우 NULL이 반환된다.

명명된 오브젝트를 통한 커널 오브젝트 공유 과정
1. A 프로세스에서 이름을 붙여 커널 오브젝트를 생성한다.
2. B 프로세스에서도 이름을 넣어 커널 오브젝트를 생성하는 함수를 호출한다.
3. 먼저 이름을 가지고 같은 이름의 핸들이 있는지 검사한다.
4. 이후 운영체제는 B 프로세스가 오브젝트에 대한 최대 접근 권한이 있는지 확인한다.
5. 권한이 있다면 B 프로세스의 핸들 테이블에 추가하고, 이미 있던 커널 오브젝트를 가리키도록 설정한다.
6. 오브젝트 타입이 일치하지 않거나 접근 권한이 없다면 NULL을 리턴한다.

커널 오브젝트 생성 함수는 항상 최대 접근 권한을 가지고 있는 핸들을 리턴한다.
* 접근 권한을 설정하고 싶다면 Ex가 붙은 버전의 함수로 생성한다.

명명된 오브젝트가 이미 생성된 경우에만 사용할 수 있는 함수가 있다.
* Open* 류의 함수들
* pszName에 이름을 전달한다. NULL을 넣으면 안된다.

지정된 이름을 찾지 못하면 NULL을 반환한다.
* GetLastError를 호출하면 ERROR_FILE_NOT_FOUND를 반환한다.

커널 오브젝트가 존재하고 타입이 맞지 않는다면
* GetLastError가 ERROR_INVALID_HANDLE을 반환한다.

Create* 류와 Open* 류의 차이점은 커널 오브젝트가 존재하지 않는 경우 Create\*는 성공 Open*은 실패라는 것이다.


명명된 오브젝트는 동일한 애플리케이션이 여러번 수행되지 못하도록 하기 위해 자주 사용된다.
* Create* 류의 함수를 호출해서 결과가 ERROR_ALREADY_EXISTS 라면 애플리케이션을 종료시킨다.


### 터미널 서비스 네임스페이스
터미널 서비스를 수행하는 머신은 커널 오브젝트에 대해 다수의 네임스페이스를 가진다.
* 전역 네임스페이스 "Global\\\\"를 붙이면 전역
* 고유의 네임스페이스 "Local\\\\"를 붙이면 고유


### 프라이비트 네임스페이스
사용자 고유의 프라이비트 네임스페이스
* 서버 프로세스는 네임스페이스 이름 자체를 보호하기 위해 바운더리 디스크립터를 생성하고 이를 이용하여 프라이비트 네임스페이스를 생성해야 한다.
* 프라이비트 네임스페이스 간 이름을 공유


### 오브젝트 핸들의 복사를 이용하는 방법
DuplicateHandle 함수를 사용하는 것

BOOL DuplicateHandle(\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hSourceProcessHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hSourceHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;HANDLE hTargetProcessHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;PHANDLE phTargetHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwDesiredAccess,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInheritHandle,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwOptions);

1. hSourceProcessHandle : Source 프로세스 핸들
2. hSourceHandle : Source에서 복사할 핸들
3. hTargetProcessHandle : Target 프로세스 핸들
4. hTargetHandle : HANDLE 변수의 주소값
5. dwDesiredAccess : 새 핸들에 대해 요청된 액세스
   * dwOptions가 DUPLICATE_SAME_ACCESS인 경우 무시됨
6. bInheritHandle : 상속 여부
7. dwOptions : 옵션
   * DUPLICATE_CLOSE_SOURCE : 원본 핸들을 삭제
   * DUPLICATE_SAME_ACCESS : 원본 핸들과 동일한 권한