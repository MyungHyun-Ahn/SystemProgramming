# Chapter 01 에러 핸들링

윈도우 함수가 사용하고 있는 반환 자료형
1. VOID : 절대 실패하지 않는 함수
2. BOOL : 실패 시 0 리턴, 절대 TRUE와 비교하면 안된다.
3. HANDLE : 실패하면 대개 NULL, 혹은 -1 : INVALID_HANDLE_VALUE
4. PVOID : 실패 시 NULL, 성공하면 메모리 주소
5. LONG/DWORD : 개수 반환, 실패하면 0이나 -1

윈도우는 발생할 가능성의 모든 에러 코드를 32비트 숫자로 정의

함수가 실패하면 TLS(thread local storage)에 저장
* GetLastError()로 에러를 출력해도 스레드간 안전성을 보장

윈도우의 함수 중에 성공 시 성공 이유를 파악하기 위해 GetLastError()를 호출하여 확인해야 한다.
* 이런 윈도우의 함수는 msdn에서 명세를 확인하자.

디버깅을 진행하며 Watch 창에서 현재 수행 중인 스레드의 마지막 에러코드와 메시지를 확인할 수 있는 기능을 제공한다.

DWORD FormatMessage(\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwFlags,\
&nbsp;&nbsp;&nbsp;&nbsp;LPCVOID pSource,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwMessageId,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwLanguageId,\
&nbsp;&nbsp;&nbsp;&nbsp;PTSTR pszBuffer,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nSize,\
&nbsp;&nbsp;&nbsp;&nbsp;va_list *Arguments\
);
* 에러 코드를 텍스트로 보여주는 함수

ErrorShow.cpp 분석
* Dlg_OnCommand이 id 값을 받으며 루프를 돔
* id : 1 -> 입력 받은 상태, 1000 -> 입력 받지 않은 상태
* GetDlgItemInt 함수로 ErrorCode 받아옴
* 그 ErrorCode에 대한 메시지를 FormatMessage를 통해 출력


## Section 01 - 자신만의 에러코드를 정의하는 방법
개발하는 함수 또한 실패할 수 있으며, 실패의 원인을 호출자에게 반환하도록 작성해야 한다.

스레드의 마지막 에러코드를 설정하기 위한 방법

VOID SetLastError(DWORD dwErrorCode);
* 함수 인자의 값은 어떤 32비트 값이라도 상관없다.
* 저자의 경우 이미 정의된 에러코드를 사용하는 편

에러코드 필드
1. 31-30 : 심각도 : 0 = 성공, 1 = 정보, 2 = 주의, 3 = 에러
2. 29 : 0 = 마이크로소프트가 정의한 코드, 1 = 사용자 정의 코드
3. 28 : 예약됨 : 항상 0
4. 27-16 : 식별 코드 : 256까지는 예약됨
5. 15-0 : 예외 코드 : 마이크로소프트나 고객이 정의한 코드

29번 필드만 기억하면 된다.
* 반드시 1로 설정해야 한다.