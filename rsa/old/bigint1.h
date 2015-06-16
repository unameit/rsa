#ifndef	BIGINT_H
#define	BIGINT_H

#include <iostream.h>
#include <sys/types.h>
#include <string.h>

/* Program ten jest napisany tak, by w miar� trzyma� si� standardu, kt�rego
 * w�a�ciwie nie ma (tzn. chodzi o C++, bo przecie� jest standard ANSI C). 
 * Typ 'long long' nie jest (chyba) jeszcze w �adnym standardzie, wi�c,
 * je�li dany kompilator go nie 'prze�knie' mo�na bez obaw zast�pi� go tu
 * typem 'long'.
 */


/* Wog�le nas nieobchodz� nadmiary i przepe�nienia. Robimy tak, jak to jest
 * normalnie w arytmetyce ca�kowito-liczbowej w kompach, czyli bez sprawdzania
 * i komunikat�w. Je�li zaczn� wychodzic g�upoty tzn., �e nale�y zwi�kszy�
 * sta�� DIGITS
 */

#define	DIGITS	20U


class bigint {
	friend ostream& operator <<(ostream&, const bigint&);
	friend void rand(bigint&);
	friend void random(const bigint &seed, bigint&);
public:
	bigint(void) { setmarkers(); n = 0; }
	bigint(long long);
	bigint(const char *);

	bigint& operator ++(void);
	bigint& operator --(void);

	bigint operator ++(int);
	bigint operator --(int);

	bigint& operator +=(const bigint&);
	bigint& operator -=(const bigint&);

	bigint operator +(const bigint&) const;
	bigint operator -(const bigint&) const;

	bigint& operator *=(const bigint&);
	bigint& operator /=(const bigint&);
	bigint& operator /=(int);

	bigint& operator %=(const bigint&);

	bigint operator *(const bigint&) const;
	bigint operator /(const bigint&) const;
	bigint operator /(int) const;
	bigint operator %(const bigint&) const;

	int operator ==(const bigint &b) const	{ return cmp(b) == 0; }
	int operator !=(const bigint &b) const	{ return cmp(b) != 0; }
	int operator >(const bigint &b) const	{ return cmp(b) > 0; }
	int operator <(const bigint &b)	const	{ return cmp(b) < 0; }
	int operator >=(const bigint &b) const	{ return cmp(b) >= 0; }
	int operator <=(const bigint &b) const	{ return cmp(b) <= 0; }

	int odd(void) const	{ return *(last()) & 1; }

	void bigdiv(const bigint&, bigint&);
	int cmp(const bigint&) const;
private:
	char d[DIGITS+2];	/* digits */	/* +2 na markery ko�ca i pocz�tku */
	unchar n;			/* negative */

	void setmarkers(void)	{ d[0] = d[sizeof d - 1] = 1; }
	size_t size(void) const	{ return sizeof d - 2; }	/* rozmiar ca�ej tablcy */
	char *first(void)	{ return d + 1; }	/* zostawiamy miejsce na markery ko�ca */
	char *last(void)	{ return d + sizeof d - 1 - 1; }

	const char *first(void)	const { return d + 1; }
	const char *last(void)	const { return d + sizeof d - 1 - 1; }

	uint base(void) const { return 10; }

/* czyli:
 * pierwszy wolny, kt�ry mo�na u�ywa�, to first()
 * ostatni wolny, to last();
 * czyli wszystkie s� zawarte:  first() <= digit <= last();
 * nal꿱cy jeszcze do tablicy, ale ju� nie do bezpo�redniego u�ycia jest last()+1;
 */
	void nrm1p(char *);
	void nrm1m(char *);
	void nrm1oncep(char *);
	void nrm1oncem(char *);
	void nrmallp(char *);
	void nrmallm(char *);
	void nrmalloncep(char *);
	void nrmalloncem(char *);

	void pre(const bigint &, unchar);
	void pre(const bigint&b) { pre(b, b.n); }

	void add(const bigint&);
	void sub(const bigint&);
};


#endif
