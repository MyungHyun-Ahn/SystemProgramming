# Story04 - 프로바이더의 내부
## 1. POP와 NOC
액세스 회선을 통과한 패킷은 프로바이더의 라우터에 도착
* 이곳이 인터넷의 입구
* 여기부터 패킷은 인터넷의 내부로 진입

인터넷의 실체
* 다수의 프로바이더의 네트워크를 서로 접속한 것

### POP
ADSL이나 FTTH의 액세스 회선은 사용자가 계약한 프로바이더의 POP에 연결
* 인터넷의 입구인 라우터는 여기에 설치

POP
* 여러 유형의 라우터가 설치되어 있음
* 중앙 부분에 액세스 회선을 접속하는 몇 개의 라우터
  * 이렇게 하여 회선의 종류에 따라 여러 유형의 라우터를 구분하여 사용

POP의 구성
1. 위부터 차례로 전용선을 이용한 액세스 회선을 연결한 부분은 통신 회선용 포트를 장착한 보통의 라우터 사용
  * 전용선은 본인 확인이나 설정값 통지 등의 기능이 필요없기 때문
2. RAS 라우터
   * 전화 회선이나 ISDN이라는 다이얼업 회선을 연결하는 부분
   * 사용자로부터 걸려온 전화를 받는 기능
   * PPP 프로토콜의 패스워드 확인과 설정 정보 기능
3. PPPoE의 ADSL과 FTTH
   * PPPoE의 경우 ADSL과 FTTH를 운영하는 사업자에게 BAS가 설치됨
   * 이것과 연결하기 위한 라우터가 프로바이더에 설치
   * 본인확인, 설정 통지 동작은 BAS가 창구 역할
4. BAS와 연결되는 부분에는 보통의 라우터
   * ADSL 액세스 회선이 PPPoA를 채택한 경우에는 DSLAM에서 ATM 스위치를 경유하여 BAS에 연결 - 프로바이더의 라우터에 연결

### NOC
프로바이더의 핵심이 되는 설비
* 고성능의 라우터
* 전송능력이 테라비트/초를 넘는 것도 있다.
* 일반 사용자 라우터의 1만 배 이상

## 2. 건물 밖은 통신 회선 등으로 접속한다
POP나 NOC는 전국 각지에
* 회사의 서버 룸과 비슷
* 건물 안에 있으므로 라우터는 케이블로 직접 접속하거나 스위치를 경유하여 접속
  * 회사나 가정의 LAN
  * 회사의 머신 룸이라면 트위스트 페어 케이블을 사용하는 경우가 많다.
  * 하지만 프로바이더의 네트워크는 흐르는 패킷의 양이 많아 광섬유 케이블을 사용

멀리 떨어진 장소에 있는 NOC나 POP를 연결하는 방법
* 광섬유로 연결
  * 그러나 매우 비싸기 때문에 대형 프로바이더에 한정
* 그외의 프로바이더는 광섬유를 대출하여 연결
  * 통신 회선