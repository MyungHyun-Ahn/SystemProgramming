# WSAEWOULDBLOCK과 WSA_IO_PENDING
## WSAEWOULDBLOCK(논블로킹 소켓)
논 블로킹(non-blocking) 소켓에서 작업이 즉시 완료되지 못하는 경우 발생함
* select() 방법 등으로 처리 가능

## WSA_IO_PENDING(Overlapped I/O 등의 비동기 작업)
비동기 입출력에서 작업이 비동기로 처리되고, 작업이 즉시 완료되지 않는 경우
* WSA_IO_PENDING은 "작업이 비동기로 예약되었고 완료되지 않았다"는 신호
* Overlapped 구조체 혹은 I/O 완료 포트를 통해 완료를 확인

## 둘의 공통점
둘 다 "작업을 즉시 처리할 수 없다"는 상황에서 발생함
* 그럼 여기서 의문 WSA_IO_PENDING과 WSAEWOULDBLOCK은 같은 상황에서 발생하는가?

## 소켓 함수 종류에 따른 비교
### accept
WSAEWOULDBLOCK
* 클라이언트 연결 요청이 없을 때 발생
* 즉, 백 로그 큐가 비어있음

WSA_IO_PENDING
* 연결 요청이 완료되지 않은 경우
  * 일단 WSAEWOULDBLOCK 상황은 확정적으로 PENDING이 발생
* 그럼 이 외에도 발생할 수 있는가?
  * 비동기로 작업이 완료되기까지 시간이 걸리는 경우 발생 가능
  * 즉, 백로그 큐에 요청이 있더라도 PENDING이 발생 가능함

### connect
WSAEWOULDBLOCK
* 원격 서버에 연결 요청이 즉시 완료되지 않은 경우
* syn 메시지만 전송하고 즉시 리턴함
* 거의 대부분의 상황에서 WSAEWOULDBLOCK이 발생함

WSA_IO_PENDING
* 비동기 연결을 시도한 경우
* WSAEWOULDBLOCK 상황에서는 무조건 발생함
  * 대부분의 상황에서 WSA_IO_PENDING이 발생함

### send
WSAEWOULDBLOCK
* 전송 버퍼가 꽉 찬 경우 발생
* 혼잡 윈도우가 꽉참

WSA_IO_PENDING
* WSAEWOULDBLOCK의 경우와 작업이 바로 완료되지 않는 경우

### recv
WSAEWOULDBLOCK
* 수신 버퍼에 데이터가 없을 때
  * TCP 헤더에 PSH 비트가 켜진 메시지가 없다면 WSAWOULDBLOCK이 반환됨

WSA_IO_PENDING
* WSAEWOULDBLOCK의 경우와 작업이 바로 완료되지 않는 경우

## 결론
WSA_IO_PENDING이 발생하는 상황
* WSAEWOULDBLOCK이 뜰 상황은 무조건 발생함
* WSAEWOULDBLOCK이 아니지만 비동기적으로 실행되는 경우에도 뜰 수 있음