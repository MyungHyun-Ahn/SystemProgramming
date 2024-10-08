# 2장 컴퓨터 네트워크
## 2.1 컴퓨터 네트워크를 구성하는 기기

네트워크 스위치
* 단말기 사이를 연결하는 기기
* 네트워크 케이블로 연결됨

LAN
* 네트워크 스위치 하나를 사이에 두고 단말기들이 연결되어 있는 것
* 로컬 지역 네트워크(Local Area Network)
* 별 모양으로 연결된 것을 star topology 라고 함
* 가까운 지역을 묶는 대표적인 컴퓨터 네트워크

### 2.1.1 OSI 모델
계층 1: 물리 계층
* 하드웨어를 다룸
* 어떤 파형의 전류로 보낼지 결정

계층 2: 데이터 링크 계층
* LAN에서의 통신을 가능하게 함

계층 3: 네트워크 계층
* WAN에서의 통신을 가능하게 함

계층 4: 전송 계층
* 상대방에게 데이터가 반드시 가게 해 줌

계층 5: 세션 계층, 계층 6: 표현 계층, 계층 7: 응용 계층
* 어플리케이션 계층

### 2.1.2 OSI 모델의 계층 2
데이터 링크 계층을 간단하게 설명
* 각 단말기는 고유한 주소를 가짐
* 단말기는 데이터를 프레임이라는 단위로 주고 받음

프레임(Frame)
* 데이터 링크 계층의 전송 단위
* 페이로드와 레더를 합친 것

LAN은 크기가 제각각
* 빌딩 한층 정도를 다룰 수 있음

몇 가지 문제
* 스위치 하나가 연결할 수 있는 단말기 수가 제한
* 단말기의 주소를 고유하게 만들기 힘듬

해결책 중 하나 LAN과 LAN을 연결하는 것
* WAN이라고 함 (Wide Area Network)

### 2.1.3 OSI 모델의 계층 3
네트워크 계층
* WAN으로 구성
* WAN에서는 게층적으로 데이터를 건네주는 방식으로 작동
* 라우터
* IP를 주소로 사용
* 인터넷 공유기도 일종의 라우터

## 2.2 인터넷
많은 스위치와 라우터가 연결되어 지구를 뒤덮은 것

## 2.3 컴퓨터 네트워크 데이터
### 2.3.1 스트림 형식
스트림
* 데이터의 흐름

컴퓨터 네트워크의 스트림
* 단말기 A에서 보낸 것을 단말기 B에서 꺼낸다.
* 단말게 A의 데이터는 단말기 B에서 꺼낸 데이터를 모두 이은 것과 같다.
* 즉, 보낸 개수와 받은 개수는 다를 수 있다.
* TCP의 특징

우리가 따로 헤더를 정의하여 해결한다.
* 데이터의 앞부분에 크기 정보를 기입

혹은 구분자 방식을 사용
* 마지막에 구분자를 대입
* 단, 데이터 사이에 구분자가 겹치면 안된다.

### 2.3.2 메시지 형식
메시지 형식
* 데이터 시작과 끝을 구별할 수 있다.
* 여러 필드로 나누어 사용

IP 패킷
* 데이터 크기, 송신자 주소, 수신자 주소, 체크섬 등이 들어있다.
* IP 패킷의 크기는 제한적
* 그러나 스트림이나 메시지에는 제한이 없다.
  * 운영체제의 네트워크 스택에서 관리해주기 때문
* IP 패킷은 쪼개져서 보내질 수 있다.
* 받는 쪽에서 재조립하여 해석

## 2.4 컴퓨터 네트워크 식별자
IP 주소
* IPv4 IPv6 두 종류
  * 서로 호환되지 않음
* IPv4 주소는 4바이트

포트
* 2바이트 정수 65535

앤드 포인트
* IP주소:포트

## 2.5 컴퓨터 네트워크 품질과 특성

### 2.5.1 네트워크 품질을 저해하는 것들
스위치나 라우터에 처리 한계를 넘어서 데이터가 도착
* 그냥 버림
  * 패킷 유실
  * 초과분을 버리는 게 더 낫다.
* 메모리에 누적
  * 장시간 지속되면 라우터가 멈추거나 재부팅되기도 함

패킷 유실이나 드롭 비율이 높을수록 네트워크 품질이 나쁨

패킷 유실률 정리
* 네트워크 기기가 처리할 수 있는 한계를 넘으면 패킷 유실 발생 가능
* 회선 신호가 약하거나 잡음의 섞이면 패킷 유실이 발생 가능

### 2.5.2 전송 속도와 전송 지연 시간
전송 속도
* 두 기기 간에 초당 전송될 수 있는 최대 데이터의 양
* 비트 혹은 바이트로 표현

전송 속도에 영향을 주는 것
* 선로의 종류와 품질
* 두 기기의 소프트웨어와 하드웨어 종류

레이턴시
* 두 기기 간에 데이터를 최소량 전송할 때 걸리는 시간
* ms 밀리초를 자주 사용

레이턴시에 영향을 주는 것
* 매체의 종류와 품질
* 라우터 처리 속도

### 2.5.3 네트워크 품질 기준 세 가지
전송 속도(스루풋)
* 전송될 수 있는 데이터의 단위 시간당 총량

패킷 유실률
* 데이터가 중간에 버려지는 비율

레이턴시
* 전송되는 데이터가 목적지에 도착하는데 걸리는 시간

## 2.6 컴퓨터 네트워크에서 데이터 보내기와 받기