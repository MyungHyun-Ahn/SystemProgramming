# Story02 - 웹 서버의 IP 주소를 DNS 서버에 조회한다.
## 1. IP 주소의 기본
HTTP 메시지를 만들면 OS에 의뢰하여 이것을 웹 서버에게 송신
* 브라우저는 HTTP 메시지 생성 / 해독은 가능하지만 송신 기능은 없다. - OS가 송신
* 이때 URL 안에 쓰여있는 서버의 도메인 명에서 IP 주소를 조사해야 한다.
* OS에 송신을 의뢰할 때는 IP 주소로 메시지를 받을 상대를 지정해야하기 때문

### TCP/IP의 기본이 되는 개념과 IP 주소
TCP/IP는 서브넷이라는 작은 네트워크를 라우터로 접속하여 전체 네트워크가 만들어진다고 생각할 수 있다.
* 서브넷이란, 허브에 몇 대의 PC가 접속된 것이라고 생각해도 좋다.
* 한 개의 단위로 생각하여 '서브넷'이라고 부름
* 라우터가 연결하면 네트워크 전체가 완성됨

여기서 네트워크 주소를 할당
* 동에 해당하는 번호를 서브넷에 할당
* 번지에 해당하는 번호를 컴퓨터에 할당

네트워크 번호와 호스트 번호를 합쳐 IP 주소라고 함

엑세스 대상의 서버까지 메시지를 운반할 때는 IP 주소에 따라 대상의 위치를 판단하고 운반
* 송신측이 메시지를 보내면
  * 서브넷 안에 있는 허브가 운반하고, 송신측에서 가장 가까운 라우터까지 도착
  * 라우터가 보낸 상대를 확인하여 다음 라우터를 판단
  * 이런 동작을 반복하면 최종적으로 상대의 데이터가 도착한다는 원리

IP 주소의 규칙
* 네트워크와 호스트 번호를 합쳐서 32비트로 한다는 것 뿐
* 즉, 네트워크를 구축할 때 사용자가 직접 내역을 결정할 수 있다.
* 이 내역을 나타내는 정보를 필요에 따라 IP 주소에 덧붙임 - 넷마스크

IP 주소
* 10.1.2.3

넷마스크
* 255.255.255.0

네트워크 번호화 호스트 번호로 변환
* 네트워크 번호 : 10.1.2.
* 호스트 번호 : 3

만약 호스트 번호 부분이 모두 0 또는 1인 경우
* 0이라면 : 서브넷 자첼르 나타냄
* 1이라면 : 브로드캐스트

## 2. 도메인명과 IP 주소를 구분하여 사용하는 이유
OS에 메시지 송신을 할 때는 IP 주소를 조사해야 함
* 그럼 URL에 서버명이 아닌 IP 주소를 쓰면?
  * 실제로 IP 주소를 써도 올바르게 동작
  * 그러나 기억하기 어렵다.
  * 따라서 서버의 이름을 쓰도록 한다.

실행 효율의 관점
* 이름은 그닥 좋은 방법이 아니다.
  * 해독해야할 바이트 수가 최대 255바이트
  * 라우터에 부담
* IP 주소라면 4바이트 분의 수치만 확인

따라서 네트워크 주소로 이름을 그대로 사용하는 것은 좋은 방법이 아니다.

DNS 
* 이름을 알면 IP 주소를 알 수 있다는 원리

## 3. Socket 라이브러리가 IP 주소를 찾는 기능을 제공
DNS 서버에 조회한다는 것
* DNS 서버에 조회 메시지를 보내고, 응답 메시지를 받는 것

DNS 클라이언트
* DNS 리졸버 혹은 리졸버라고 함

네임 리졸루션(Name resolution)
* DNS의 원리를 사용해서 IP 주소를 찾는 것
* 이것을 실행하는 것이 바로 리졸버(resolver)

리졸버의 실체는 Socket 라이브러리에 들어있는 부품화한 프로그램
* 리졸버는 라이브러리의 한 기능

## 4. 리졸버를 이용하여 DNS 서버를 조회한다.
리졸버의 프로그램 명과 웹 서버의 이름을 쓰기만 하면 리졸버 호출 가능
* 리졸버가 DNS 서버에 조회 메시지를 보내고
* DNS 서버에서 응답 메시지가 돌아온다.

## 5. 리졸버 내부의 작동
리졸버를 호출하면 제어가 리졸버의 내부로 넘어간다.
* 제어가 넘어간다.
  * 호출처의 프로그램은 쉬고, 호출한 대상 프로그램이 움직이기 시작하는 것

리졸버에 제어가 넘어가면 DNS 서버에 문의하기 위한 메시지를 만든다.
* 도메인 이름에 대한 서버의 IP 주소를 알려주세요.
* 메시지 송신 동작은 리졸버가 스스로 실행하는 것이 아닌 OS의 내부에 포함된 프로토콜 스택을 호출하여 실행을 의뢰

리졸버가 프로토콜 스택을 호출하면 제어가 리졸버에게 넘어감
* 여기서 메시지 보내는 동작을 실행, LAN 어댑터를 통해 메시지가 DNS 서버로 송신

조회 메시지가 DNS 서버에 도착
* 조회 내용을 조사하여 답을 찾음

답을 클라이언트에게 반송 - IP 주소

리졸버를 호출할 때 지정한 메모리 영역에 IP 주소를 저장

리졸버의 동작이 끝나고 제어가 다시 브라우저에 돌아옴

물론 DNS 서버에 메시지를 송신할 때도 DNS 서버의 IP 주소가 필요
* 그러나 이것은 이미 컴퓨터에 미리 설정되어 있다.