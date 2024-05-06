# Chapter14 - 가상 메모리 살펴보기
## Section01 시스템 정보
현재 시스템의 구성 정보 값 가져오기
```C++
VOID GetSystemInfo(LPSYSTEM_INFO psi);
```
* 이 함수는 SYSTEM_INFO 구조체의 주소를 전달

SYSTEM_INFO 구조체의 정의
```C++
typedef struct _SYSTEM_INFO
{
    union
    {
        struct
        {
            WORD wProcessorArchitecture;
            WORD wReserved;
        };
    };

    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;
```
* 시스템이 부팅되는 순간 값이 결정
* 애플리케이션 수행 중 운영체제의 구성 정보를 얻어오기 위해 필요한 함수

메모리에 관련된 멤버
* dwPageSize : CPU 페이지 크기, x86/x64 머신에서는 4096 바이트
* lpMinimumApplicationAddress : 각 프로세스가 사용할 수 있는 가장 작은 주소 값
  * 65536 혹은 0x00010000
* lpMaximumApplicationAddress : 각 프로세스가 사용할 수 있는 가장 큰 주소 값
* dwAllocationGranularity : 최소 예약 단위 크기, 65536

메모리 관리와는 상관없는 멤버
* wReserved : 예약된 공간 - 사용하면 안됨
* dwNumberOfProcessors : 머신에 설치된 CPU의 개수
* dwActiveProcessorMask : 사용 가능한 CPU를 가리키는 비트 마스크
* dwProcessorType : 더 이상 사용되지 않음
* wProcessorArchitecture : 프로세서 아키텍처를 나타내는 값
* wProcessorLevel : 아키텍처를 세분화한 값
* wProcessorRevision : 프로세서의 레벨을 세부화한 값

## Section02 가상 메모리 상태
메모리의 현재 상태에 대한 동적인 정보를 획득하는 함수
```C++
VOID GlobalMemoryStatus(LPMEMORYSTATUS pmst);
```
* VirtualMemoryStatus라고 명명하는 편이 더 적절할 것 같음

MEMORYSTATUS 구조체
```C++
typedef struct _MEMORYSTATUS
{
    DWORD dwLength;
    DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;
```
* GlobalMemoryStatus 함수를 호출하기 전 dwLength 멤버를 구조체의 크기를 가지도록 초기화해야 함

만일 애플리케이션이 4GB 이상의 램을 장착한 머신에서 수행되거나 스왑 파일의 크기가 4GB 이상이라면

```C++
BOOL GlobalMemoryStatusEx(LPMEMORYSTATUSEX pmst);
```

MEMORYSTATUSEX 구조체
```C++
typedef struct _MEMORYSTATUSEX
{
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORDLONG ullTotalPhys;
    DWORDLONG ullAvailPhys;
    DWORDLONG ullTotalPageFile;
    DWORDLONG ullAvailPageFile;
    DWORDLONG ullTotalVirtual;
    DWORDLONG ullAvailVirtual;
    DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
```
* 각 멤버의 크기가 4GB 이상의 값을 수용할 수 있도록 64비트로 커짐
* ullAvailExtendedVirtual : 프로세스의 가상 주소 공간의 VLM 내의 예약되지 않은 메모리의 크기

## Section03 NUMA 머신에서의 메모리 관리
NUMA 머신
* Non-Uniform Memory Access 머신
* 자신의 노드뿐만 아니라 다른 노드의 메모리에도 접근이 가능
* 그러나 CPU는 자신의 노드에 속한 메모리에 접근하는 것이 월등히 빠름
* CPU는 기본적으로 자신의 노드에 위치한 램만을 물리적 저장소로 커밋하려 함
* 물리적 저장소가 충분하지 않을 때만 다른 노드의 램을 사용

특정 NUMA 노드 내의 가용 메모리만을 얻어오고 싶다면
```C++
BOOL GetNumaAvailableMemoryNode(UCHAR uNode, PULONGLONG pulAvailableBytes);
```

현재 CPU의 NUMA 노드의 번호를 가져오는 함수
```C++
BOOL WINAPI GetNumaProcessorNode(UCHAR Processor, PUCHAR NodeNumber);
```
현재 시스템의 전체 노드의 개수를 얻어오는 함수
```C++
BOOL GetNumaHighestNodeNumber(PULONG pulHighestNodeNumber);
```

이후 0부터 pulHighestNodeNumber가 가리키는 값 사이의 노드 번호를 이용하여 해당 노드에 설치되어 있는 CPU의 목록을 얻어올 수 있다.
```C++
BOOL GetNumaNodeProcessorMask(UCHAR uNode, PULONGLONG pulProcessorMask);
```

## Section04 주소 공간의 상태 확인하기
VirtualQuery 함수
```C++
DWORD VirtualQuery(LPCVOID pvAddress, PMEMORY_BASIC_INFORMATION pmbi, DWORD dwLength);
```
* Ex 함수를 사용하면 다른 프로세스의 메모리 정보를 가져올 수 있다.
* 디버거나 유틸리티 성격의 프로그램이 이용

PMEMORY_BASIC_INFORMATION 구조체
```C++
typedef struct _MEMORY_BASIC_INFORMATION
{
    PVOID BaseAddress;
    PVOID AllocationBase;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;
```
* BaseAddress : 전달한 주소를 페이지 크기 단위로 내림을 수행한 결과 값
* AllocationBase : 전달한 주소를 포함하는 영역의 시작 주소
* AllocationProtect : 해당 영역이 최초로 예약될 때 할당된 보호 특성
* RegionSize : BaseAddress로 시작하여 전달된 주소를 포함하는 페이지와 동일한 보호 특성, 상태, 페이지 형태를 가진 인접하는 모든 페이지들의 전체 크기를 바이트 단위로 나타냄
* State : 페이지의 상태 COMMIT, RESERVE, FREE
* Protect : 인접한 모든 페이지의 보호 특성
* Type : 모든 페이지의 물리적 저장소 형태를 나타냄

더욱 자세한 메모리 정보를 얻어올 수 있도록 하기 위한 함수
```C++
BOOL VMQuery(HANDLE hProcess, LPCVOID pvAddress, PVMQUERY pVMQ);
```

VMQUERY 구조체
```C++
typedef struct
{
    // 영역 정보
    PVOID pvRgnBaseAddress;
    DOWRD dwRgnProtection; // PAGE_*
    SIZE_T RgnSize;
    DWORD dwRgnStorage; // MEM_* : Free, Reserve, Image, Mapped, Private
    DWORD dwRgnBlocks;
    DWORD dwRgnGuardBlks; // 만일 이 값이 0을 초과하면 이 영역은 스레드 스택을 포함하고 있다.
    BOOL bRgnIsAStack; // 영역이 스레드 스택을 포함하고 있으면 TRUE

    // 블록 정보
    PVOID pvBlkBaseAddress;
    DWORD dwBlkProtection; // PAGE_*
    SIZE_T BlkSize;
    DWORD dwBlkStorage; // MEM_* : Free, Reserve, Image, Mapped, Private
} VMQUERY, *PVMQUERY;
```
* pvRgnBaseAddress : 가상 주소 공간 영역의 시작 주소
* dwRgnProtection : 최초로 예약될 때 할당된 보호 특성
* RgnSize : 해당 영역을 예약할 때 사용하였던 크기를 바이트 단위로
* dwRgnStorage : 어떤 형태의 물리적 저장소를 사용하는지, MEM_FREE, MEM_IMAGE, MEM_MAPPED, MEM_PRIVATE
* dwRgnBlocks : 해당 영역에 포함된 블록의 개수
* dwRgnGuardBlks : PAGE_GUARD 보호 특성을 가진 블록이 몇 개나 존재하는지를 나타냄, 1인 경우 해당 영역이 스레드 스택으로 사용된다는 의미
* bRgnIsAStack : 스레드 스택을 포함하고 있는지 - 100% 확신은 불가
* pvBlkBaseAddress : 매개변수로 전달한 주소를 포함하는 블록의 시작 주소
* dwBlkProtection : 블록의 보호 특성
* BlkSize : 블록의 크기를 바이트 단위로 나타냄
* dwBlkStorage : 블록의 내용이 어떤 형태인지