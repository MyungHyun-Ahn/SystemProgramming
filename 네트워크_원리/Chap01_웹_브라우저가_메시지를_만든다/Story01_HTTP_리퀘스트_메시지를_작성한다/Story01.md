# Story01 HTTP 리퀘스트 메시지를 작성한다.
## 1. 탐험 여행은 URL 입력부터 시작한다.
### URL - Uniform Resource Locator
http: 로 시작하는 것 뿐만 아닌 다른 것으로 시작하는 것도 있다.
* file:
* mailto:

다양한 URL이 준비되어 있는 이유
* FTP 파일 다운/업로드 등의 기능도 가지고 있다.
* 몇 가지 기능 중 어떤 것을 사용하여 데이터에 엑세스할지 판단하는 재료
* http: 웹 서버
* ftp: FTP 서버

이것을 사용하는 방법
* 웹서버 혹은 FTP 서버 : 도메인명이나 엑세스할 파일 경로를 URL에 포함
* 메일의 경우 : 상대 메일 주소를 URL에 포함
* 필요에 따라 사용자명 혹은 포트 번호를 포함

엑세스 방법을 나타내는 것
* http:, ftp:, file:, mailto:
* 프로토콜의 종류가 적혀있다.

## 2. 브라우저는 먼저 URL을 해독한다.
브라우저가 가장 먼저 하는 일

URL을 분해해서 해독한다.
* http: + // + 웹 서버명 + / + 디렉토리명 + / + ... + 파일명

## 3. 파일명을 생략한 경우
/ 로 끝나는 URL
* http://www.lab.cyber.co.kr/dir/
* 이와 같이 파일명을 생략해도 된다.
* 서버측에서 미리 설정해둔 파일로 엑세스 된다.
* 보통 index.html, 혹은 default.htm

웹 서버의 도메인명만 쓴 URL
* http://www.lab.cyber.co.kr/
* /라는 디렉토리가 지정되고 파일명은 생략된 것

/ 까지 생략
* http://www.lab.cyber.co.kr
* 위와 동일하게 해석

애매한 경우
* http://www.lab.cyber.co.kr/whatisthis
* whatisthis라는 파일이 있으면 파일로
* 디렉토리가 있으면 디렉토리명으로 본다.

## 4. HTTP의 기본 개념
### HTTP 프로토콜이란?
* 클라이언트와 서버가 주고받는 메시지의 내용이나 순서를 정한 것

기본적인 개념
1. 리퀘스트 메시지를 보낸다.
   * "무엇을" "어떻게 해서" 하겠다는 내용이 들어감
   * "무엇을" : URL, 보통 데이터 페이지 데이터를 저장한 파일의 이름 혹은 CGI 프로그램이 들어감
   * "어떻게 해서" : 메소드, 웹 서버에 어떤 동작을 하고 싶은지 전달
   * 보충적인 요소(헤더)도 있다.
2. 웹 서버에 리퀘스트 메시지가 도착하면 그 내용을 해독
   * "무엇을", "어떻게 하는지" 판단하고 요구에 따라 동작
   * 결과 데이터를 응답 메시지에 저장
  
### 응답 메시지
스테이터스 코드
* 맨 앞부분에 실행 결과가 정상 종료되었는지 또는 이상 정보를 기록하는 용도

이후 헤더 파일과 페이지의 데이터가 이어지고 이것을 클라이언트에 반송

응답 메시지 구조
1. 스테이터스 코드
2. 헤더 파일
3. 페이지의 데이터

이것이 클라이언트에 도착하여 브라우저가 화면에 표시

### HTTP 메소드
1. GET : URI로 지정한 정보를 도출
2. POST : 클라이언트에서 서버로 데이터를 송신, 폼에 입력한 데이터를 송신하는 경우 사용
3. HEAD : GET과 거의 같다. 데이터의 내용을 돌려보내지 않음
4. OPTIONS : 통신 옵션을 통지, 조사할 때 사용
5. PUT : URI로 지정한 서버의 파일을 치환, URI로 지정한 파일이 없는 경우에는 새로 파일을 작성
6. DELETE : URI로 지정한 서버의 파일을 삭제
7. TRACE : 서버측에서 받은 리퀘스트 라인과 헤더를 그대로 클라이언트에 반송
8. CONNECT : 암호화한 메시지를 프록시로 전송할 때 이용하는 메소드

GET 메소드
* 가장 많이 사용
* 보통 웹 서버에 엑세스하여 페이지의 데이터를 읽을 때 사용

POST 메소드
* 다음으로 자주 사용
* 폼에 데이터를 사용하여 웹 서버에 송신하는 경우에 사용

GET과 POST만 있다면 페이지 데이터를 웹 서버에서 읽거나 페이지에 있는 필드에 입력한 데이터를 웹 서버에 보내는 사용법만 가능
* PUT이나 DELETE를 사용하면 웹 서버의 파일을 바꿔쓰거나 삭제하는 것도 가능

## 5. HTTP 리퀘스트 메시지를 만든다.
URL을 해독하고 웹 서버와 파일명을 판단하면 이것을 바탕으로 HTTP의 리퀘스트 메시지를 만든다.
* 포멧이 미리 결정되어 있으므로 이것에 맞게 리퀘스트 메시지를 만든다.

### 리퀘스트 메시지
1. 리퀘스트 라인
   * 가장 중요한 것 맨 앞의 메소드
     * 보통 URL과 하이퍼링크는 GET
     * 폼에 데이터를 입력한 경우 POST
   * 한 칸 띄운 후 URI를 쓴다. 
   * 맨 끝에는 HTTP 버전 정보
2. 메시지 헤더
   * 다수의 항목이 저장
   * 날짜, 데이터 종류, 언어, 등등
3. 메시지 본문
   * 메시지 헤더 이후 아무 것도 쓰지 않은 하나의 공백 행을 넣고 송신할 데이터를 쓴다.
   * GET 메소드의 경우에는 본문이 비어있다.

## 6. 리퀘스트 메시지를 보내면 응답이 되돌아온다.
응답 메시지의 포맷도 기본적인 개념은 리퀘스트 메시지와 같다.
* 단 첫 번째 행이 다르다.

리퀘스트의 실행 결과를 나타내는 스테이터스 코드와 응답 문구를 첫 번째 행에 써야 한다.
* 스테이터스 코드 : 숫자 - 주로 프로그램 등에 실행 결과를 알려주는 용도
* 응답 문구 : 문장 - 사람에게 실행 결과를 알리는 것이 목적

응답 메시지가 도착
* 데이터를 추출한 후 화면에 표시하여 웹 페이지를 눈으로 볼 수 있다.

코드값
* 1xx : 처리의 경과 상황 등을 통지
* 2xx : 정상 종료
* 3xx : 무언가 다른 조치가 필요함을 나타냄
* 4xx : 클라이언트 측의 오류
* 5xx : 서버 측의 오류

태그 
* 영상 등을 포함한 경우, 문장 안에 영상 파일을 나타내는 태그 정보 포함
* 브라우저는 태그를 탐색
  * 영상을 포함하는 의미의 태그를 만나면 영상용 공백을 비워두고 문장을 표시
  * 이후 영상 파일을 웹 서버에서 읽어와 공백 부분에 표시
    * 이것 또한 리퀘스트 메시지
    * 영상이 3개라면 3개의 리퀘스트 메시지를 보낸다.

한개의 리퀘스트 메시지에 대해 한개의 응답을 되돌려보냄