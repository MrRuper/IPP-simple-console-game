# Zadanie gra, część 1

Tegoroczne zadanie polega na zaimplementowaniu gry. Grę rozgrywa się na prostokątnej planszy składającej się z jednakowych kwadratowych pól. 
Pola sąsiadujące na planszy to takie, które stykają się bokami. Pól stykających się tylko rogami nie uważamy za sąsiadujące. Pola tworzą obszar, jeśli z każdego z nich można dojść do innego, przechodząc jedynie przez pola sąsiadujące. Pojedyncze pole też jest obszarem. Grać może jedna lub więcej osób. Na początku gry plansza jest pusta. Gracze kolejno rzucają tradycyjną sześcienną kostką do gry. Każdy gracz zajmuje co najwyżej tyle pól, stawiając na nich swój pionek, ile oczek wypadło na kostce. Gracz może zajmować dowolne niezajęte pola, przestrzegając jedynie zasady, że zbiór pól zajętych przez tego samego gracza nie może w żadnym momencie składać się z więcej niż, będącej parametrem gry, maksymalnej liczby obszarów. Gracz, który nie jest w stanie wykonać ruchu zgodnego z powyższymi zasadami, wypada z gry. Gra kończy się, gdy już żaden gracz nie może wykonać ruchu. Wygrywa gracz, który zajmuje największą liczbę pól.
Jako pierwszą część zadania należy zaimplementować w języku C moduł „silnika” gry. Opis interfejsu modułu znajduje się w pliku **game.h** w formacie komentarzy dla programu **doxygen**. Przykład użycia znajduje się w pliku **game_example.c**.

Jako rozwiązanie należy dostarczyć: - plik **game.h** z definicją interfejsu silnika gry, - plik **game.c** z implementacją silnika gry, - ewentualnie inne pliki *.h i *.c z implementacją, - plik **game_example.c** zawierający funkcję main, - plik makefile domyślnie tworzący plik wykonywalny **game**.

Zaleca się kompilowanie za pomocą polecenia gcc z parametrami

```
-Wall -Wextra -Wno-implicit-fallthrough -std=c17 -O2
```

Polecenie **make clean** powinno usuwać wszystkie pliki stworzone poleceniem **make**.

Zawartość pliku **game_example.c** nie będzie oceniana – można w nim umieścić własne testy.

Rozwiązanie powinno się kompilować i uruchamiać w laboratorium studenckim i na maszynie students.

# Ocena

Za w pełni poprawne rozwiązanie zadania implementujące wszystkie wymagane funkcjonalności można zdobyć maksymalnie 20 punktów. Od tej oceny będą odejmowane punkty za poniższe uchybienia:
- Za problemy z kompilowaniem rozwiązania i z **makefile** można stracić wszystkie punkty.
- Za usterki w sprawdzaniu poprawności parametrów wywołania programu można stracić do 2 punktów.
- Za brak komunikatów o błędach lub niejasne komunikaty można stracić do 2 punktów.
- Za „wrażenia nieartystyczne” z rozgrywki można stracić do 6 punktów.
- Za każdy test (silnika gdy), którego program nie przejdzie, traci się do 1 punktu.
- Za problemy z zarządzaniem pamięcią można stracić do 6 punktów.
- Za niezgodną ze specyfikacją strukturę plików w rozwiązaniu, niezgodne ze specyfikacją nazwy plików w rozwiązaniu lub umieszczenie w paczce niepotrzebnych albo tymczasowych plików można stracić do 2 punktów.
- Za złą jakość kodu, brzydki styl kodowania można stracić do 4 punktów.
- Za ostrzeżenia wypisywane przez kompilator można stracić do 2 punktów.
- Za braki w dokumentacji można stracić do 2 punktów.

Za w pełni poprawne rozwiązanie zadania implementujące wszystkie wymagane funkcjonalności można zdobyć maksymalnie 20 punktów. Od tej oceny będą odejmowane punkty za poniższe uchybienia:

- Za problemy z kompilowaniem rozwiązania i z makefile można stracić wszystkie punkty.
- Za każdy test, którego program nie przejdzie, traci się do 1 punktu.
- Za problemy z zarządzaniem pamięcią można stracić do 6 punktów.
- Za niezgodną ze specyfikacją strukturę plików w rozwiązaniu, niezgodne ze specyfikacją nazwy plików w rozwiązaniu lub umieszczenie w repozytorium niepotrzebnych albo tymczasowych plików można stracić do 4 punktów.
- Za złą jakość kodu, brzydki styl kodowania można stracić do 4 punktów.
- Za ostrzeżenia wypisywane przez kompilator można stracić do 2 punktów.
- Za braki w dokumentacji można stracić do 2 punktów.
