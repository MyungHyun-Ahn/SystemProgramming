# 20장 메모리 관리(Virtual Memory) 컨트롤
## Section 01 - 가상 메모리(Virtual Memory) 컨트롤
윈도우에서는 가상 메모리를 컨트롤할 수 있는 기능을 제공
* 효율적인 메모리 사용을 위해서 도움이 된다.

### Reserve, Commit, Free
1. Reserve : 예약
2. Commit : 할당
3. Free : 할당되지 않음

Windows 시스템에서 부여할 수 있도록 정의한 페이지의 상태를 의미
* 페이지 크기 계산 공식
* 가상 메모리의 크기 / 페이지 하나당 크기 = 페이지의 개수

Commit : 물리 메모리에 할당된 부분
* 물리 메모리 : RAM과 하드 포함
* molloc을 통해 동적 할당하면 -> Commit 상태

만약 배열을 선언할 때 지금은 1개 페이지 부분이 필요한데 나중에는 늘어날 것이다?
* 조금씩 추가로 할당하면 되지 않을까?
* 순차적으로 연결된 메모리 공간이 필요하다면 위의 전략은 사용이 불가능하다.

*(배열이름 + a) : 의 접근이 불가능해진다.


이래서 나온 것이 Reserve 이다.
* Reserve 상태는 Free와 Commit의 중간 상태이다.
* 이것을 실험할 수 있는 예제가 바로 물리 메모리의 할당량이 증가하는 배열을 만드는 것


### 메모리 할당의 시작점과 단위 확인하기
메모리 할당 전 생각해야 하는 것
* 메모리 할당의 시작 주소, 할당할 메모리의 크기
* 가상 메모리 시스템은 페이지 단위로 관리되므로
* 페이지의 중간 위치부터 할당할 수 없으며, 페이지 크기의 배수 단위로 할당을 해야한다.

Windows에서는 메모리가 지나치게 조각나는 것을 방지하기 위해 조금 더 넓은 범위의 값을 할당의 경계로 정의한다.
* 기본 단위로 : Allocation Granularity Boundary라 한다.
* GetSystemInfo 함수를 통해 얻을 수 있다.

Page Size : 4 KB\
Alloc Granularity : 64 KB
* Alloc Granularity는 Page Size의 배수이다.


### VirtualAlloc & VirtualFree 함수
페이지 상태를 COMMIT, RESERVE 상태로 만드는 함수

LPVOID VirtualAlloc (\
&nbsp;&nbsp;&nbsp;&nbsp;LPVOID lpAddress,\
&nbsp;&nbsp;&nbsp;&nbsp;SIZE_T dwSize,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD flAllocationType,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD flProtect\
);

* 함수 실패 시 NULL
* 성공 시 시작 번지의 위치 반환

1. lpAddress
   * 예약 및 할당 하고자 하는 메모리의 시작 주소, 일반적으로 NULL을 전달하면 할당하고자 하는 크기에 맞춰 메모리의 위치를 임의로 결정
   * RESERVE 상태에 있는 페이지를 COMMIT 상태로 변경할 때는 해당 페이지의 시작 주소를 지정해야 한다.
   * 예약을 할 때는 Allocation Granularity Boundary를 기준으로 값이 조절되고 할당을 할 때는 페이지 크기 단위로 값이 조절된다.
2. dwSize : 할당하고자 하는 메모리 크기를 바이트 단위로 지정한다.
   * 메모리의 할당은 페이지 크기 단위로 결정한다. - 4의 배수
3. flAllocationType : 메모리 할당의 타입 지정
   * MEM_RESERVE : 페이지를 reserve 상태로 둔다.
   * MEM_COMMIT : 페이지를 commit 상태로 둔다.
4. flProtect : 페이지 별 접근 방식에 제한을 두는 용도
   * PAGE_NOACCESS : 접근을 허용하지 않는 reserve 상태에 둘 때 사용
   * PAGE_READWRITE : 읽기 쓰기를 모두 허용 commit 상태에 둘 때 사용

페이지 상태를 Free 상태에 두는 함수

BOOL VirtualFree(\
&nbsp;&nbsp;&nbsp;&nbsp;LPVOID lpAddress,\
&nbsp;&nbsp;&nbsp;&nbsp;SIZE_T dwSize,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwFreeType\
)

* 함수 실패 시 리턴 0

1. lpAddress : 해제할 공간의 시작 주소
2. dwSize : 해제할 메모리의 크기 바이트 단위
3. dwFreeType
   * MEM_RELEASE : 해당 페이지 Free 상태, 두 번째 전달인자는 반드시 0, lpAddress는 VirtualAlloc을 통해 받은 메모리 시작 위치
   * MEM_DECOMMIT : 페이지를 RESERVE 상태로 되돌린다.

### Dynamic Array Design
1. 시스템 페이지 사이즈와 'Allocation Granularity Boundary' 값을 얻어 온다.
   * 할당하고자 하는 메모리 위치에 직접적으로 관여하지 않겠다면, 페이지 사이즈만 얻어와도 된다.

2. 메모리를 예약(RESERVE)한다. 예약을 할 때는 필요하다고 예상되는 최대의 크기로 예약을 한다.
3. 필요한 만큼의 메모리를 물리 모리에 할당(COMMIT)한다. 필요에 따라 점진적으로 크기를 증가시킨다.
4. 할당했던 메모리를 반환한다.


## Section 02 - 힙(Heap) 컨트롤
### 디폴트 힙(Default Heap) & Windows 시스템에서의 힙
C의 malloc과 free, C++의 new와 delete 연산자를 사용하면 힙을 생성할 수 있다.
* 이 때 생성할 때 1 mb의 크기의 디폴트 힙에 메모리를 할당하게 된다.

힙 A
* 홍길동의 힙 A -> 슈퍼맨1 -> 슈퍼맨2 -> 슈퍼맨3

힙 B
* 최대수의 힙 B -> BATMAN1 -> BATMAN2 -> BATMAN3

위와 같이 두개의 리스트를 각각 다른 힙 영역에서 관리하면\
내부의 요소를 일일히 삭제하지 않고 한번에 지울 수 있다.


### 힙(Dynamic Heap) 생성이 가져다 주는 또 다른 이점
디폴트 힙 이외에 Windows 시스템 함수 호출을 통해서 생성되는 힙을 가리켜 동적 힙이라고 한다.
1. 메모리 단편화의 최소화에 따른 성능 향상
   * 힙을 미리 선언하면 할당된 페이지가 RESERVE 상태에 놓이기 때문에 메모리 단편화가 발생하지 않는다.
   * 프로그램의 로컬리티 특성을 활용할 수 있다.
2. 동기화 문제에서 자유로워짐으로 인한 성능 향상
   * 힙은 스레드가 공유하는 영역이다. - 둘 이상의 스레드가 힙에 동시접근할 수 있음
   * 여기서 말하는 동기화 문제는 변수를 동시 참조하는 것이 아니다. - 뮤텍스, 세마포어를 활용해야 함
   * 할당과 해제를 말하는 것이다.


## Section 03 - MMF(Memory Mapped File)
* 파일 일부를 가상 메모리 일부에 연결 시키는 메커니즘

## 이것만은 알고 갑시다.
1. 가상 메모리의 페이지 상태

2. 동적 힙 생성
3. MMF