# Chapter13 - 윈도우 메모리의 구조
## Section01 프로세스의 가상 주소 공간
모든 프로세스는 자신만의 가상 주소 공간을 가짐
* 32비트 프로세서 : 0x00000000 ~ 0xFFFFFFFF
* 64비트 프로세서 : 0x00000000, 00000000 ~ 0xFFFFFFFF, FFFFFFFF
  * 16EB(엑사바이트)의 공간 - 엄청나게 크다
* 다른 프로세스에서 소유한 메모리는 숨겨져 있으며 접근이 불가능하다.

## Section02 가상 주소 공간의 분할
파티션(partition)
* 각 프로세스의 가상 주소 공간은 분할, 각각의 분할 공간을 파티션이라 함

### 1. 널 포인터 할당 파티션
NULL 포인터 할당 연산을 수행할 경우를 대비하기 위해 준비된 영역
* 이 영역에 접근을 시도하면 접근 위반(access violation)이 발생

윈도우에서는 이 파티션에 대해서는 Reserve 조차 허용하지 않는다

### 2. 유저 모드 파티션
프로세스 내의 주소 공간에 활용될 수 있는 파티션

CPU 아키텍처별 사용 가능한 유저 모드 파티션의 크기
* x86 : ~2GB
* x86 w/3GB : ~3GB
* x64 : ~8192GB
* IA-64 : ~7152GB

특징
* 다른 프로세스는 이 공간에 접근 불가능
* 프로세스가 유지해야 하는 대부분의 데이터가 여기 저장
* 이 특성으로 인해 시스템을 좀 더 안정적으로 동작할 수 있게 한다.
* DLL 모듈은 이 파티션에 로드

나머지 절반은 커널 모드의 파티션
* 커널 코드, 디바이스 드라이버 코드, 장치 I/O 캐시 버퍼, 논페이지 풀 할당(nonpaged pool allocation), 프로세스 페이지 테이블 등을 저장하기 위해 사용
* 2GB 공간에 겨우 집어넣을 수 있었다.


x86 윈도우에서 더 큰 유저 모드 파티션 획득하기
* x86 윈도우는 3GB까지 유저 모드 파티션으로 확장할 수 있다.
* BCD를 변경하면 된다.

64비트 윈도우에서 유저 모드 파티션으로 2GB만 사용하기
* 32비트 애플리케이션을 64비트 환경으로 포팅하는 경우
* 프로세스 가용 주소 공간을 2GB 이하로 제한하는 주소 공간 샌드박스 내에서 애플리케이션을 수행하는 방식
* 기본적으로는 주소 공간 샌드박스가 켜져있다.
* 전체 유저 모드 파티션을 쓰고 싶다면 /LARGEADDRESSAWARE 링커 스위치를 이용하여 링킹되어야 함

### 3. 커널 모드 파티션
운영체제를 구성하는 코드들이 위치
* 스레드 스케줄링, 메모리 관리, 파일 시스템 지원, 네트워크 지원 등의 코드와 모든 디바이스 드라이버들이 이 파티션에 로드
* 이 파티션 영역에 존재하는 내용은 모든 프로세스에 의해 공유
* 모든 프로세스의 유저 모드 파티션의 상위에 존재하긴 하지만, 이 파티션 내의 코드와 데이터는 완벽하게 보호
* 애플리케이션에서 이 파티션에 대해 읽/쓰기 시도 - access violation - 프로세스 종료

## Section03 주소 공간 내의 영역
프로세스가 생성되고 가상 주소 공간이 주어짐
* 대부분의 가상 주소 공간은 Free 혹은 할당되지 않은 상태
* VirtualAlloc 함수로 할당해야 한다. - Reserve 작업

주소 공간 상에 영역을 예약할 때
* 할당 단위(allocation granularity) 경계 상에 위치해야 한다.
* 64KB 단위 - 즉 시작 주소가 항상 64KB로 나누어 떨어지는 위치부터 시작

시스템이 프로세스를 대신하여 특별한 영역을 예약
* PEB (process environment block) 프로세스 환경 블록
  * 시스템에 의해 생성 수정 삭제 될 수 있는 데이터 구조체
* TEB (thread environment block) 스레드 환경 블록
  * 프로세스 내에 존재하는 모든 스레드에 대한 관리 작업을 돕는 공간
  * 프로세스 내 새로운 스레드가 생성되거나 삭제될 때에 맞추어 새로 예약되거나 해제
* 이런 운영체제가 관리하는 영역은 할당 단위에 자유롭다.
  * 64KB 경계부터 시작하지 않을 수 있다.
  * 그러나 영역의 크기는 여전히 CPU 페이지 크기의 배수여야 한다.

페이지 page
* 메몸리를 관리할 때 사용하는 최소 단위
* x86/x64 4KB
* 만약 10KB 공간을 예약하려 하면 12KB의 영역을 예약
  * 페이지 크기의 배수가 되도록 올림을 수행
* 더 이상 예약된 영역에 접근할 필요가 없다면
  * 해제 Release - VirtualFree 함수 사용

## Section04 물리적 저장소를 영역으로 커밋하기
예약된 영역을 사용하려면 반드시 물리적 저장소를 할당
* 물리적 저장소를 Commit 한다고 한다.
* VirtualAlloc 함수를 한 번 더 호출
* 64KB에 예약 공간 중 특정 페이지를 선택하요ㅕ 호출할 수 있다.

더 이상 물리적 저장소를 사용할 필요가 없다면
* VirtualFree 함수 호출
* Decommit 한다고 함

## Section05 물리적 저장소와 페이징 파일
최근의 운영체제는 디스크 공간을 메모리처럼 활용할 수 있는 기능
* 디스크 상에 존재하는 이런 파일을 페이징 파일(paging file)이라 한다
  * CPU가 접근하고자 하는 바이트가 램에 있는지 혹은 디스크에 있는지 여부를 판단할 수 있어야 한다.
  * 램이 커져보이는 것과 같은 효과를 가져온다.

프로세스 내의 스레드가 프로세스 주소 공간에 있는 데이터 블록에 접근을 시도하게 되면 둘 중 하나의 작업 수행
* 첫 번째 작업 - 램에 존재하는 경우
  * CPU는 데이터의 가상 메모리 주소를 메모리 내의 물리적 주소로 변경한 후 데이터 접근
* 두 번째 작업 - 페이징 파일 어딘가에 위치하는 경우
  * 페이지 폴트 (page fault) 발생 - CPU가 운영체제에게 통지
  * 운영체제는 램에서 프리 페이지를 찾는다.
    * 프리 페이지가 없는 경우 - 램의 페이지 중 하나를 프리 상태로 변경
    * 해당 페이지가 수정된 적이 없다면 단순히 프리 상태로 변경
    * 수정된 적이 있다면 페이지의 내용을 페이징 파일로 복사한 후 프리 상태로 변경
  * 작업이 끝나면 페이징 파일의 데이터를 프리 페이지로 가져온다.
  * CPU는 페이지 폴트를 유발했던 명령을 다시 수행
  * 가상 메모리를 물리적 램의 주소로 변경한 후 데이터에 접근

만약 이러한 작업이 많아지면
* 하드 디스크 트레쉬가 발생
* 시스템의 수행 속도 저하가 유발된다.

트레슁(trashing)
* 운영체제가 프로그램을 수행하지 못하고 대부분의 시간을 페이지 파일과 램 사이의 스와핑에 소비하는 현상
* 이 경우 추가적인 램을 설치하면 시스템의 성능이 개선된다.

### 1. 페이지 파일 내에 유지되지 않는 물리적 저장소
하드 디스크 상에 존재하는 .exe나 DLL 파일이 주소 공간의 특정 영역에 대한 물리적 저장소로 사용되는 경우
* 메모리 맵 파일이라 한다.
* 이런 파일을 로드하면 자동으로 프로세스의 주소 공간에 영역을 예약하고 해당 파일을 이 영역에 매핑
* 페이징 파일에 공간을 할당하는 대신 프로세스의 주소 공간에 예약딘 영역을 활용
* 페이징 파일의 크기를 크게 증가시키지 않고 그대로 유지

## Section06 보호 특성
메모리 페이지를 위한 보호 특성
* PAGE_NOACCESS
* PAGE_READONLY
* PAGE_READWRITE
* PAFE_EXECUTE
* PAGE_EXECUTE_READ
* PAGE_EXECUTE_READWRITE
* PAGE_WRITECOPY
* PAGE_EXECUTE_WRITECOPY

윈도우의 데이터 수행 방지 Data Execution Prevention(DEP)
* 멜웨어의 공격으로부터 보호하기 위한 기능
* 코드 영역만 PAGE_EXECUTE 보호 특성 사용
* 데이터 페이지는 다른 보호 특성 사용
  * 일반적으로 PAGE_READWRITE

만약 PAGE_EXECUTE 보호 특성이 설정되지 않은 페이지를 수행하면
* 접근 위반 예외를 유발

### 1. 카피 온 라이트 접근
여러 프로세스가 공유하는 데이터 페이지의 경우
* 특정 프로세스가 데이터를 변경하면 혼란 야기
* WRITECOPY 속성의 보호 특성은 절대 직접 사용하면 안된다.
  * .exe나 DLL 파일 이미지를 매핑할 때만 사용한다.

### 2. 특수 접근 보호 특성 플래그
PAGE_NOCACHE
* 커밋된 페이지에 대해 캐싱을 수행하지 않는다.
* 하드웨어 디바이스 드라이버 개발자를 위해 존재

PAGE_WRITECOMBINE
* 디바이스 드라이버 개발자들에 의해 주로 사용
* 단일 장치에 대한 여러 번의 쓰기 작업을 하나로 결합할 수 있도록 함

PAGE_GUARD
* 페이지에 내용이 쓰여졌을 경우 애플리케이션이 그 사실을 인지할 수 있도록 하기 위해 사용
* 운영체제는 자체적으로 스레드 스택을 생성할 때 이 플래그를 활용

## Section07 모두 함께 모아
영역의 타입
* Free : 어떠한 저장소로 매핑되지 않은 상태 - 예약조차 안됨 - 예약 수행 가능
* Private : 시스템의 페이징 파일에 매핑
* Image : 이전에 메모리 맵 이미지 파일에 매핑되었었음 - 더 이상 매핑되어 있지 않을 수 있다.
* Mapped : 메모리 맵 데이터 파일에 매핑되었었다. - 더 이상 매핑되어 있지 않을 수 있다.

## Section08 데이터 정렬의 중요성
데이터 정렬(data alignment)
* 운영체제의 메모리 구조 보다는 CPU의 구조와 관련이 있다.
* 메모리 주소 값을 데이터의 크기로 나누었을 때 나머지가 0인 경우 데이터가 정렬되어 있다고 함

CPU는 데이터가 적절하게 정렬되어 있을 때 효율적으로 접근 가능
* CPU가 메모리 상에 정렬되지 않은 데이터를 읽어오려 하면 두 가지 중 한 가지 작업 수행
  1. 예외 유발
  2. 정렬된 위치를 여러 번 읽어서 정렬되지 않은 데이터를 모두 읽을 때까지 반복
    * 성능 저하 실제 소요시간 2배 이상

x86 CPU가 데이터 정렬 문제를 처리하는 방법
* x86 CPU는 EFLAGS 레지스터 상에 AC(alignment check) 플래그를 가지고 있다.
* CPU에 전원이 인가되면 이 값은 기본적으로 0
  * 이 값이 0이먄 CPU가 정렬되지 않은 데이터에 접근하는 경우에도 값을 얻기 위해 수행해야 하는 추가적인 작업들을 자동적으로 수행
  * x86에서는 절대 이 값을 변경하지 않는다.
  * 비정렬 예외가 발생할 확률이 전혀 없다.

x64의 경우
* 비정렬 데이터에 접근할 경우 운영체제에게 그 사실을 알려줌
* 데이터 비정렬 예외를 유발 시킬지 혹은 문제를 해결하고 코드를 수행할 것인지 결정
  * SetErrorMode 함수를 통해 조건 변경 가능