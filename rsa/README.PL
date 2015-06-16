1. Informacje wstêpne

Przedmiot:	Laboratorium C/C++
Prowadz±cy:	Jêdrzej Byrski <wbyrski@uci.agh.edu.pl>

Projekt:	Szyfrowanie algorytmem RSA
Autor:		Micha³ Jacykiewicz <yac@icslab.agh.edu.pl>

2. Kompilacja i uruchamianie
Program kompiluje siê przy pomocy standardowego narzêdzia systemu UNIX,
jakim jest `make`. Kod wynikowy generowany jest do pliku `prog`.  Lista
dostêpnych opcji podawana jest po uruchomienu bez argumentów.
Szyfrowaæ mo¿na za pomoc± obojêtne którego klucza, jednak nale¿y zachowaæ
kolejno¶æ: szyfrowanie -e, deszyforwanie -d. Zwi±zane jest to z sposobem
czytania i zapisu danych. Nie utrudnia to bynajmniej szyfrowania podwójengo,
tj. np. moim kluczem prywatnym, a nastêpnie drugiej osoby kluczem publicznym
(³adnie to mo¿na zrobiæ w potoku).

3. U¿yte algorytmy
Wiêkszo¶æ algorytmów zaczerpniêta zosta³a z ksi±zki:
 D.E.Robling Denning "Kryptografia i ochrona danych"
 WNT Warszawa 1992,1993  ISBN 83-204-1658-2
Pozosta³e s± albo powszechnie znane, albo moje w³asne, lub te¿ cudze, lecz z
moimi modyfikacjami. Wiêcej informacji na ten temat u autora.

4. Zmiany zwi±zane z d³ugosci± klucza
Kod zostal tak napisany, by wszelkie zmiany d³ugo¶ci klucza, jak i
pojedynczych porcji danych mo¿na by³o wykonaæ zmieniaj±c jedn± (dwie) sta³e
zdefiniowane w pliku "ubigint.h". S³u¿y do tego celu g³ównie sta³a DIGITS
oraz ewentualnie typedef dt i udt. Proszê nie wprowadzaæ modyfikacji w
innych miejscach, ze wzglêdu na poprawno¶æ dzia³ania.

5. Przeprowadzone testy
Program by testowany na Linux i586 oraz sun4m sparc SUNW,SPARCstation-10. Na
obydwu tych maszynah zachowywa³ siê prawid³owo.
Na AIXie siê nie kompiluje:
"ubigint.h", line 122.21: 1540-038: (S) The array bound must be a positive integral constant expression.
Nie. Jednak siê skompilowa³o. Wy¶wietli³ masê b³êdów, ale kod jest. Dzia³a
cholernie wolno.

6. B³êdy wynikajace z nadmiarów/przepe³nieñ.
Nie sa wykrywane - zachowanie identyczne jak w normalnej arytmetyce
ca³kowito-liczbowej komputera.

7. Inne uwagi:
Wygl±da na to, ¿e Linux ma o wiele gorszy generator liczb losowych - s± o
wiele mniej pierwsze ni¿ te na SUNie i w zwi±zku z tym czas oczekiwania na
generacjê klucza jest o wiele d³u¿szy.

8. 32bity vs 64bity
Zachowanie na ró¿nych maszynach jest naprawdê ró¿ne. Najlepiej sobie
przetestowaæ. W pliku "ubigint.h" wystraczy zmieniæ "#if 0" na "#if 1", i na
odwrót.

9. Przebieg pracy nad projektem
Proszê zobaczyæ do katalogu old.

Najlepiej uruchomiæ tak:
% ./prog -g
% cat txts/* | ./prog -e - - | ./prog -d - -


Kraków, kwiecieñ-czerwiec 1998.
