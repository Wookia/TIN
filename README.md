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
{
	addresses: [
		{
			address: ip
		},
		{...}]`
}
```
Zwrotka z numerem zadania:
```
{
	task: nr
}
```
Zapytanie o dane z numeru zadania:
```
{
	tasks:[
		{
			task: nr
		},
		{...}]
}
```
Dane z zadania:
```
{
	tasks:[
	{
	task: nr,
	addresses:[
		{
			address: ip,
		},
		{...}]
	},
	{...}
	]
}
```

####Obiekty:
Zadanie:
```
class Task:
	ip[]
	task_number
```
Wynik zadania(z Modułu 2):
```
class TaskResult:
	task_number
	addresses[traceroute[]] //tablica tablic (wiele adresow ip)
```
Adresy:
```
class Addresses:
	taks: nr
	addresses[traceroute[]] //tablica tablic (wiele adresow ip)
```
Sparsowane dane:
```
class ParsedData:
	Addresses[]
```

####SQLite(tabele): ??? zmianiamy na jakis plik tekstowy skoro nie mamy tego ładnie parsowac ???
Zadania:
```
TASK_NUMBER: numer zadania (pk)
START_DATE: czas rozpoczęcia zadania (notnull)
END_DATE: czas zakończenia zadania
```
Pojedynczy wynik tracerouta:
```
TASK_NUMBER: numer zadania (pk)
IP: adres ip (pk)
```
Relacja
```
PRE_IP: numer poprzedniego (pk, fk)
NEXT_IP: numer nastepnego (pk, fk)
```

Szczegółowy opis działania modułów
-------------
###Moduł 1
Moduł 1 odbiera JSON'y przesyłane od kilenta za pomocą protokołu HTTP(POST). Następnie w zależności od danego żądania będzie wykonywał jedno z dwóch zadań.
#### /doTraceroute
Moduł odbiera JSON'a z danymi do tracerouta (struktura powyżej) przekształca go do obiektu, nadaje unikalny numer zadania (który zwraca również w postaci JSON'a) a następnie umieszcza obiekt w kolejce oczekujących,
####/getData
Moduł odbiera JSON'a z numerem zadania. Składa żądanie do modułu 3 o dane o zadanym numerze. Jeśli w zwrocie dostaje dane to parsuje je do JSON'a którego zwraca, jeśli nie zwraca odpowiedni kod błędu

Moduł 1 działa na "jednym" samoklonującym się wątku który w sytuacji odebrania żądania tworzy swojego kolna a sam zajmuje się wykonaniem zadanego zadania.


###Moduł 2
Moduł nr 2 wykonuje operację trasowania pakietów. Wykorzystuje protokół ICMP - internetowy protokół komunikatów kontrolnych. Moduł wysyła komunikaty ICMP ECHO_REQUEST (znane np. z programu ping) z kolejnymi wartościami pola TTL i oczekuje komunikatów TIME_EXCEEDED (przekroczony TTL) oraz ECHO_REPLY (pakiet dotarł do celu, koniec trasy). Moduł podzielony jest na trzy zasadnicze elementy - generator pakietów, wątki wywysłające pakiety oraz wątek odbierający pakiety i rozdzielający odebrane dane według odpowiednich pól nagłówka odebranego komunikatu. Generator pakietów generuje pakiety o określoym TTL (Time-To-Live) i określonych wartościach pól Sequence i Identifier. Identifier to całkowitoliczbowy identyfikator konkretnej śledzonej trasy (czyli też wątku wysyłającego), a Sequence to TTL pakietu. Dzięki możliwości identyfikacji pakietów należących do poszczególnych tras i o konkretnych TTL, aplikacja może śledzić wiele ścieżek na raz.

Moduł do łączenia się ze światem zewnętrznym wykorzystuje tzw. "raw sockets", czyli gniazda umożliwiające wysyłkę i odbiór pakietów IP bez informacji warstwy transportu.

Maksymalny TTL, liczba pakietów bez informacji zwrotnej i oczekiwanie na odpowiedź to parametry konfiguracyjne, które mogą być określone w pliku XML i otagowane identyfikatorami ttl (1-255), attempts oraz timeout.

Algorytm trasowania:

1. Przyjmij od modułu nr 3 (kolejka) dane określające, jaka trasa ma być wyznaczona.

2. Uruchom wątek wysyłający pakiety. Wątek ma przydzielony "identyfikator trasy", który zostanie wykorzystany jako pole Identifier w nagłówku ICMP.

3. n = 1.

4. Wygeneruj za pomocą generatora pakiet o TTL = n, Identifier = identyfikator trasy, Sequence = n, wyślij go i poinformuj wątek odbierający o konieczności rozpoczęcia odmierzania czasu przeznaczonego na odbiór pakietu. Po tym czasie wątek odbierający poinformuje wątek wysyłający, że może spróbować jeszcze raz (ale maksymalnie tyle razy, ile wynosi parametr Attempts).

5. Czekaj na informację zwrotną od wątku odbierającego zawierającą adres IP routera pośredniczącego i kod odpowiedzi. Zinterpretuj informację - być może należy zakończyć trasowanie. Jeśli nie, dodaj adres do trasy. n += 1 i wróć do punktu 3.

6. Po zakończeniu trasowania wątek wysyłający przesyła do Modułu nr 3 wyznaczoną trasę lub jej fragment/kod błędu (struktura składająca się z nagłówka oraz listy adresów).
###Moduł 3
Moduł trzy zarządza wszelkim ruchem na serwerze. Obsługuje i wysyła żądania do wszystkich pozostałych modułów.
#### Interakcja z modułem 1:
1. Odbiór danych do tracerouta
2. Odbiór żądania danych wynikowych:
	a. Brak gotowych
	b. Sparsowanie danych i przesłanie do modułu 1
#### Interakcja z modułem 2:
1. Wrzucenie do kolejki danych do tracerouta
2. Odbiór z kolejki danych z tracerouta i sparsowanie ich.
#### Interakcja z bazą danych:
1. Dodanie nowego zadania
2. Dodanie danych z zadania - zakończenie zadania
3. Wyciągnięcie informacji o stanie zadania:
	a. Nieskończone
	b. Gotowe - parsowanie danych.

Moduł będzie działał na dwóch wątkach.
Pierwszy będzie cyklicznie sprawdzał czy w kolejkach nie ma zadań do wykonania a następnie w zależności od sytuacji wykonywał odpowiednie zadania takie jak parsowanie, zapisywanie do bazy, przesyłanie danych między kolejkami.
Drugi będzie przeznaczony tylko i wyłącznie do sytuacji związanych z żądaniami wyników jako, że takie działania mają priorytet (klient oczekuję na reakcję serwera). Będzie on sprawdzał gotowość zadania i w zależności od sytuacji zwracał informację o tym że zadanie jeszcze nie skończone lub parsował dane z bazy do wersji obiektowej i przesyłał z powrotem do modułu 1.
