1. Informacje wst�pne

Przedmiot:	Laboratorium C/C++
Prowadz�cy:	J�drzej Byrski <wbyrski@uci.agh.edu.pl>

Projekt:	Szyfrowanie algorytmem RSA
Autor:		Micha� Jacykiewicz <yac@icslab.agh.edu.pl>

2. Kompilacja i uruchamianie
Program kompiluje si� przy pomocy standardowego narz�dzia systemu UNIX,
jakim jest `make`. Kod wynikowy generowany jest do pliku `prog`.  Lista
dost�pnych opcji podawana jest po uruchomienu bez argument�w.
Szyfrowa� mo�na za pomoc� oboj�tne kt�rego klucza, jednak nale�y zachowa�
kolejno��: szyfrowanie -e, deszyforwanie -d. Zwi�zane jest to z sposobem
czytania i zapisu danych. Nie utrudnia to bynajmniej szyfrowania podw�jengo,
tj. np. moim kluczem prywatnym, a nast�pnie drugiej osoby kluczem publicznym
(�adnie to mo�na zrobi� w potoku).

3. U�yte algorytmy
Wi�kszo�� algorytm�w zaczerpni�ta zosta�a z ksi�zki:
 D.E.Robling Denning "Kryptografia i ochrona danych"
 WNT Warszawa 1992,1993  ISBN 83-204-1658-2
Pozosta�e s� albo powszechnie znane, albo moje w�asne, lub te� cudze, lecz z
moimi modyfikacjami. Wi�cej informacji na ten temat u autora.

4. Zmiany zwi�zane z d�ugosci� klucza
Kod zostal tak napisany, by wszelkie zmiany d�ugo�ci klucza, jak i
pojedynczych porcji danych mo�na by�o wykona� zmieniaj�c jedn� (dwie) sta�e
zdefiniowane w pliku "ubigint.h". S�u�y do tego celu g��wnie sta�a DIGITS
oraz ewentualnie typedef dt i udt. Prosz� nie wprowadza� modyfikacji w
innych miejscach, ze wzgl�du na poprawno�� dzia�ania.

5. Przeprowadzone testy
Program by testowany na Linux i586 oraz sun4m sparc SUNW,SPARCstation-10. Na
obydwu tych maszynah zachowywa� si� prawid�owo.
Na AIXie si� nie kompiluje:
"ubigint.h", line 122.21: 1540-038: (S) The array bound must be a positive integral constant expression.
Nie. Jednak si� skompilowa�o. Wy�wietli� mas� b��d�w, ale kod jest. Dzia�a
cholernie wolno.

6. B��dy wynikajace z nadmiar�w/przepe�nie�.
Nie sa wykrywane - zachowanie identyczne jak w normalnej arytmetyce
ca�kowito-liczbowej komputera.

7. Inne uwagi:
Wygl�da na to, �e Linux ma o wiele gorszy generator liczb losowych - s� o
wiele mniej pierwsze ni� te na SUNie i w zwi�zku z tym czas oczekiwania na
generacj� klucza jest o wiele d�u�szy.

8. 32bity vs 64bity
Zachowanie na r�nych maszynach jest naprawd� r�ne. Najlepiej sobie
przetestowa�. W pliku "ubigint.h" wystraczy zmieni� "#if 0" na "#if 1", i na
odwr�t.

9. Przebieg pracy nad projektem
Prosz� zobaczy� do katalogu old.

Najlepiej uruchomi� tak:
% ./prog -g
% cat txts/* | ./prog -e - - | ./prog -d - -


Krak�w, kwiecie�-czerwiec 1998.
