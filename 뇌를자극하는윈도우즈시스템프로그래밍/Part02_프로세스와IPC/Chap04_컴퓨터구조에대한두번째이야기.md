# 4장 컴퓨터 구조의 접근 방법
## Section 01 - 컴퓨터 구조의 접근방법
### 컴퓨터를 디자인하자
정확히 디자인할 요소는 CPU\
CPU의 기본 구성 요소는 ALU, 컨트롤 유닛, 레지스터가 있다.\
이중에서 레지스터만을 디자인 대상을 삼는다.\
시스템 프로그래머 입장에서 CPU 보는 대상이 대부분 레지스터에 집중된다.

### 레지스터를 디자인하자
레지스터를 디자인할 때 중요요소
1. 레지스터를 몇 비트로 구성할 것인가?
2. 몇 개 정도로 레지스터를 구성할 것인가?
3. 레지스터 각각을 무슨 용도로 사용할 것인가?

해당 책에서는 16비트 레지스터, 개수는 8개로 정하겠다.\
레지스터 이름은 r0-7로 정한다.

다음과 같이 레지스터가 존재하게 된다.\
&nbsp;&nbsp;&nbsp;&nbsp;r0\
&nbsp;&nbsp;&nbsp;&nbsp;r1\
&nbsp;&nbsp;&nbsp;&nbsp;r2\
&nbsp;&nbsp;&nbsp;&nbsp;r3\
&nbsp;&nbsp;&nbsp;&nbsp;r4 &nbsp;&nbsp;ir &nbsp;: instruction register\
&nbsp;&nbsp;&nbsp;&nbsp;r5 &nbsp;&nbsp;sp : stack pointer\
&nbsp;&nbsp;&nbsp;&nbsp;r6 &nbsp;&nbsp;lr &nbsp;&nbsp;: link register\
&nbsp;&nbsp;&nbsp;&nbsp;r7 &nbsp;&nbsp;pc : program counter

### 명령어 구조를 디자인하자
레지스터를 디자인 하였으니, 이를 바탕으로 명령어를 디자인할 차례이다.
* 여기서 중요한 것은 레지스터와 명령어의 상관관계이다.
* CPU가 달라지면 명령어 구조 또한 달라진다.

레지스터가 16비트이기 때문에 명령어 길이 또한 16비트로 디자인 한다.

16비트를 효율적으로 활용할 수 있는 방법을 생각해보자.\
"레지스터 r1에 있는 값과 숫자 7을 더해서 레지스터 r2에 저장하라"\
위 경우 '덧셈', 'r1', 'r2', '7'이라는 정보가 필요하다.

4가지 정보를 활용하기 위해 다음과 같은 구조를 디자인한다.\
예약 2비트, 연산자 3비트, 저장소 3비트, 피연산자1 4비트, 피연산자2 4비트\
&nbsp;&nbsp;[ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;] | [ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;] | [ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;] | [ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;] | [ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;]&nbsp;&nbsp;[ &nbsp;&nbsp;]


추가로 어셈블리 명령어도 결정하자.
* ADD : 001
* SUB : 010
* MUL : 011
* DIV : 100

각각의 레지스터를 나타내는 2진 코드는 다음과 같이 구성한다.
* r0 : 000
* r1 : 001
* r2 : 010
* r3 : 011
* r4, ir : 100
* r5, sp : 101
* r6, lr : 110
* r7, pc : 111

여기서 피연산자의 0001이 레지스터 r1을 나타내는지 숫자 1을 의미하는지 구분할 수 없게 된다.

따라서 피연산자의 최상위 비트는 0 : 숫자, 1 : 레지스터를 의미하도록 정한다.

그럼 이젠 r2 = r1 + 7 명령은 다음과 같이 표현할 수 있다.
* 00 001 010 1001 0111

어셈블리 코드는 다음과 같이 표현된다.
* ADD r2, r1, 7

명령어가 수행되는 과정은 다음과 같다.
1. 바이너리 코드 생성 Assemble
2. 메모리 로드 Load
3. IO Bus 통해 ir에 저장
4. ir 에 저장된 명령어가 컨트롤 유닛에서 해석 Decode
5. 해석된 명령어가 ALU에 의해 실행 Execution

* ir은 다음 실행될 명령어를 미리 가져다 두는 역할을 한다.

여기서 피연산자가표현할 수 있는 수가 8가지 밖에 되지 않는다는 문제가 생긴다.

여기까지 진행하며 결정된 제약사항들은 다음과 같다.
1. 첫 번째 피연산자 위치에는 레지스터 이름이 와야 한다.
   * 메모리 주소가 오게 하면 명령어 구조가 복잡해지고, 하드웨어 구성도 복잡해진다.
   * 명령어 종류에 따라 처리되는 시간이 다양해질 수 있다.

**"제약사항들은 CPU의 종합적 측면 (성능, 비용 등)이 고려되는 가운데서 등장하게 된다.""**

## Section 02 -LOAD & STORE 명령어 디자인
### LOAD & STORE 명령어의 필요성
지금까지 만든 명령어 구조에서 모든 피연산자에는 메인 메모리 주소가 올 수 없다는 제약사항이 있다.

다음과 같은 명령을 수행하지 못한다.\
int a = 10;\
int b = 20;\
int c = 0;\
c = a + b;

* 모든 변수들은 메인 메모리에 할당되었고 이를 피연산자로 사용할 수 없기 때문이다.
* 따라서 메인 메모리에 저장된 값을 레지스터에 불러오는 명령어와
* 레지스터에 있는 데이터를 메인 메모리에 저장하는 명령어가 필요하다.

### LOAD & STORE 명령어의 디자인
* LOAD 명령어는 이진코드 110으로 정의한다.
* LOAD r3, 0x07
* 0x07 번지에 있는 데이터를 r3에 저장

예약 LOAD destination sourc\
00 &nbsp;&nbsp;&nbsp;&nbsp;110 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;011 &nbsp;&nbsp;&nbsp;00000111

* STORE 명령어는 이진코드 111로 정의한다.
* STORE r2, 0x08
* r2의 데이터를 0x08 번지에 저장

여기까지 구성하면 이제는 c = a + b 명령을 수행할 수 있다.

1. LOAD r1, 0x10
2. LOAD r2, 0x20
3. ADD r3, r1, r2
4. STORE r3, 0x30

## Section 03 - Direct 모드와 Indirect 모드
### Direct 모드의 문제점과 Indirect 모드의 제안
하나의 명령어에 여러 정보를 담다보니 데이터 크기에 제한이 생겼다.

* LOAD 명령을 보면 source 부분의 비트 수는 총 8비트이다.
* 메인 메모리의 0x00ff 까지는 참조 가능하나 0x0100 부터는 참조가 불가능하다.

**직접 명령어에 메모리 주소값을 표현하기 때문에 Direct (직접) 모드이다.**

* 이런 단점을 해결하기 위해 Indirect 모드가 등장하였다.

### Indirect 모드의 이해
* Indirect 모드에서는 명령어에서 지정하는 번지에 저장된 값을 주소값으로 참조하게 된다.
* LOAD r1, [0x10] 참조 시

1. 0x30 : 0x10 번지
2. xxxx : 0x20 번지
3. 10   : 0x30 번지

10이라는 값이 레지스터 r1에 저장되게 된다.

여기서 연산이 direct 모드인지 indirect 모드인지 구분하기 위해 최상위 2비트 예약 공간을 사용하기로 하자.\
최상위 비트가 01이라면 indirect 모드, 00이라면 direct 모드이다.

## 이것만은 알고 갑시다.
1. 명령어

2. CPU의 내부 연산과 레지스터
3. Direct vs Indirect