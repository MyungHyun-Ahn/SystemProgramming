# Chap15 - 애플리케이션에서 가상 메모리 사용 방법
마이크로소프트 윈도우에서 메모리를 사용하는 방법
* 가상 메모리, 크기가 큰 객체나 구조체의 배열을 관리하는 데 최적의 방법
* 메모리 맵 파일, 크기가 큰 스트림을 관리하거나 단일 머신에서 수행 중인 다수의 프로세스 사이에서 데이터를 공유하고자 할 때 최적의 방법
* 힙, 크기가 작지만 개수가 많은 객체를 관리하는 데 최적의 방법

## Section01 주소 공간 내에 영역 예약하기
VirtualAlloc 함수를 통해 프로세스의 주소 공간 내에 영역을 예약할 수 있다.
```C++
PVOID VirtualAlloc(PVOID pvAddress, SIZE_T dwSize, DWORD fdwAllocationType, DWORD fdwProtect);
```
* pvAddress : 예약하고자 하는 메모리 주소, 대부분 NULL 전달 - 알아서 할당
  * 성공하면 영역의 시작 주소 반환
  * 64KB 단위의 나누어 떨어지는 주소로 예약을 수행
  * 주소를 입력하더라도 내림 처리
* dwSize : 예약하고자 하는 영역의 크기를 바이트 단위로 전달
  * 4KB 단위의 배수
  * 62KB 예약하고자 한다면 64KB
* fdwAllocationType : 예약 혹은 커밋 수행 여부
  * 예약만 수행하고 싶다면 MEM_RESERVE
  * 가용 주소 공간 중 가장 높은 주소 공간 상에만 예약하고 싶다면 MEM_TOP_DOWN | MEM_RESERVE 전달
* fdwProtect : 보호 특성을 지정

커밋되지 않은 영역에 접근을 시도하게 되면 항상
* access violation 유발

## Section02 예약 영역에 저장소 커밋하기
예약한 영역을 접근하기 전 반드시 커밋해주어야 함
* 시스템의 페이지 크기 단위로 커밋을 수행

VirtualAlloc 함수를 다시 호출
* fdwAllocationType : MEM_COMMIT 플래그를 사용
* 보호특성은 보통 예약시와 동일하게 사용 PAGE_READWRITE를 가장 많이 사용

## Section03 영역에 대한 예약과 저장소 커밋을 동시에 수행하는 방법
MEM_RESERVE | MEM_COMMIT으로 동시에 전달

## Section04 언제 물리적 저장소를 커밋하는가
가상 메모리 기법의 장점을 이용하기 위한 프로그램 수행 단계
* 전체 구조체 행렬을 포함할 수 있는 충분한 크기 영역 예약 - 물리적 저장소를 필요로하지 않음
* 입력하려할 때 Access violation 발생
* Access violation이 발생한 위치에 물리적 저장소를 커밋
* 새로운 구조체의 멤버 설정

영역 내 특정 부분에 물리적 저장소가 매핑되었는지 여부를 확인하기 위한 방법
* 항상 물리적 저장소를 커밋해본다.
  * 이미 커밋되었다면 다시 커밋하지 않는다.
  * 구조체의 내용이 변경될 때마다 함수를 호출해야 하므로 프로그램이 더욱 느리게 동작한다.
* VirtualQuery 함수로 체크
  * 코드 또한 느리게 동작하고, 코드의 양도 증가
* 커밋 여부 기록
  * 어떤 방식으로든 페이지의 커밋 여부를 지속적으로 유지해야 한다.
* SEH 구조적 예외 처리를 이용
  * 가장 좋은 방법
  * 접근 위반이 발생하면 구조적 예외 처리기에서 처리

## Section05 물리적 저장소의 디커밋과 영역 해제하기
디커밋 혹은 해제를 하려면 VirtualFree 함수 호출
```C++
BOOL VirtualFree(LPVOID pvAddress, SIZE_T dwSize, DWORD fdwFreeType);
```
* pvAddress : 영역의 시작 주소
* dwSize : 항상 0을 전달한다 - 이미 시스템은 사이즈를 알고 있다.
* fdwFreeType : MEM_RELEASE - 디커밋과 동시에 해제, MEM_DICOMMIT - 디커밋만 수행

디커밋 이후에 페이지에 접근을 시도하면
* Access violation 유발

언제 디커밋 할 것인가
* 구조체를 페이지 크기와 정확히 일치시키는 것
  * 정확히 필요하지 않은 경우 물리적 저장소를 디커밋 가능
  * 그런데 머신에 따라 페이지의 크기가 다를 수 있음
* 구조체가 현재 사용 중인지 여부를 확인할 수 있는 정보를 포함
  * 페이지 내의 모든 상태를 확인하여 모든 구조체가 사용 중이 아닌 경우에 페이지를 디커밋
* 가비지 컬렉션 함수를 구현하는 것
  * 구조체 내의 BOOL 값을 저장하는 멤버를 두고
  * bInUse 값을 TRUE로 설정

## Section06 보호 특성 변경하기
VirtualProtect 함수를 이용
```C++
BOOL VirtualProtect(PVOID pvAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD pflOldProtect);
```
* PAGE_WRITECOPY와 PAGE_EXECUTE_WRITECOPY는 사용할 수 없다.
* 한번에 한 페이지만 변경 가능
  * 다수의 페이지를 변경하고 싶다면 여러번 함수를 호출해야 한다.

## Section07 물리적 저장소의 내용 리셋하기
VirtualAlloc 함수를 수행할 때
* 세 번째 매개변수로 MEM_RESET 전달
* 다음 접근하게 되면 0으로 초기화된 새로운 램 페이지가 사용됨

## Section08 주소 윈도우 확장
AWE(Address Windowing Extension)
* 윈도우 기법을 이용한 주소 확장

두 가지 목적을 염두에 두고 설계함
* 애플리케이션에게 운영체제에 의해 디스크로 스왑되지 않는 램을 할당할 수 있는 방법을 제공
* 애플리케이션이 프로세스의 주소 공간보다 더 큰 램에 접근할 수 있는 방법을 제공