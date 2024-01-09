# 5장 프로세스의 생성과 소멸
## Section 01 - 프로세스(Process)의 이해
### 프로세스란 무엇인가?
1. .exe 실행파일을 실행한다.
2. 프로그램을 실행하면 메모리 할당이 이뤄지고, 메모리 공간에 바이너리 코드가 올라간다.
3. 이 순간부터 프로그램은 프로세스라고 불린다.

**"즉, 프로세스란 실행 중에 있는 프로그램을 의미한다."**

### 프로세스를 구성하는 요소
C언어를 공부하며 프로그램이 실행될 때 구성되는 메모리 영역은 다음과 같다.\
메인 메모리 즉, RAM에 메모리 영역이 구성된다.
1. Code 영역 : 명령어 (Instruction Code)
2. Data 영역 : 전역 변수, Static 변수
3. Heap 영역 : 프로그래머 할당
4. Stack 영역 : 지역변수, 전달인자 정보

프로그램 실행 시 만들어지는 메모리 공간의 구성인데, 이것이 즉, 프로세스의 실체이다.

* 프로그램을 실행할 때마다 위의 메모리 공간이 실행된 프로그램의 개수만큼 구성된다.

**Register Set**\
프로세스를 구성하는 요소로 더불어 생각해야하는 것은 CPU 내의 레지스터들이다.

* 프로세스를 실행하면 CPU를 구성하는 레지스터들은 해당 프로세스를 실행하기 위한 데이터로 채워진다.
* **즉, 레지스터들의 상태까지도 프로세스의 일부로 포함시켜 말할 수 있다.**

## Section 02 - 프로세스의 스케줄링과 상태 변화
"CPU는 하나인데, 어떻게 여러 개의 프로그램이 동시에 실행 가능한 것인가?"\
기본적으로 CPU는 한 순간에 하나의 프로그램만 실행 가능하다.

### 프로세스의 스케줄링 (Scheduling)
CPU는 아주 빠르기 때문에 여러개의 프로세스를 고속으로 번갈아 실행시킬 경우\
사용자는 CPU가 동시에 여러 개의 프로그램을 실행시킨다고 느끼게 된다.

여기서 여러 개의 프로세스들이 CPU 할당 시간을 나눠서 사용한다.

**스케줄링의 기본 원리**
* 스케줄링(Scheduling) : 프로세스의 CPU 할당 순서 및 방법을 결정짓는 일을 뜻한다.
* 스케줄링 알고리즘 : 스케줄링에 사용되는 알고리즘을 뜻한다.
* 스케줄러(Scheduler) : 실제로 스케줄링 알고리즘을 적용해 프로세스를 관리하는 운영체제 모듈을 가리킨다.

### 프로세스의 상태 변화
Start - 1 -> Ready - 2 -> Running - 3 -> Ready\
Running - 4 -> Blocked - 5 -> Ready\
Blocked - 6 -> Exit

**상황 1 : Start에서 Ready 상태로의 전이를 보여준다.**
* 프로세스는 생성과 동시에 Ready 상태로 진입한다.
* CPU에 의해 실행되기를 희망하는 상태다.

왜 바로 실행이 안되고 대기할까?
* 멀티 프로세스 운영체제이기 때문에 임의의 프로세스가 실행중에 있을 것이다.
* 따라서 스케줄러에 의해 실행될 때까지 Ready 해야 한다.

**상황 2 : Ready 상태에서 Running 상태로의 전이를 보여준다.**
* 스케줄러에 선택된 프로세스는 Running 상태로 전환된다.

**상황 3 : Running 상태에서 Ready 상태로의 전이를 보여준다.**
* 프로세스는 생성 시 중요도에 따라 우선순위(Priority)라는 것이 매겨진다.
* 만약 프로세스 B가 실행 중인데 우선순위가 더 높은 A가 Ready 상태에 들어오면 B는 Ready로 전이되고 A가 Running 상태에 진입한다.

**상황 4 : Running 상태에서 Blocked 상태로의 전이를 보여준다.**
* 입출력 관련 작업을 하는 경우에 발생한다.
* 입출력 작업 진행 도중에는 CPU에 의해 프로세스가 더 이상 실행될 수 없다.
* 이 때 Ready 상태에 있던 다른 프로세스를 실행시키고 Blocked 상태로 전이된다.

**상황 5 : Blocked 상태에서 Ready 상태로의 전이를 보여준다.**
* Ready 상태와 Blocked 상태의 차이점은 Blocked 상태는 스케줄러에 의해 선택될 수 없다는 것이다.
* 입출력 작업이 완료되면 Ready 상태에서 Blocked 상태로 전이된다.

## Section 03 - 컨텍스트 스위칭 (Context Switching)
프로그램 실행 시간의 대부분은 입출력 작업으로 보내기 때문에 둘 이상의 프로세스 실행은 CPU 활용도를 높여 성능 향상까지 가져온다는 장점이 있다.

하지만 이것은 장점만을 이야기하는 것이다.

* Running 상태의 프로세스의 상태전이는 시스템에 많은 부하를 준다.
* CPU 내에 존재하는 레지스터들은 현재 실행 중에 있는 프로세스 데이터로 채워진다.

그럼 실행중인 프로세스가 변경되면 어떻게 될까?

* 레지스터에 있는 값이 변경된다.
Blocked 상태로 전환된 프로세스의 데이터를 백업하고 변경된 프로세스의 데이터를 Register에 채워준다.

* 위 프로세스 변경 과정에서 나타나는 작업을 Context Switching 이라고 한다.

**실행되는 프로세스의 변경과정에서 발생하는 컨텍스트 스위칭은 시스템에 많은 부하를 준다.**

## Section 04 - 프로세스의 생성
### 프로세스의 생성
* 일반 사용자는 더블 클릭해서 실행
* 프로그래머는 코드 내부 즉, 실행중인 프로세스를 만들어서 그 프로세스에서 다른 프로세스를 실행시키도록 할 수 있다.

### CreateProcess 함수의 이해
Windows는 프로세스 생성을 돕기 위해 CreateProcess 함수를 제공한다.

Process A (부모 프로세스) - CreateProcess -> Process B (자식 프로세스)
* 즉, 함수를 호출하는 프로세스 : 부모 프로세스
* CreateProcess 함수를 통해 생성된 프로세스 : 자식 프로세스

### CreateProcess 함수

BOOL CreateProcess (\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpApplicationName,\
&nbsp;&nbsp;&nbsp;&nbsp;LPTSTR lpCommandLine,\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpProcessAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;LPSECURITY_ATTRIBUTES lpThreadAttributes,\
&nbsp;&nbsp;&nbsp;&nbsp;BOOL bInheritHandles,\
&nbsp;&nbsp;&nbsp;&nbsp;DWORD dwCreationFlags,\
&nbsp;&nbsp;&nbsp;&nbsp;LPVOID lpEnvironment,\
&nbsp;&nbsp;&nbsp;&nbsp;LPCTSTR lpCurrentDirectory,\
&nbsp;&nbsp;&nbsp;&nbsp;LPPROCESS_INFORMATION lpProcessInformation\
)

1. lpApplicationName : 실행 파일 이름, 경로로 지정가능
2. lpCommandLine : 실행할 프로세스의 인자
3. lpProcessAttributes : 보안속성 NULL - Default
4. lpThreadAttributes : 보안속성 NULL - Default
5. bInheritHandles : TRUE인 경우 자식 프로세스는 부모 프로세스가 소유한 핸들 중 일부를 상속
6. dwCreationFlags : 프로세스의 특성(주로 우선순위) 지정, 보통 0
7. lpEnvironment : 환경블록
8. lpCurrentDirectory : 생성하는 프로세스의 현재 경로
9. lpProcessInformation : 생성한 프로세스 정보를 얻기 위해 사용