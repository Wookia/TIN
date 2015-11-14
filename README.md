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

###Moduł 2
Moduł nr 2 wykonuje operację trasowania pakietów. Wykorzystuje protokół ICMP - internetowy protokół komunikatów kontrolnych. Moduł podzielony jest na trzy zasadnicze elementy - generator pakietów, wątki wywysłające pakiety oraz wątek odbierający pakiety i rozdzielający odebrane dane według odpowiednich pól nagłówka odebranego komunikatu. Generator pakietów generuje pakiety o określoym TTL (Time-To-Live_ i określonych wartościach pól Sequence i Identifier. Identifier to całkowitoliczbowy identyfikator konkretnej śledzonej trasy (czyli też wątku wysyłającego), a Sequence to TTL pakietu. Dzięki możliwości identyfikacji pakietów należących do poszczególnych tras i o konkretnych TTL, aplikacja może śledzić wiele ścieżek na raz.

Moduł do łączenia się ze światem zewnętrznym wykorzystuje tzw. "raw sockets", czyli gniazda umożliwiające wysyłkę i odbiór pakietów IP bez informacji warstwy transportu. 

Maksymalny TTL, liczba pakietów bez informacji zwrotnej i oczekiwanie na odpowiedź to parametry konfiguracyjne, które mogą być określone w pliku XML i otagowane identyfikatorami <ttl> (1-255), <attempts> oraz <timeout>.

Algorytm trasowania:

1. Przyjmij od modułu nr 3 dane określające, jaka trasa ma być wyznaczona.

2. Uruchom wątek wysyłający pakiety. Wątek ma przydzielony "identyfikator trasy", który zostanie wykorzystany jako pole Identifier w nagłówku ICMP.

3. n = 1.

4. Wygeneruj za pomocą generatora pakiet o TTL = n, Identifier = identyfikator trasy, Sequence = n, wyślij go i poinformuj wątek odbierający o konieczności rozpoczęcia odmierzania czasu przeznaczonego na odbiór pakietu. Po tym czasie wątek odbierający poinformuje wątek wysyłający, że może spróbować jeszcze raz (ale maksymalnie tyle razy, ile wynosi parametr Attempts).

5. Czekaj na informację zwrotną od wątku odbierającego zawierającą adres IP routera pośredniczącego i kod odpowiedzi. Zinterpretuj informację - być może należy zakończyć trasowanie. Jeśli nie, dodaj adres do trasy. n += 1 i wróć do punktu 3.

6. Po zakończeniu trasowania wątek wysyłający przesyła do Modułu nr 3 wyznaczoną trasę lub jej fragment/kod błędu.

###Moduł 3
