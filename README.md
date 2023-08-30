# SIC-Assembler

자료구조 과제용 레퍼지토리입니다.  
과제 내용 : **자료구조를 활용한 SIC-Assembler 컴파일러 구현**

* **SIC-Assembler**  
시스템 프로그래밍과 주로 연관되지만, 자료구조 과제이므로 시스템 프로그래밍적인 요소는 많이 빠져있다.  
참고 : [SIC-Assembler](https://scshim.tistory.com/293)
<br><br>

* **구현할 기능**  
교육용으로 설계된 SIC-Assembler를 컴파일하고 목적코드를 생성할 수 있는 컴파일러를 설계하고 구현한다.

```
COPY START 2000
** LDA FIVE
** STA ALPHA
** LDCH CHARZ
** STCH C1
ALPHA RESW 1
FIVE WORD 5
CHARZ BYTE C'EOF'
C1 RESB 1
** END **
```

&emsp;&emsp;위 코드는 입력(input_file1)으로 받아서 컴파일할 어셈블리 코드이다.  
&emsp;&emsp;이 코드를 컴파일하여 다음과 같은 결과를 출력한다.

```
The contents of Input file:

COPY START 2000
** LDA FIVE
** STA ALPHA
** LDCH CHARZ
** STCH C1
ALPHA RESW 1
FIVE WORD 5
CHARZ BYTE C'EOF'
C1 RESB 1
** END **


Length of the input program is 20.

The contents of Symbol Table:

ALPHA 2012
FIVE 2015
CHARZ 2018
C1 2019


The contents of Intermed File:

COPY START 2000
2000 ** LDA FIVE
2003 ** STA ALPHA
2006 ** LDCH CHARZ
2009 ** STCH C1
2012 ALPHA RESW 1
2015 FIVE WORD 5
2018 CHARZ BYTE C'EOF'
2019 C1 RESB 1
2020 ** END **

Object Program has been generated.

Object Program:

H^COPY^002000^002020
T^002000^19^332015^442012^532018^572019^000005^454f46
E^002000
```
<br>

* **입력 파일 상세 설명**  
```
COPY START 2000
** LDA FIVE
** STA ALPHA
** LDCH CHARZ
** STCH C1
ALPHA RESW 1
FIVE WORD 5
CHARZ BYTE C'EOF'
C1 RESB 1
** END **
```

1. 'COPY START 2000' : 코드의 시작을 의미하고 2000번지부터 시작함을 나타낸다.
2. '** END **' : 코드의 마지막을 의미한다.
3. 코드의 처음이 '**'로 시작하면 명령어이다.
4. 명령어는 LDA , STA , LDCH , STCH 4개만 존재한다.
   * LDA FIVE : A register <- FIVE의 값
   * STA ALPHA : ALPHA <- A register의 값
   * LDCH CHARZ : A register의 값(rightmost byte) <- CHARZ의 값
   * STCH C1 : C1(byte) <- A register의 값(rightmost byte)
5. Storage는 다음과 같이 정의한다.
   * BYTE: 문자 혹은 16진수 상수를 생성한다.
   * WORD: 한 워드 정수 상수를 생성한다.
   * RESB: 지정해준 수만큼의 바이트를 예약한다.
   * RESW: 지정해준 수만큼의 워드들을 예약한다.
   * example
      * ALPHA RESQ 1 : 1 만큼의 워드를 예약
      * FIVE WORD 5 : 상수 5 생성
      * CHARZ BYTE C'EOF' : 문자상수 EOF 생성
      * C1 RESB 1 : 1만큼의 바이트 예약
<br><br>

* **구현 환경**  
  * 프로그래밍 언어 : C
  * 리눅스 환경에서 makefile을 통해 컴파일되고 실행 가능하여야 한다.
<br><br>

* **자료구조**  
헤드 노드를 포함한 원형 이중 연결 리스트를 사용하였다.  
리스트의 끝부분에서만 노드 삽입이 이루어지므로 원형 이중 연결 리스트를 선택하였고,  
코드가 간편해져 구현이 쉬워지므로 헤드 노드를 포함시켰다.
<br><br>

* **동작 설명**  
추후 수정 