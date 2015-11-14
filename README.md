TIN
===================

Celem zadania jest implementacja serwera umożliwiającego współbieżne wykonywanie wielu mapowań połączeń pomiędzy ruterami. Zachowanie pojedynczego sprawdzenia trasy zgodne jest z działaniem programu tracert ze środowiska MS Windows - program wysyła komunikaty ICMP ECHO_REQUEST z kolejnymi wartościami pola TTL i oczekuje komunikatów ICMP TIME_EXCEEDED. Zlecenie wykonania zadania oraz odbiór wyników wykorzystuje połączenie z wykorzystaniem protokołu HTTP i notacji/składni JSON.

Podział na moduły
-------------
####Moduł 1: Kontakt poprzez protokół HTTP za pomocą JSON'ów:
Odbiera prośby z poza serwera oraz zwraca dane.

####Moduł 2: Tracer:
Na podstawie żądań <b>Modułu 3</b> wykonuj tracerouta (TODO: lepiej to ubrać w słowa)
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
###Moduł 3
