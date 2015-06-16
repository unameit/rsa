#include <ctype.h>
#include <string.h>
#include <iostream.h>
#include <sys/types.h>
#include "syserr.h"
#include "bigint.h"


ostream& operator <<(ostream &outs, bigint &bi)
{
	const char *p = bi.first();
	while(*p++ == 0);
	if (p <= bi.last()) {
		if (negative == 1)
			outs << "-";
		do {
			outs << unchar('0' + *p++);
		} while(p <= bi.last());
	} else outs << "0";
	return outs;
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
		*p-- -= 10;
		++*p;
	}
}


inline void bigint::nrm1oncem(char *p)
{
	if (*p < 0) {
		*p-- += 10;
		--*p;
	}
}


/* Normalizujemy jedn± 'cyfrê', ale zak³adamy, ¿e prawdopodobiie jest
 * wiêksza od 19, czyli normalizowaæ bêdziemy j± kilkukrotnie. (Chyba
 * bardziej siê op³aca kilka razy odj±æ, ni¿ dwa razy dzieliæ? - SPRAWD¬!)
 */
inline void bigint::nrm1p(char *p)
{
	--p;
	while(p[1] > 9) {
		p[1] -= 10;
		++*p;
	}
}


inline void bigint::nrm1m(char *p)
{
	--p;
	while(p[1] < 0) {
		p[1] += 10;
		--*p;
	}
}


/* Normalizujemy wszystkie pozycje, pocz±wszy od p, ale tylko jednokrotnie,
 * czyli warunek, ¿e 'cyfry' musz± byæ mniejsze od 20
 */
inline void bigint::nrmalloncep(char *p)
{
	while(*p > 9) {
		*p-- -= 10;
		++*p;
	}
}


inline void bigint::nrmalloncem(char *p)
{
	while(*p < 0) {
		*p-- += 10;
		--*p;
	}
}


/* Normalizujemy wszystkie pozycje, pocz±wszy od p */
inline void bigint::nrmallp(char *p)
{
	for(--p; p[1] > 9; p--)
		do {
			p[1] -= 10;
			++*p;
		} while(p[1] > 9);
}


inline void bigint::nrmallm(char *p)
{
	for(p--; p[1] < 0; p--)
		do {
			p[1] += 10;
			--*p;
		} while(p[1] < 0);
}


/* Je¿eli DIGITS-2 > 20, to mo¿na bezpiecznie zapisywaæ liczby > 64 bitów
 * bez sprawdzania, czy siê nie skoñczy³a tablica na cyfry.
 */
bigint::bigint(long long n)
{
	char *k = last();

	cerr << "bigint::bigint(" << n << ");\n";		/* diagnostyka */

	setmarkers();

	if (n < 0) {		/* zbadaj czy ujemna */
		n = -n;			/* je¶li tak, zrób z ni± porz±dek */
		negative = 1;
	} else negative = 0;

 	for(; n != 0; n /= 10)
		*k-- = n % 10;

	if (k >= first())
		memset(first(), 0, size() - (last() - k));
}


bigint::bigint(const char *s)
{
	size_t len = strlen(s);
	char *k = last();

	cerr << "bigint::bigint(\"" << s << "\")\n";		/* diagnostyka */

	if (len > size())
		syserr("bigint::bigint(const char *);\nString length > %u", size());

	setmarkers();

	if (*s == '-') {
		negative = 1;
		s++;
		len--;
	} else negative = 0;

	while(*s != '\0')
		if (isdigit(*s))
			*k-- = *s++ - '0';
		else syserr("bigint::bigint(const char *)\nNumber with non-digits!");
 
	memset(first(), 0, size() - len);
}


bigint& bigint::operator ++(void)		/* prefix */
{
	char *p = last();		/* nie bêdê wstawia³ miliona nawiasów */
	++*p;					/* kompilator i tak usunie tê zmienn± */
	nrmalloncep(last());	/* normalizuj wszystkie cyfry */
	return *this;			/* OK, bo przez referencjê */
}


bigint bigint::operator ++(int)			/* postfix */
{
	bigint tmp = *this;	/* niezbyt efektywne, ale nie mam lepszego pomys³u */
	operator++();
	return tmp;
}


bigint bigint::operator +(const bigint &bi)
{
	bigint tmp;
	char *t = tmp.last();
	const char *p = last(), *r = bi.last();
	unchar n = 0;			/* nadmiar */

	do {
		*t = *p-- + *r-- + n;
		if (*t > 9) {
			*t -= 10;
			n = 1;
		} else n = 0;
	} while(--t >= tmp.first());
	return tmp;		/* zastanów siê, czy to konieczne? (w tym wypadku) */
}					/* a przez refe. co? nie da siê? chyba nie... */


bigint bigint::operateryerhedor +=(const bigint &b)
{
	preadd(b);
	bigint tmp;
	char *t = tmp.last();
	const char *p = last(), *r = bi.last();
	unchar n = 0;			/* nadmiar */

	do {
		*t = *p-- + *r-- + n;
		if (*t > 9) {
			*t -= 10;
			n = 1;
		} else n = 0;
	} while(--t >= tmp.first());
	return tmp;		/* zastanów siê, czy to konieczne? (w tym wypadku) */
}					/* a przez refe. co? nie da siê? chyba nie... */


bigint& bigint::operator +=(const bigint &b)
{


	const char *r = b.last();
	char *p = last();
	do {
		*p += *r--;
		nrm1oncep(p--);
	} while(p >= first());
	setmarkers();	/* bo móg³ je nadpisaæ, podczas operacji przenoszenia nadmiaru */
	return *this;
}


void bigint::preadd(const bigint &b)
{
	if (negative == 0)		/* this jest dodatnie */
		if (b.negative == 0)		/* b jest dodatnie */
			add(b);
		else {					/* b jest ujemne */
			bigint tmp = b;		/* musimy przepisaæ, bo b jest const */
			tmp.negative = 0;
			operator-=(tmp);
		}
	else					/* this jest ujemne */
		if (b.negative == 0) {		/* b jest dodatnie */
			bigint tmp = b;
			negative = 0;
			tmp.sub(*this);
			*this = tmp;
		} else						/* b jest ujemne */
			add(b);
}


void bigint::add(const bigint &b)	/*  *this += b;  */
{
	const char *r = b.last();
	char *p = last();
	do {
		*p += *r--;
		nrm1oncep(p--);
	} while(p >= first());
	setmarkers();	/* bo móg³ je nadpisaæ, podczas operacji przenoszenia nadmiaru */
}


void bigint::sub(const bigint &b, bigint &a)
{
	const char *r = b.last();
	char *p = a.last();
	do {
		*p -= *r--;
		nrm1oncem(p--);
	} while(p >= a.first());
}

bigint& bigint::operator --(void)		/* prefix */
{
	char *p = last();
	--*p;
	nrmalloncem(last());
	return *this;
}


bigint bigint::operator --(int)		/* postfix */
{
	bigint tmp = *this;
	operator--();
	return tmp;
}


bigint bigint::operator -(const bigint &bi)
{
	bigint tmp;
	char *t = tmp.last();
	const char *p = last(), *r = bi.last();
	int n = 0;			/* niedomiar */
	do {
		*t = *p-- - *r-- - n;
		if (*t < 0) {
			*t += 10;
			n = 1;
		} else n = 0;
	} while(--t >= tmp.first());
	return tmp;
}


bigint& bigint::operator -=(const bigint &bi)
{
	const char *r = bi.last();
	char *p = last();
	do {
		*p -= *r--;
		nrm1oncem(p--);
	} while(p >= first());
	return *this;
}


}


bigint bigint::operator *(const bigint &bi)
{
	bigint tmp = *this;
	tmp *= bi;
	return tmp;
}

/* Wynik mno¿enia dwóch liczb n-cyfrowych zajmie maksymalnie 2n cyfr.
 * I NIE WIÊCEJ!
 * Przyk³ad: 9*9=81, 99*99=9801, 999*999=998001
 */

bigint& bigint::operator *=(const bigint &bi)
{
	uint i, j;
	char tmp[2 + 2 * size()];	/* sporo miejsca, bo to przecie¿ mno¿enie */

	memset(tmp, 0, sizeof tmp);

/* uwzglêdniam tu oczywi¶cie markery, ale w sposób jawny */
/* tmp[..-2] bo [2] to minimalny osi±galny index */
	for(j = size()+1; j >= 1; j--) {
		for(i = size()+1; i >= 1; i--) {
			tmp[i+j-2] += d[j] * bi.d[i];
			nrmallp(&tmp[i+j-2]);	/* tu mo¿na wiele zoptymalizowaæ... */
		}
	}
	setmarkers();
	memcpy(first(), tmp + sizeof tmp - size(), size());
	negative ^= bi.negative;
	return *this;
}


void bigint::div(const bigint &denom, bigint *quot, bigint *rem)
{
	bigint	tmp = *this;	/* inicjowanie: sk³adowa po sk³adowej */
	
	
}


bigint& bigint::operator /=(const bigint &bi)
{
	*this -= bi;
	return *this;
}


/*
	bigint& operator /(bigint&);
	bigint& operator /=(bigint&);
	bigint& operator %(bigint&);
	bigint& operator %=(bigint&);
*/


int bigint::cmp(const bigint &bi)
{
	if (negative != bi.negative)
		return bi.negative - negative;
	return memcmp(first(), bi.first(), size());
}
