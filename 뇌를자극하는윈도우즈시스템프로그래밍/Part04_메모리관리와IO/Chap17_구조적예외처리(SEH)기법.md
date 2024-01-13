# 17장 구조적 예외처리(SEH) 기법
## Section 01 - SEH(Structured Exception Handling)
책의 예제 등의 코드를 보면 예외처리를 상당 부분 생략하는 경우가 많다.\
ex) malloc NULL 체크

* 하지만 이것은 매우 중요하다.

### 예외처리의 필요성
책에서 생략되는 이유
* 소스코드가 매우 길어진다.
* 복잡한 코드를 분석하는데 상당히 어려워진다.

프로그램과 실제 흐름과 예외처리 영역을 분리하자
* 프로그램의 실제 흐름 : __try
* 예외처리 영역 : __except

### 예외(예외상황)와 에러(혹은 오류)의 차이점
* 에러 : 처리 불가능한 문제
* 예외 : 처리가능한 프로그램 실행 시 발생하는 대부분의 문제

### 하드웨어 예외와 소프트웨어 예외
* 하드웨어 예외 : 하드웨어에서 인식하고 알려주는 예외
* 소프트웨어 예외 : 소프트웨어에서 감지하는 예외 - 프로그래머가 직접 정의할 수 있는 예외

## Section 02 - 종료 핸들러(Termination Handler)
* 사실 SEH는 약간 성능을 저하시킨다.
* 서버 프로그래밍 환경에서는 시스템 호환성을 조금이나마 높이기 위해 SEH를 사용하지 않는다.

SEH는 기능적 특성에 따라 크게 두 가지로 나뉜다.
1. 종료 핸들러
2. 예외 핸들러

### 종료 핸들러의 기본 구성과 동작 원리
종료 핸들러에서 사용되는 키워드 두가지는 __try와 __finally다.

__try{}\
__finally{}

* try 블록을 한 줄이라도 실행하면 finally 블록을 실행하라
* finally 블록의 실행은 컴파일러에 의해서 반드시 보장된다.
* 예외가 발생해도 finally 블록은 실행된다.

종료 핸들러 활용 사례
* 파일의 개발과 이에 따른 종료 구문
* 소유한 뮤텍스의 반환

## Section 03 - 예외 핸들러 (Exception Handler)
* 예외상황 발생 시 선별적 실행
### 예외 핸들러와 필터(Exception Handler & Filter)
* __try 블록 : 예외상황이 발생 가능한 영역을 묶는데 사용
* __catch(예외처리 방식) : 예외상황이 발생 했을 때 실행할 코드 지역
  * 가장 대표적인 것이 EXCEPTION_EXECUTE_HANDLER

### 예외 핸들러의 활용 사례
* 0으로 나누는 상황

### 처리되지 않은 예외의 이동
* Divide <- Calculator 함수 호출
* Divide 함수에서 예외 발생
* Calculator 코드 영역 내부에 Divide가 있으므로 예외 발생


### 핸들러의 중복
* 예외 핸들러는 중복이 가능하다.

### 정의되어 있는 예외의 종류와 예외를 구분하는 방법
* GetExceptionCode 함수를 호출해서 예외 코드를 얻어올 수 있다.

### EXCEPTION_CONTINUE_EXECUTION & EXCEPTION_CONTINUE_SEARCH
* 프로그램 개발 시 위 두가지 만을 사용하는 것이 좋다고 생각


EXCEPTION_CONTINUE_EXECUTION을 사용하는 경우
* 예외가 발생한 구문부터 다시 실행시키고 싶은 경우
* __except(FilterFunction(GetExceptionCode()))
* 위 구문을 사용하면 에러가 발생한 위치로 되돌아가 실행한다.

EXCEPTION_CONTINUE_SEARCH을 사용하는 경우
* 다른 곳에 있는 예외를 찾아서 수행하라는 의미
* _tmain -> Calculator 순서로 함수를 호출한 경우
* Calculator 함수에서 EXCEPTION_CONTINUE_SEARCH로 예외를 처리한 경우
* _tmain 함수에 있는 예외를 찾아서 처리한다.
* 그런데 이 예외처리 지시문을 반드시 써야하는가?
* 별로 좋지 못하다고 생각

## Section 04 - 소프트웨어 기반의 개발자 정의 예외
### 소프트웨어 예외(Software Exceptions)의 발생
* 하드웨어 예외는 결정되어 있다. 그러나 소프트웨어 예외는 결정해야만 한다.

예외 발생을 유발하는 함수

void RaiseException(\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwExceptionCode,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwExceptionFlags,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD nNumberOfArguments,\
&nbsp;&nbsp;&nbsp;&nbsp;const ULONG_PTR *lpArguments\
)

1. dwExceptionCode : 발생시킬 예외의 형태를 지정한다.
2. dwExceptionFlags : 예외발생 이후의 실행방식에 있어서 제한을 둘 때 사용한다.
3. nNumberOfArguments : 추가정보의 개수를 지정한다.
4. lpArguments : 추가정보를 전달한다.

## 이것만은 알고 갑시다.
1. SEH가 가져다 주는 이점

2. 종료 핸들러의 동작 원리와 적용 범위에 대한 이해
3. 대표적인 예외처리 방식
4. 처리되지 않은 예외의 이동
5. 과도한 SEH 처리는 좋지 않다.