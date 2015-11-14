TIN
===================

Celem zadania jest implementacja serwera umoÅ¼liwiajÄ…cego wspÃ³Å‚bieÅ¼ne wykonywanie wielu mapowaÅ„ poÅ‚Ä…czeÅ„ pomiÄ™dzy ruterami. Zachowanie pojedynczego sprawdzenia trasy zgodne jest z dziaÅ‚aniem programu tracert ze Å›rodowiska MS Windows - program wysyÅ‚a komunikaty ICMP ECHO_REQUEST z kolejnymi wartoÅ›ciami pola TTL i oczekuje komunikatÃ³w ICMP TIME_EXCEEDED. Zlecenie wykonania zadania oraz odbiÃ³r wynikÃ³w wykorzystuje poÅ‚Ä…czenie z wykorzystaniem protokoÅ‚u HTTP i notacji/skÅ‚adni JSON.

PodziaÅ‚ na moduÅ‚y
-------------
####ModuÅ‚ 1: Kontakt poprzez protokÃ³Å‚ HTTP za pomocÄ… JSON'Ã³w:
Odbiera proÅ›by spoza serwera oraz zwraca dane.

####ModuÅ‚ 2: Traceroute:
Wykonuj operacjÄ™ trasowania pakietÃ³w na Å¼Ä…danie moduÅ‚u nr 3.
####ModuÅ‚ 3: Centrum kontroli danych:
ObsÅ‚uguje dwie kolejki Å¼Ä…daÅ„: od <b> moduÅ‚u 1</b> i <b> moduÅ‚u 2</b>(moÅ¼e rÃ³wnieÅ¼ Å¼Ä…daÄ‡ wykonywania zadaÅ„). Na ich podstawie dokonuje parsowania danych do formy rozumianej przez konkretne moduÅ‚y i przesyÅ‚ania ich do bazy lub wyciÄ…gania z bazy w celu dalszej obrÃ³bki i zwrÃ³cenia Å¼Ä…danych danych.

Struktury danych
-------------

####JSON:
Å»Ä…danie wykonania tracerouta:
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
Wynik zadania(z ModuÅ‚u 2):
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

SzczegÃ³Å‚owy opis dziaÅ‚ania moduÅ‚Ã³w
-------------
###ModuÅ‚ 1

###ModuÅ‚ 2
ModuÅ‚ nr 2 wykonuje operacjÄ™ trasowania pakietÃ³w. Wykorzystuje protokÃ³Å‚ ICMP - internetowy protokÃ³Å‚ komunikatÃ³w kontrolnych. Podzielony jest na trzy zasadnicze elementy - generator pakietÃ³w, wÄ…tki wysyÅ‚ajÄ…ce pakiety oraz wÄ…tek odbierajÄ…cy pakiety i rozdzielajÄ…cy odebrane dane wedÅ‚ug odpowiednich pÃ³l nagÅ‚Ã³wka komunikatu. Generator pakietÃ³w generuje pakiety o okreÅ›lonym TTL (Time-To-Live) i okreÅ›lonych wartoÅ›ciach pÃ³l Sequence i Identifier. Identifier to caÅ‚kowitoliczbowy identyfikator konkretnej Å›ledzonej trasy, a Sequence to TTL pakietu. DziÄ™ki moÅ¼liwoÅ›ci identyfikacji pakietÃ³w naleÅ¼Ä…cych do poszczegÃ³lnych tras i o konkretnych TTL, aplikacja moÅ¼e Å›ledziÄ‡ wiele Å›cieÅ¼ek na raz.

Modu³ do ³¹czenia siê ze œwiatem zewnêtrznym wykorzystuje tzw. "raw sockets", czyli gniazda umo¿liwiaj¹ce wysy³kê i odbiór pakietów IP bez informacji warstwy transportu. 

Maksymalny TTL, liczba pakietów bez informacji zwrotnej i oczekiwanie na odpowiedŸ to parametry konfiguracyjne, które mog¹ byæ okreœlone w pliku XML i otagowane identyfikatorami <ttl> (1-255), <attempts> oraz <timeout>.

Algorytm trasowania:
1. Przyjmij od modu³u nr 3 dane okreœlaj¹ce, jaka trasa ma byæ wyznaczona.
2. Uruchom w¹tek wysy³aj¹cy pakiety. W¹tek ma przydzielony "identyfikator trasy", który zostanie wykorzystany jako pole Identifier w nag³ówku ICMP.
3. n = 1.
4. Wygeneruj za pomoc¹ generatora pakiet o TTL = n, Identifier = identyfikator trasy, Sequence = n, wyœlij go i poinformuj w¹tek odbieraj¹cy o koniecznoœci rozpoczêcia odmierzania czasu przeznaczonego na odbiór pakietu. Po tym czasie w¹tek odbieraj¹cy poinformuje w¹tek wysy³aj¹cy, ¿e mo¿e spróbowaæ jeszcze raz (ale maksymalnie tyle razy, ile wynosi parametr Attempts).
5. Czekaj na informacjê zwrotn¹ od w¹tku odbieraj¹cego zawieraj¹c¹ adres IP routera poœrednicz¹cego i kod odpowiedzi. Zinterpretuj informacjê - byæ mo¿e nale¿y zakoñczyæ trasowanie. Jeœli nie, dodaj adres do trasy. n += 1 i wróæ do punktu 3.
6. Po zakoñczeniu trasowania w¹tek wysy³aj¹cy przesy³a do Modu³u nr 3 wyznaczon¹ trasê lub jej fragment/kod b³êdu.

###ModuÅ‚ 3
