TIN
===================

Celem zadania jest implementacja serwera, umożliwiającego wykonywanie mapowań połączeń pomiędzy ruterami. Zachowanie algorytmu traceroutingu zgodne jest z działaniem programu tracert ze środowiska MS Windows - program wysyła komunikaty ICMP ECHO_REQUEST z kolejnymi wartościami pola TTL i oczekuje komunikatów ICMP TIME_EXCEEDED. Zlecenie wykonania zadania oraz odbiór wyników wykorzystuje połączenie z wykorzystaniem protokołu HTTP i notacji/składni JSON. Program będzie zrealizowany w środowisku Linuks.

Uruchamianie programu
-------------
Program będzie uruchamiany w środowisku Linuks za pomocą przykładowej instrukcji: <b>./traceroutemapper [PLIK]</b>, gdzie argumenty między nawiasami kwadratowymi są opcjonalne. PLIK to ścieżka (jeśli podamy samą nazwę program sproboje znaleźć ten plik w folderze w ktorym znajduje sie wykonywany program), do miejsca gdzie znajduje się plik konfiguracyjny. W przypadku braku argumentu PLIK domyślnie wczytywane są dane konfiguracyjne wpisane w projekcie.

Plik konfiguracyjny(domyślne dane w kodzie)
-------------
port_number:8080
ip_address:127.0.0.1
max_ttl:20
max_packets_per_ttl:8
freq:3
timeout:20
repo_path:
size_limit:100000


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
Żądanie wykonania tracerouta(nr 1.):
```
{
	addresses: [
		{
			address: ip
		},
		{...}]`
}
```
Zwrotka z numerem zadania(nr. 2):
```
{
	task: nr
}
```
Zapytanie o dane z numeru zadania(nr 3.):
```
{
	tasks:[
		{
			task: nr
		},
		{...}]
}
```
Dane z zadania(nr. 4):
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

####System plików
Zadania:
```
Nazwa: nr zadania
Treść: ip; ip; ip; ip; ip;\n ip; ip; ip; \n ip; ip; ip; ip; \n ...
```

Szczegółowy opis działania modułów
-------------
###Moduł 1
Moduł 1 odbiera JSON'y, przesyłane od kilenta za pomocą protokołu HTTP(POST). Następnie w zależności od danego żądania będzie wykonywał jedno z dwóch zadań. W przypadku błędnej struktury lub zbyt dużego rozmiaru JSON'a, serwer zwraca błąd 400 BAD REQUEST z informacją typu JSON o powodzie błędu.
####doTraceroute
Moduł odbiera JSON'a z danymi do tracerouta (nr. 1), przekształca go do obiektu, nadaje unikalny numer zadania (który zwraca również w postaci JSON'a), a następnie umieszcza obiekt w kolejce oczekujących.
####getData
Moduł odbiera JSON'a z numerami zadań(nr. 3). Wywołuje metodę Modułu 3. o dane o zadanym numerze. Jeśli w zwrocie dostaje dane, to parsuje je do JSON'a którego zwraca. Jeśli nie, zwraca '404 NOT FOUND'.

Moduł 1 działa na "jednym" samoklonującym się wątku, który w sytuacji odebrania żądania tworzy swojego klona, a sam zajmuje się wykonaniem zadanego zadania.

####Parametry dotyczące Modułu 1:

Parametry przechowywane są w standardowym tekstowym pliku konfiguracyjnym ConfigFile.conf - jeden parametr odpowiadający jednej linii pliku:

port_number - numer portu, na którym nasłuchuje serwer

ip_address - adres IP, na którym nasłuchuje serwer

###Moduł 2
Moduł nr 2 wykonuje właściwą operację traceroute pakietów. Podzielony jest na cztery zasadnicze elementy: wątek zarządcy, generator pakietów (działający w wątku wysyłającym), wątek wysyłający pakiety oraz wątek odbierający pakiety. Wykorzystuje protokół ICMP - internetowy protokół komunikatów kontrolnych.
Moduł wysyła komunikaty ICMP ECHO_REQUEST (znane np. z programu ping) z kolejnymi wartościami pola TTL i oczekuje komunikatów TIME_EXCEEDED (przekroczony TTL) oraz ECHO_REPLY (pakiet dotarł do celu, koniec trasy).
####Generator pakietów:
Ze względu na stosowanie protokołu ICMP zastosowany musi być tzw. "raw socket", czyli gniazda umożliwiające wysyłkę i odbiór pakietów IP bez informacji warstwy transportu. Zastosowanie tego typu gniazd wymagana ręcznego tworzenia pakietów do wysłania, odpowiedzialny za to będzie obiekt klasy Generator pakietów. Tworzy on pakiety IP o zadanym Adresie docelowym oraz TTL (Time-To-Live), w którym zawarty będzie pakiet protokołu ICMP o typie komunikatu ECHO_REQUEST i określonych wartościach pól Sequence i Identifier. Identifier to całkowitoliczbowy identyfikator konkretnej śledzonej trasy, a Sequence to TTL pakietu.

####Budowa pakietu ICMP Echo:
Nagłówek protokołu IP będzie budowany automatycznie, wykorzystując flagę IP_HDRINCL z API raw socket.
Nagłówek ICMP oraz dane będą budowane ręcznie w następujący sposób:
- Typ - typ wysyłanego komunikatu ICMP w naszym przypadku 08 - Echo request (1 oktet)
- Kod - podtyp wiadomości, naśladując MS Windows, program wpisuje kod 00 (1 oktet)
- Suma kontrolna - wyliczana na podstawie nagłówka pakietu ICMP oraz danych (2 oktety)
- Identyfikator - reprezentujący numer zadania, używany do identyfikacji odpowiadających pakietów (2 oktety)
- Numer sekwencji - reprezentujący TTL, jak identyfikator używany do identyfikacji odpowiadających pakietów (2 oktety)

Dane: 64 oktety zer.

####Wątek zarządcy
Wątek zarządcy przyjmuje z kolejki wejściowej zadania do wykonania umieszczone tam przez Moduł 1. Następnie powołuje do życia wątek wysyłający, wątek odbierający i czeka na koniec ich pracy, wywołując następnie metodę zapisu Modułu 3, która zapisze wyznaczoną trasę do pliku.

####Wątek wysyłający
Generuje za pomocą Generatora pakiety do wysłania, tworzy gniazdo i wysyła pakiety. Przy tworzeniu gniazda (korzystać z niego będzie również wątek odbierający) ustawia odpowiedni filtr, który ogranicza odbiór niepożądanych pakietów, np. ECHO_REQUEST.

####Wątek odbierający
Zastosowanie ICMP wraz z "raw socket" wymusza utworzenie jednego wątku odbierającego przez brak rozróżnienia portów. Jego zadaniem jest odbieranie wszystkich pakietów ICMP i ich interpretacja (możemy np. otrzymać pakiet zupełnie niezwiązany z zadaniem lub zdezaktualizowany).

####Komunikacja z Modułem 1
Kolejka std::queue zabezpieczona dodatkowo semaforem, przechowująca struktury z adresami do traceroutingu. Dzięki semaforowi i funkcji sem_wait(), wątek zarządcy zawiesza się na pustej kolejce i pobiera z niej nowy element dopiero, gdy taki zostanie do niej wstawiony.

####Komunikacja z Modułem 3
Komunikacja z Modułem 3 odbywa się w prosty sposób - wywoływana jest po prostu jego metoda zapisu rezultatu działań do pliku.

####Synchronizacja pomiędzy wątkami odbierającym i wysyłającym
Sygnał SIGUSR1 to polecenie "kontynuuj wysyłanie pakietów", wydawane wątkowi wysyłającemu przez wątek odbierający po odebraniu pakietu zidentyfikowanego jako odpowiedź na pakiet wysłany pobrany z kolejki.
Wątek wysyłający odbiera sygnał poprzez funkcję pselect() z odpowiednią maską sygnałów. Funkcja ta przerywa się przy wystąpieniu jednego z niezamaskowanych sygnałów.

####Warunki śmierci wątków
1. Wątek odbierający - wykonanie całej trasy.
2. Wątek odbierający - brak jakichkolwiek pakietów odebranych przez wątek odbierający przez czas określony w parametrach konfiguracyjnych. Gniazdo filtruje pakiety, więc próby wykonania operacji ping na naszym adresie nie będą powodować nieśmiertelności wątku.
3. Wątek wysyłający - przekroczenie TTL.
4. Wątek wysyłający - przekroczenie czasu oczekiwania na sygnał od wątku odbierającego.
3. Wątek wysyłający - śmierć wątku odbierającego.

####Algorytm pojedynczej operacji traceroute inicjowanej przez wątek zarządcy po pobraniu zadania:

1. Wątek zarządcy zapisuje dane dotyczące zadania w odpowiednich polach obiektu.

2. n = 1.

3. Wygeneruj za pomocą generatora pakiet o TTL = n, Identifier = numer zadania, Sequence = n, a następnie wyślij je.

4. Czekaj na sygnał od wątku odbierającego (funkcja pselect() o określonej wartości czasu aktywności timeout).
	Jeśli mamy kontynuować wysyłanie pakietów, n += 1 i wróć do punktu 4.
	Jeśli w czasie wyznaczonym przez parametr timeout nie otrzymano sygnału, wyślij kolejny pakiet o tym samym TTL i dekrementuj licznik możliwych powtórzeń max_packets_per_ttl dla danego TTL.
	Jeśli wyczerpano limit powtórzeń dla danego TTL, sprawdź, czy wątek odbierający żyje - jeśli tak, poczekaj dodatkowy czas na sygnał. Jeśli nie, zakończ pracę wątku wysyłającego.

5. Po zakończeniu traceroutingu wątek odbierający wykonuje metodę Modułu nr 3 przekazując wyznaczoną trasę lub jej fragment (struktura składająca się z nagłówka oraz listy adresów).

####Parametry dotyczące Modułu 2:

Parametry przechowywane są w pliku konfiguracyjnym programu razem z resztą parametrów dotyczących działania aplikacji.

max_ttl - domyślna wartość maksymalnego czasu życia pakietu.

max_packets_per_ttl - domyślna ilość pakietów wysyłanych do danego adresu z określoną wartościa TTL. Ze względu na brak gwarancji dostarczenia.

freq - częstotliwość wysyłania pakietów. Część zapór ogniowych może wykryć dużą ilość pakietów ICMP i zablokować dalszy ruch.

timeout - maksymalny czas oczekiwania na odpowiedź.
###Moduł 3
Moduł trzy obsługuje dostęp do systemu plikow.
####Interakcja z Modułem 1:
1. Odbiór żądania danych wynikowych:
	a. Brak gotowych.
	b. Sparsowanie danych i przesłanie do Modułu 1.

####Interakcja z Modułem 1:
1. Odebranie danych do zapisania:
	a. Stworzenie pliku w wersji roboczej(gdy nie wszystkie dane odebrane) - 'taskNr'+'r'+'.txt'
	b. Zmiana nazwy pliku z wersji roboczej na wersję gotową do odbioru  - 'taskNr'+'.txt'

####Interakcja z systemem plików:
1. Dodanie nowego zadania (utwórz plik z dopiskiem ze niegotowy).
2. Dodanie danych z zadania (otworzenie, zapis).
3. Wyciągnięcie informacji o stanie zadania:
	a. Nieskończone.
	b. Gotowe - parsowanie danych.
