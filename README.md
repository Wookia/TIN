TIN
===================

Celem zadania jest implementacja serwera, umożliwiającego wykonywanie mapowań połączeń pomiędzy ruterami. Zachowanie algorytmu traceroutingu zgodne jest z działaniem programu tracert ze środowiska MS Windows - program wysyła komunikaty ICMP ECHO_REQUEST z kolejnymi wartościami pola TTL i oczekuje komunikatów ICMP TIME_EXCEEDED. Zlecenie wykonania zadania oraz odbiór wyników wykorzystuje połączenie z wykorzystaniem protokołu HTTP i notacji/składni JSON. Program będzie zrealizowany w środowisku Linuks.

Podział na moduły
-------------
####Moduł 1: Kontakt poprzez protokół HTTP za pomocą JSON'ów:
Odbiera prośby spoza serwera oraz zwraca dane.
####Moduł 2: Tracer:
Na podstawie żądań uzyskanych z <b>Modułu 3.</b> buduje pakiety i na ich bazie wykonuje zadanie traceroutingu.
####Moduł 3: Centrum kontroli danych:
Obsługuje dwie kolejki żądań: od <b> Modułu 1.</b> i <b> Modułu 2.</b> (może również żądać wykonywania zadań). Na ich podstawie dokonuje parsowania danych do formy rozumianej przez konkretne moduły i przesyłania ich do bazy lub wyciągania z bazy w celu dalszej obróbki i zwrócenia żądanych danych.

```sequence
Świat->Moduł 1: JSON(prośba o traceroute)
Moduł 1->Świat: JSON(nr. zad)
Moduł 1-->Moduł 3: Object(adresy IP + nr. zad)
Moduł 3-->Plik: Create(Name: nr. zad)
Moduł 3-->Moduł 2: Object(adresy IP + nr. zad)
Moduł 2-->Moduł 3: Object(Wynik tracerouta + nr. zad)
Moduł 3->Moduł 3: Parsowanie danych
Moduł 3-->Plik: Save(Name: nr. zad; Data: dane z tracerouta)
Świat->Moduł 1: JSON(nr. zad)
Moduł 1->Moduł 3: Object(nr. zad)
Moduł 3->Plik: Open(Nazwa: nr. zad)
Plik->Moduł 3: GetData(Data:dane z tracerouta)
Moduł 3->Moduł 3: Parsowanie danych
Moduł 3->Moduł 1: Object("dane sparsowane")
Moduł 1->Świat: JSON(dane)
```

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

####System plików
Zadania:
```
Nazwa: nr zadania
Treść: ip; ip; ip; ip; ip;\n ip; ip; ip; \n ip; ip; ip; ip; \n ...
```

Szczegółowy opis działania modułów
-------------
###Moduł 1
Moduł 1 odbiera JSON'y, przesyłane od kilenta za pomocą protokołu HTTP(POST). Następnie w zależności od danego żądania będzie wykonywał jedno z dwóch zadań.
#### /doTraceroute
Moduł odbiera JSON'a z danymi do tracerouta (struktura powyżej), przekształca go do obiektu, nadaje unikalny numer zadania (który zwraca również w postaci JSON'a), a następnie umieszcza obiekt w kolejce oczekujących.
####/getData
Moduł odbiera JSON'a z numerem zadania. Składa żądanie do Modułu 3. o dane o zadanym numerze. Jeśli w zwrocie dostaje dane, to parsuje je do JSON'a którego zwraca. Jeśli nie, zwraca odpowiedni kod błędu.

Moduł 1 działa na "jednym" samoklonującym się wątku, który w sytuacji odebrania żądania tworzy swojego klona, a sam zajmuje się wykonaniem zadanego zadania.


###Moduł 2
Moduł nr 2 wykonuje właściwą operację traceroute pakietów. Podzielony jest na trzy zasadnicze elementy: generator pakietów (działający w wątku wysyłającym), wątek wysyłający pakiety oraz wątek odbierający pakiety i rozdzielający odebrane dane według odpowiednich pól nagłówka odebranego komunikatu. Wykorzystuje protokół ICMP - internetowy protokół komunikatów kontrolnych.
Moduł wysyła komunikaty ICMP ECHO_REQUEST (znane np. z programu ping) z kolejnymi wartościami pola TTL i oczekuje komunikatów TIME_EXCEEDED (przekroczony TTL) oraz ECHO_REPLY (pakiet dotarł do celu, koniec trasy).
####Generator pakietów:
Ze względu na stosowanie protokołu ICMP zastosowany musi być tzw. "raw socket", czyli gniazda umożliwiające wysyłkę i odbiór pakietów IP bez informacji warstwy transportu. Zastosowanie tego typu gniazd wymagana ręcznego tworzenia pakietów do wysłania, odpowiedzialny za to będzie obiekt klasy Generator pakietów. Tworzy on pakiety IP o zadanym Adresie docelowym oraz TTL (Time-To-Live), w którym zawarty będzie pakiet protokołu ICMP o typie komunikatu ECHO_REQUEST i określonych wartościach pól Sequence i Identifier. Identifier to całkowitoliczbowy identyfikator konkretnej śledzonej trasy, a Sequence to TTL pakietu.

####Budowa pakietu ICMP Echo:
Nagłówek protokołu IP będzie budowany automatycznie, wykorzystując flagę IP_HDRINCL z API raw socket.
Nagłówek ICMP oraz dane będą budowane ręcznie w następujący sposób:
- Typ - typ wysyłanego komunikatu ICMP w naszym przypadku 08 - Echo request(1 bajt)
- Kod - podtyp wiadomości, naśladując MS Windows, program wpisuje kod 00 (1 bajt)
- Suma kontrolna - wyliczana na podstawie nagłówka pakietu ICMP oraz danych (2 bajty)
- Identyfikator - reprezentujący numer zadania, używany do identyfikacji odpowiadających pakietów (2 bajty)
- Numer sekwencji - reprezentujący TTL, jak identyfikator używany do identyfikacji odpowiadających pakietów (2 bajty)

Dane: 64 bajty zer.

####Wątek wysyłający
Przyjmuje zadania od Modułu 3., generuje za pomocą Generatora pakiety do wysłania, tworzy gniazdo i wysyła pakiety. Zapisuje informacje o wysłanym pakiecie (w tym czas wysłania) do kolejki, z której odbierze tę strukturę wątek odbierający.

####Wątek odbierający
Zastosowanie ICMP wraz z "raw socket" wymusza utworzenie jednego wątku odbierającego przez brak rozróżnienia portów. Jego zadaniem będzie odbieranie wszystkich pakietów ICMP i ich interpretacja (możemy np. otrzymać pakiet zupełnie niezwiązany z zadaniem).

####Komunikacja z Modułem 3.
Kolejka std::queue zabezpieczona semaforem, przechowująca struktury z adresami do traceroutingu (Moduł 2. <-- Moduł 3.).
Kolejka std::queue zabezpieczona semaforem, przechowująca wyznaczone trasy (Moduł 2. --> Moduł 3.).
Sygnał SIGUSR2 nadawany przez Moduł 3., pobudzający do działania wątki Modułu 2.
Sygnał SIGUSR2 nadawany przez Moduł 2. po wykonaniu zadania traceroutingu, informujący Moduł 3. o danych w kolejce wynikowej.

####Synchronizacja pomiędzy wątkami odbierającym i wysyłającym
Wątki współdzielą kolejkę typu std::queue, zabezpieczoną semaforem, przeznaczoną do dostarczania wiedzy do wątku odbierającego o wysłanych pakietach. Kolejka ta przechowuje struktury zawierające kompletny pakiet oraz wyodrębione najważniejsze informacje na jego temat: wartości pól Identifier, Sequence i czas wysłania.
Sygnał SIGUSR1 to polecenie "kontynuuj wysyłanie pakietów", wydawane wątkowi wysyłającemu przez wątek odbierający po odebraniu pakietu, zidentyfikowanego jako odpowiedź na pakiet wysłany pobrany z kolejki.
Aby uniknąć aktywnego oczekiwania na komunikaty (np. "zawieszając" się na zmiennej współdzielonej), wątki odbierają sygnały poprzez wykorzystanie funkcji pselect(), która poza korzystaniem ze standardowych deskryptorów gniazd, powiadamia również w razie wystąpienia jednego z sygnałów.

####Algorytm traceroute:

1. Wątek wysyłający przyjmuje od Modułu nr 3 dane, określające, jaka trasa ma być wyznaczona.

2. n = 1.

3. Wygeneruj za pomocą generatora pakiet o TTL = n, Identifier = numer zadania, Sequence = n, a następnie wyślij je i poinformuj wątek odbierający o wysłanych pakietach poprzez wstawienie pierwszego z nich i jego najważniejszych danych do współdzielonej kolejki.

4. Czekaj na sygnał od wątku odbierającego (funkcja pselect() o określonej wartości timeout, powodującej automatyczne przejście do następnego zadania). Jeśli mamy kontynuować wysyłanie pakietów, n += 1 i wróć do punktu 4.

5. Po zakończeniu traceroutingu wątek odbierający przesyła do Modułu nr 3 wyznaczoną trasę lub jej fragment/kod błędu (struktura składająca się z nagłówka oraz listy adresów). Jeśli w kolejce są kolejne trasy do wyznaczania, rozpoczynamy pracę.

####Parametry dotyczące Modułu 2.:

Parametry przechowywane są w standardowym tekstowym pliku konfiguracyjnym Module2.conf - jeden parametr odpowiadający jednej linii pliku.

MAX_TTL - domyślna wartość maksymalnego czasu życia pakietu.

MAX_PACKETS_PER_TTL - domyślna ilość pakietów wysyłanych do danego adresu z określoną wartościa TTL. Ze względu na brak gwarancji dostarczenia.

FREQ - częstotliwość wysyłania pakietów. Część zapór ogniowych może wykryć dużą ilość pakietów ICMP i zablokować dalszy ruch.

TIMEOUT - maksymalny czas oczekiwania na odpowiedź.
###Moduł 3
Moduł trzy zarządza wszelkim ruchem na serwerze. Obsługuje i wysyła żądania do wszystkich pozostałych modułów.
####Interakcja z Modułem 1.:
1. Odbiór danych do tracerouta.
2. Odbiór żądania danych wynikowych:
	a. Brak gotowych.
	b. Sparsowanie danych i przesłanie do Modułu 1.

####Interakcja z Modułem 2.:
1. Wstawienie do kolejki danych do tracerouta.
2. Nadanie sygnału SIGUSR2 do Modułu 2. w celu pobudzenia wątków tego modułu.
3. Odebranie sygnału SIGUSR2 przez odpowiedni wątek od Modułu 2., informującego o danych z tracerouta.
4. Odbiór z kolejki danych z tracerouta i sparsowanie ich.

####Interakcja z systemem plików:
1. Dodanie nowego zadania (utwórz plik z dopiskiem ze niegotowy).
2. Dodanie danych z zadania (otworzenie, zapis).
3. Wyciągnięcie informacji o stanie zadania:
	a. Nieskończone.
	b. Gotowe - parsowanie danych.

Moduł będzie działał na dwóch wątkach.
Pierwszy będzie cyklicznie sprawdzał, czy w kolejkach nie ma zadań do wykonania, a następnie w zależności od sytuacji wykonywał odpowiednie zadania, takie jak: parsowanie, zapisywanie do plików czy przesyłanie danych między kolejkami.
Drugi będzie przeznaczony tylko i wyłącznie do sytuacji związanych z żądaniami wyników, jako że takie działania mają priorytet (klient oczekuję na reakcję serwera). Będzie on sprawdzał gotowość zadania i w zależności od sytuacji zwracał informację o tym, że zadanie jest jeszcze nie skończone lub parsował dane z plików do wersji obiektowej i przesyłał z powrotem do Modułu 1.
