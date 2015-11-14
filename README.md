TIN
===================

Celem zadania jest implementacja serwera umożliwiającego współbieżne wykonywanie wielu mapowań połączeń pomiędzy ruterami. Zachowanie pojedynczego sprawdzenia trasy zgodne jest z działaniem programu tracert ze środowiska MS Windows - program wysyła komunikaty ICMP ECHO_REQUEST z kolejnymi wartościami pola TTL i oczekuje komunikatów ICMP TIME_EXCEEDED. Zlecenie wykonania zadania oraz odbiór wyników wykorzystuje połączenie z wykorzystaniem protokołu HTTP i notacji/składni JSON.

Podział na moduły
-------------
####Moduł 1: Kontakt poprzez protokół HTTP za pomocą JSON'ów:
Odbiera prośby spoza serwera oraz zwraca dane.

####Moduł 2: Traceroute:
Wykonuj operację trasowania pakietów na żądanie modułu nr 3.
####Moduł 3: Centrum kontroli danych:
Obsługuje dwie kolejki żądań: od <b> modułu 1</b> i <b> modułu 2</b>(może również żądać wykonywania zadań). Na ich podstawie dokonuje parsowania danych do formy rozumianej przez konkretne moduły i przesyłania ich do bazy lub wyciągania z bazy w celu dalszej obróbki i zwrócenia żądanych danych.

Struktury danych
-------------

####JSON:
Żądanie wykonania tracerouta:
```
#TODO
```
Zwrotka z numerem zadania:
```
#TODO
```
Zapytanie o dane z numeru zadania:
```
#TODO
```
Dane z zadania:
```
#TODO
```

####Obiekty:
Zadanie:
```
#TODO
```
Wynik zadania(z Modułu 2):
```
#TODO
```
Numer zadania:  
```
#TODO
```
Sparsowane dane:
```
#TODO
```

####SQLite(tabele):
Zadania:
```
Nr, Data
#TOCHECK
```
Pojedynczy wynik tracerouta:
```
Id, Nr, Id-poprzednika
#TOCHECK
```

Szczegółowy opis działania modułów
-------------
###Moduł 1

###Modu� 2
Modu� nr 2 wykonuje operacj� trasowania pakiet�w. Wykorzystuje protok� ICMP - internetowy protok� komunikat�w kontrolnych. Modu� podzielony jest na trzy zasadnicze elementy - generator pakiet�w, w�tki wywys�aj�ce pakiety oraz w�tek odbieraj�cy pakiety i rozdzielaj�cy odebrane dane wed�ug odpowiednich p�l nag��wka odebranego komunikatu. Generator pakiet�w generuje pakiety o okre�loym TTL (Time-To-Live_ i okre�lonych warto�ciach p�l Sequence i Identifier. Identifier to ca�kowitoliczbowy identyfikator konkretnej �ledzonej trasy (czyli te� w�tku wysy�aj�cego), a Sequence to TTL pakietu. Dzi�ki mo�liwo�ci identyfikacji pakiet�w nale��cych do poszczeg�lnych tras i o konkretnych TTL, aplikacja mo�e �ledzi� wiele �cie�ek na raz.

Modu� do ��czenia si� ze �wiatem zewn�trznym wykorzystuje tzw. "raw sockets", czyli gniazda umo�liwiaj�ce wysy�k� i odbi�r pakiet�w IP bez informacji warstwy transportu. 

Maksymalny TTL, liczba pakiet�w bez informacji zwrotnej i oczekiwanie na odpowied� to parametry konfiguracyjne, kt�re mog� by� okre�lone w pliku XML i otagowane identyfikatorami <ttl> (1-255), <attempts> oraz <timeout>.

Algorytm trasowania:
1. Przyjmij od modu�u nr 3 dane okre�laj�ce, jaka trasa ma by� wyznaczona.
2. Uruchom w�tek wysy�aj�cy pakiety. W�tek ma przydzielony "identyfikator trasy", kt�ry zostanie wykorzystany jako pole Identifier w nag��wku ICMP.
3. n = 1.
4. Wygeneruj za pomoc� generatora pakiet o TTL = n, Identifier = identyfikator trasy, Sequence = n, wy�lij go i poinformuj w�tek odbieraj�cy o konieczno�ci rozpocz�cia odmierzania czasu przeznaczonego na odbi�r pakietu. Po tym czasie w�tek odbieraj�cy poinformuje w�tek wysy�aj�cy, �e mo�e spr�bowa� jeszcze raz (ale maksymalnie tyle razy, ile wynosi parametr Attempts).
5. Czekaj na informacj� zwrotn� od w�tku odbieraj�cego zawieraj�c� adres IP routera po�rednicz�cego i kod odpowiedzi. Zinterpretuj informacj� - by� mo�e nale�y zako�czy� trasowanie. Je�li nie, dodaj adres do trasy. n += 1 i wr�� do punktu 3.
6. Po zako�czeniu trasowania w�tek wysy�aj�cy przesy�a do Modu�u nr 3 wyznaczon� tras� lub jej fragment/kod b��du.

###Moduł 3
