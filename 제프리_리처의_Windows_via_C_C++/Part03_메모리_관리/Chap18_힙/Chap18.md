# Chapter 18 - 힙
힙
* 크기가 작은 데이터 블록을 할당하는 데 매우 유용한 방법

특징
* 할당 단위나 페이지 경계와 같은 특성을 고려할 필요가 없다.
* 비교적 수행 속도가 느림
* 물리적 저장소의 직접적인 제어권을 잃는다는 단점

## Section01 프로세스 기본 힙
프로세스의 기본 힙
* 프로세스가 초기화되면 시스템은 주소 공간내에 기본 힙을 생성
* 1MB의 영역을 예약 - 이보다 더 커질 수 있다.
* /HEAP 링커 스위치
  * 기본 힙 영역의 크기를 변경 가능

힙의 특징
* 스레드 세이프하다.
* 힙을 최대한 빠르게 사용하고 싶다면 독립적인 힙을 생성하는 것이 좋다.

힙 관련 함수들은 힙 핸들을 요구한다.

프로세스 기본 힙 핸들을 얻는 함수
* HANDLE GetProcessHeap();

## Section02 추가적으로 힙을 생성하는 이유
추가적으로 힙을 생성해야 하는 이유
* 컴포넌트 보호
* 더욱더 효율적인 메모리 관리
* 지역적인 접근
* 스레드 동기화 비용 회피
* 빠른 해제

### 1. 컴포넌트 보호
각각에 목적에 힙을 분리하여 사용
* 다른 컴포넌트가 다른 컴포넌트의 힙을 침범할 위험이 낮아짐

### 2. 더욱더 효율적인 메모리 관리
단편화를 최소화
* 구조체 경계 맞춤에 의한 여유 공간 생성

### 3. 지역적인 접근
비교적 가까운 거리에 데이터가 놓이게 되어 페이지 폴트가 발생하는 확률이 낮아짐

### 4. 스레드 동기화 비용 회피
유일하게 하나의 스레드만 접근하는 힙을 생성하게 되면 동기화 비용의 회피가 가능함

### 5. 빠른 해제
전체 힙을 삭제함으로 빠른 해제가 가능함

## Section03 추가적으로 힙을 생성하는 방법
추가적인 힙을 생성하는 함수
```C++
HANDLE HeapCreate(
    DWORD fdwOptions,
    SIZE_T dwInitialSize,
    SIZE_T dwMaximumSize
);
```

fdwOptions : 힙의 동작 방식을 결정하는 플래그
* 0, HEAP_NO_SERIALIZE, HEAP_GENERATE_EXCEPTIONS, HEAP_CREATE_ENABLE_EXECUTE 플래그들을 결합하여 전달 가능

HeapAlloc 함수를 호출 시 수행되는 작업
1. 할당된 메모리 블록과 프리 메모리 블록을 가지고 있는 링크드 리스트를 순회한다.
2. 할당 요청을 수용할 수 있을 만큼 충분히 큰 메모리 블록의 주소를 찾는다.
3. 프리 메모리 블록에 할당 여부를 표시함으로써 메모리가 할당되었음을 표시한다.
4. 메모리 블록 링크드 리스트에 새로운 항목을 추가한다.

HEAP_NO_SERIALIZE 플래그
* 스레드 동기화를 끔

사용해도 괜찮은 상황
1. 프로세스가 단일의 스레드만 사용한다.
2. 프로세스가 여러 개의 스레드들을 사용하지만, 이 중 하나의 스레드만 힙에 접근한다.
3. 프로세스가 여러 개의 스레드들을 사용하지만, 힙에 접근할 때 동기화를 수행한다.

HEAP_GENERIATE_EXCEPTIONS
* 메모리 할당이나 재할당이 실패할 경우 시스템이 예외를 유발하도록 한다.

HEAP_CREATE_ENABLE_EXECUTE
* DEP 기능 때문에 아주 중요
* 힙에 수행할 수 있는 코드를 저장하려 한다면 반드시 이 플래그를 지정한다.

dwInitialSize 매개변수
* 최초로 커밋할 바이트 수 지정
* CPU 페이지의 크기로 올림이 수행됨

dwMaximumSize 매개변수
* 힙의 최대 확장 크기
* 0을 사용하는 경우 계속 증가 가능한 힙을 생성

### 1. 힙으로부터 메모리 블록 할당
```C++
PVOI HeapAlloc(
    HANDLE hHeap,
    DWORD fdwFlags,
    SIZE_T dwBytes
);
```

dfwFlags 매개변수
* 메모리 할당 방식에 영향을 주는 플래그
* HEAP_ZERO_MEMORY, HEAP_GENERATE_EXCEPTIONS, HEAP_NO_SERIALIZE

HeapAlloc에 의해 발생할 수 있는 예외
* STATUS_NO_MEMORY
* STATUS_ACCESS_VIOLATION

큰 메모리 블록(대략 1MB 이상)을 할당하는 경우 VirtualAlloc을 권장

저단편화 힙
* low-fragmentation heap
* 수행 성능이 개선될 수 있다.

```C++
ULONG HeapInformationValue = 2;
if (HeapSetInformation(hHeap, HeapCompatiblityInformation, &HeapInformationValue, sizeof(HeapInformationValue)))
{
    // 저단편화 힙으로 변경 성공
}
else
{
    // 저단편화 힙 변경 실패
    // 아마도 HEAP_NO_SERIALIZE 플래그를 이용하여 힙을 생성하였기 때문
}
```

내부적으로 최적화가 발생하여 저단편화 힙을 사용할 수도 있다.

### 2. 블록 크기 변경
```C++
PVOID HeapReAlloc(
    HANDLE hHeap,
    DWORD fdwFlags,
    PVOID pvMem,
    SIZE_T dwBytes
);
```

### 3. 블록 크기 획득
```C++
SIZE_T HeapSize(
    HANDLE hHeap,
    DWORD fdwFlags,
    LPCVOID pvMem
);
```

### 4. 블록 해제
```C++
BOOL HeapFree(
    HANDLE hHeap,
    DWORD fdwFlags,
    PVOID pvMem
);
```

### 5. 힙 파괴
```C++
BOOL HeapDestroy(HANDLE hHeap);
```