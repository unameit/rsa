#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <sys/types.h>
#include "syserr.h"
#include "bigint.h"


ostream& operator <<(ostream &outs, const bigint &b)
{
	const char *p = b.first();
	while(*p++ == 0);
	--p;
	if (p <= b.last()) {
		if (b.n == 1)
			outs << "-";
		do {
			outs << unchar('0' + *p++);
		} while(p <= b.last());
	} else outs << "0";
	return outs;
}

/* UWAGA: tu porzebujemy tylko po³owê dostêpnych cyfr, wiêæ tak w³a¶nie
 * modyfikujê rand
 */
void rand(bigint &r)
{
	char *p = r.last(), *k = r.first() + r.size() / 2;
	r = "0";
	while(p >= k) {
		*p-- = rand() % r.base();
	}
	r.n = 0;
//	memset(r.first(), 0, r.size() - (r.last() - k));
	memset(r.first(), 0, k+1-r.first());
}

/* Po to porównujemy pamiêæ, by nie dzieliæ. Niestety dzielenie jest cholernie
 * wolne
 */
void random(const bigint &seed, bigint &r)
{
	char *p;
	r = "0";
	for(p = r.last(); p >= r.first(); p--) {
		*p = rand() % r.base();
		if (memcmp(r.first(), seed.first(), r.size()) >= 0) {
			*p = 0;
			break;
		}
	}
}


/* Musimy normalizowaæ, ¿eby nie wyst±pi³o przepe³nienie na char'ach, i ¿eby¶my
 * nie dostali potem g³upot.
 */

/* Normalizujemy tylko jedn± 'cyfrê' i tylko jeden raz. Tzn. spodziewamy siê,
 * ¿e normalizowana 'cyfra' bêdzie co nawy¿ej równa 19
 */
inline void bigint::nrm1oncep(char *p)
{
	if (*p > 9) {
		*p-- -= base();
		++*p;
	}
}


inline void bigint::nrm1oncem(char *p)
{
	if (*p < 0) {
		*p-- += base();
		--*p;
	}
}


/* Normalizujemy jedn± 'cyfrê', ale zak³adamy, ¿e prawdopodobnie jest
 * wiêksza od 19, czyli normalizowaæ bêdziemy j± kilkukrotnie. (Chyba
 * bardziej siê op³aca kilka razy odj±æ, ni¿ dwa razy dzieliæ? - SPRAWD¬!)
 */
inline void bigint::nrm1p(char *p)
{
	--p;
	while(p[1] > 9) {
		p[1] -= base();
		++*p;
	}
}


inline void bigint::nrm1m(char *p)
{
	--p;
	while(p[1] < 0) {
		p[1] += base();
		--*p;
	}
}


/* Normalizujemy wszystkie pozycje, pocz±wszy od p, ale tylko jednokrotnie,
 * czyli warunek, ¿e 'cyfry' musz± byæ mniejsze od 20
 */
inline void bigint::nrmalloncep(char *p)
{
	while(*p > 9) {
		*p-- -= base();
		++*p;
	}
}


inline void bigint::nrmalloncem(char *p)
{
	while(*p < 0) {
		*p-- += base();
		--*p;
	}
}


/* Normalizujemy wszystkie pozycje, pocz±wszy od p */
inline void bigint::nrmallp(char *p)
{
	for(--p; p[1] > 9; p--)
		do {
			p[1] -= base();
			++*p;
		} while(p[1] > 9);
}


inline void bigint::nrmallm(char *p)
{
	for(p--; p[1] < 0; p--)
		do {
			p[1] += base();
			--*p;
		} while(p[1] < 0);
}


/* Je¿eli DIGITS-2 > 20, to mo¿na bezpiecznie zapisywaæ liczby > 64 bitów
 * bez sprawdzania, czy siê nie skoñczy³a tablica na cyfry.
 */
bigint::bigint(long long i)
{
	char *k = last();

//	cerr << "bigint::bigint(" << i << ");\n";		/* diagnostyka */

	setmarkers();

	if (i < 0) {		/* zbadaj czy ujemna */
		i = -i;			/* je¶li tak, zrób z ni± porz±dek */
		n = 1;
	} else n = 0;

 	for(; i != 0; i /= base())
		*k-- = i % base();

	if (k >= first())
		memset(first(), 0, size() - (last() - k));
}


bigint::bigint(const char *s)
{
	size_t len = strlen(s);
	char *k = last();

//	cerr << "bigint::bigint(\"" << s << "\")\n";		/* diagnostyka */

	if (len > size())
		syserr("bigint::bigint(const char *);\nString length > %u", size());

	setmarkers();

	if (*s == '-') {
		n = 1;
		s++;
		len--;
	} else n = 0;

	k -= len-1;
	while(*s != '\0')
		if (isdigit(*s))
			*k++ = *s++ - '0';
		else syserr("bigint::bigint(const char *)\nNumber with non-digits!");
 
	memset(first(), 0, size() - len);
}


bigint& bigint::operator ++(void)		/* prefix */
{
	char *p = last();
	if (n == 0) {
		++*p;
		nrmalloncep(last());
	} else {
		--*p;
		nrmalloncem(last());
	}
	return *this;
}


bigint& bigint::operator --(void)		/* prefix */
{
	char *p = last();
	if (n == 0) {
		--*p;
		nrmalloncem(last());
	} else {
		++*p;
		nrmalloncep(last());
	}
	return *this;
}


bigint bigint::operator ++(int)			/* postfix */
{
	bigint tmp = *this;
	operator++();
	return tmp;
}


bigint bigint::operator --(int)			/* postfix */
{
	bigint tmp = *this;
	operator--();
	return tmp;
}


bigint& bigint::operator +=(const bigint &b)
{
	pre(b);
	return *this;
}


bigint& bigint::operator -=(const bigint &b)
{
	pre(b, 1 ^ b.n);
	return *this;
}


bigint bigint::operator +(const bigint &b) const
{
	bigint tmp = *this;
	return tmp += b;
}


bigint bigint::operator -(const bigint &b) const
{
	bigint tmp = *this;
	return tmp -= b;
}


bigint bigint::operator *(const bigint &b) const
{
	bigint tmp = *this;
	return tmp*=b;
}


bigint bigint::operator %(const bigint &b) const
{
	bigint tmp = *this;
	return tmp %= b;
}

bigint bigint::operator /(const bigint &b) const
{
	bigint tmp =*this;
	return tmp /= b;
}

/* Wynik mno¿enia dwóch liczb n-cyfrowych zajmie maksymalnie 2n cyfr.
 * I NIE WIÊCEJ!
 * Przyk³ad: 9*9=81, 99*99=9801, 999*999=998001
 */

bigint& bigint::operator *=(const bigint &b)
{
	uint i, j;
	char tmp[2 + 2 * size()];	/* sporo miejsca, bo to przecie¿ mno¿enie */

	memset(tmp, 0, sizeof tmp);

/* uwzglêdniam tu oczywi¶cie markery, ale w sposób jawny */
/* tmp[..-2] bo [2] to minimalny osi±galny index */
	for(j = size()+1; j >= 1; j--) {
		for(i = size()+1; i >= 1; i--) {
			tmp[i+j-2] += d[j] * b.d[i];
			nrmallp(&tmp[i+j-2]);	/* tu mo¿na wiele zoptymalizowaæ... */
		}
	}
	setmarkers();
	memcpy(first(), tmp + sizeof tmp - size(), size());
	n ^= b.n;
	return *this;
}


void bigint::bigdiv(const bigint &b, bigint &r)
{
	for(r = "0"; memcmp(first(), b.first(), size()) >= 0; ++r)
		sub(b);
	r.n ^= b.n;	/* znak wyniku dzielenia */
}
/* r - wynik dzielenia
 * this - reszta z dzielenia
 */


bigint& bigint::operator /=(const bigint &b)
{
	bigint tmp;
	bigdiv(b, tmp);
	return *this = tmp;
}


/* Metoda 'pisemna' dla ma³ych liczb */
bigint& bigint::operator /=(int a)
{
	if (abs(a) < base()) {

		bigint tmp;
		div_t dr;
		char *p = first(), *t = tmp.first();

		tmp.n = n ^ a < 0;
		a = abs(a);

		while(*p++ == 0)
			*t++ = 0;
		--p;
		while(p <= last()) {
			dr = div(*p++, a);
			*t++ = dr.quot;
			*p += dr.rem * base();
		}
		tmp.setmarkers();
		return *this = tmp;				
	} else {
		bigint tmp(a);
		return *this /= tmp;		
	}
}


/* Metoda 'pisemna' dla ma³ych liczb */
bigint bigint::operator /(int a) const
{
	if (abs(a) < base()) {

		bigint tmp;
		div_t dr;
		const char *p = first();
		char *t = tmp.first();
		int tpn;

		tmp.n = n ^ a < 0;
		a = abs(a);

		while(*p++ == 0)
			*t++ = 0;

		tpn = *--p;
		while(p <= last()) {
			dr = div(tpn, a);
			*t++ = dr.quot;
			tpn = *++p + dr.rem * base();
		}
		tmp.setmarkers();
		return tmp;				
	} else {
		bigint tmp(a);
		return *this / tmp;		
	}
}


bigint& bigint::operator %=(const bigint &b)
{
	bigint tmp;
	bigdiv(b, tmp);
	return *this;
}


int bigint::cmp(const bigint &b) const
{
	if (n != b.n)
		return b.n - n;
	return memcmp(first(), b.first(), size());
}


/* ngtv to to samo co b.n, tylko, ¿e przekazane wprost, bo czasem (a
 * nawet czêsto) zachodzi potrzeba zmiany tego pola. Wiêc po co kopiowaæ
 * ca³± bigint, je¶li mo¿na tylko jedno pole.
 */
/* Zapis tej funkcji przekracza moje mo¿liwo¶ci opisowe za pomoc± klawiatury.
 * Za to na kartce papieru nie bêdzie ¿adnych problemów z wyja¶nieniem
 * dzia³ania tej funkcji.
 */
void bigint::pre(const bigint &b, unchar ngtv)
{
	if (n != ngtv) {
		if (memcmp(first(), b.first(), size()) < 0) {
			bigint tmp = b;
			tmp.sub(*this);
			*this = tmp;
			n ^= 1;
		} else sub(b);
	} else add(b);
}


/* Dodaje b do this, nie zwracaj±c wagi na n,
 * tzn, trakuje liczby, jak unsigned
 */
void bigint::add(const bigint &b)	/*  *this += b;  */
{
	const char *r = b.last();
	char *p = last();
	do {
		*p += *r--;
		nrm1oncep(p--);
	} while(p >= first());
	setmarkers();	/* bo móg³ je nadpisaæ podczas przenoszenia nadmiaru */
}


void bigint::sub(const bigint &b)
{
	const char *r = b.last();
	char *p = last();
	do {
		*p -= *r--;
		nrm1oncem(p--);
	} while(p >= first());
	setmarkers();
}
