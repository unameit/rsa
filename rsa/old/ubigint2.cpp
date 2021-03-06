#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <sys/types.h>
#include "ubigint.h"
#include "syserr.h"
#include "artm.h"

/* Niekt�re przekszta�cenia typ�w do udt s� niezb�dne, gdy� kompilator
 * (przynajmniej ten m�j Linuxowy) bardzo nie�adnie je obcina do int, a wi�c
 * z 64 bit�w robi si� 32, co oczywi�cie daje bzdurne wyniki.
 */

ostream& operator<<(ostream &os, const ubigint &b)
{
	const udt *p = b.fn0();
	udt m;
	os << *b.tail() << "\t\t";
	for(p = b.fn0(); p <= b.tail(); p++) {
		for(m = (udt) 1 << DT_BIT-1; m != 0; m >>= 1)
			os << char((m & *p)? '1': '0');
		os << ".";
	}
	return os;
}


ubigint::ubigint(udt n)
{
//	cerr << "ubigint::ubigint(" << n << ");\n";
	setmarkers();
	*tail() = n;
	memset(head(), 0, sizeb() - sizeof(udt));
}


ubigint ubigint::operator~(void) const
{
	ubigint tmp;
	const udt *p = head();
	udt *t = tmp.head();
	do {
		*t++ = ~*p++;
	} while(t <= tmp.tail());
	return tmp;
}


ubigint& ubigint::operator&=(const ubigint &b)
{
	udt *p = head();
	const udt *r = b.head();
	do {
		*p++ &= *r++;
	} while(p <= tail());
	return *this;
}


ubigint& ubigint::operator|=(const ubigint &b)
{
	udt *p = head();
	const udt *r = b.head();
	do {
		*p++ |= *r++;
	} while(p <= tail());
	return *this;
}


ubigint& ubigint::operator^=(const ubigint &b)
{
	udt *p = head();
	const udt *r = b.head();
	do {
		*p++ ^= *r++;
	} while(p <= tail());
	return *this;
}


int ubigint::cmp(const ubigint &b) const
{
	register const udt *p = head(), *r = b.head();
	for(; p <= tail(); ++p, ++r)
		if (*p != *r)
			break;
	return p > tail()? 0: *p - *r;
}


void ubigint::add(const ubigint &b)		/*  *this += b;  */
{
	register udt *p = 1 + tail();
	register const udt *r = 1 + b.tail();
	register udt tmp;
	register uint c = 0;			/* carry */
	do {
		if (c == 0) {
			*--p += *--r;
			c = *p < *r;
		} else {
			tmp = *--p + *--r + 1;
			c = *p >= (udt) ~*r;
			*p = tmp;
		}
	} while(p > head());
}


void ubigint::add(udt n)				/*  *this += n;  */
{
	register udt *p = tail();
	*p += n;
	if (*p-- < n)
		++*p;
	else return;
	while(*p == 0)
		++*--p;
	setmarkers();
}


void ubigint::sub(const ubigint &b)
{
	register udt *p = 1 + tail();
	register const udt *r = 1 + b.tail();
	register udt tmp;
	register uint c = 0;			/* borrow */
	do {
		tmp = *--p - *--r - c;
		c = c? *p <= *r: *p < *r;
		*p = tmp;
	} while(p > head());
}


void ubigint::sub(udt n)
{
	register udt *p = tail();
	register uint c = 0;			/* borrow */
	if (*p < n)
		c = 1;
	*p-- -= n;
	if (c == 0)
		return;
	while(*p == 0)
		--*p--;
	setmarkers();
}


ubigint& ubigint::operator>>=(register udt i)
{
	register udt *p, c = 0, d, mask;
//	cerr << "ubigint::operator>>=(" << i << ")\n";
	if (i >= DT_BIT) {
		div_t dv = div(i, DT_BIT);
		size_t j = dv.quot * sizeof(udt);
		i = dv.rem;
		memmove(head() + dv.quot, head(), sizeb() - j);
		memset(head(), 0, j);
		if (i == 0)
			return *this;
	}
	mask = ((udt) 1 << i) - (udt) 1;
	for(p = fn0(); p <= tail(); ++p) {
		d = *p & mask;
		*p >>= i;
		*p |= c << DT_BIT - i;
		c = d;
	}
	return *this;
}


ubigint& ubigint::operator<<=(register udt i)
{
	register udt *p, mask, c = 0, d;
//	cerr << "ubigint::operator<<=(" << i << ")\n";
	if (i >= DT_BIT) {
		div_t dv = div(i, DT_BIT);
		size_t j = dv.quot * sizeof(udt);
		i = dv.rem;
		memmove(head(), head() + dv.quot, sizeb() - j);
		memset(tail() + 1  - j, 0, j);
		if (i == 0)
			return *this;
	}
	mask = ~(( (udt) 1 << DT_BIT - i) - 1);
	for(p = tail(); p >= head(); --p) {
		d = *p & mask;
		*p <<= i;
		*p |= (udt) c >> (DT_BIT - i);
		c = d;
	}
	return *this;
}


void ubigint::bigdiv(ubigint b, ubigint &w)
{
	int i = fn0b(), j = b.fn0b();
	register k;
	if (j == -1)
		syserr("ubigint::div()\nDivision by zero!");
	w = 0;
	if (i == -1)
		return;
	if (i < j)
		return;
	k = i - j;
	b <<= k;
	while(k >= 0) {
		if (*this >= b) {
			sub(b);
			w.setbit(k);
		}
		b >>= 1;
		--k;
	}
}
/* *this = reszta z dzielenia,
 * w = wynik z dzielenia
 */


ubigint& ubigint::operator/=(const ubigint &b)
{
	ubigint tmp;
	bigdiv(b, tmp);
	return *this = tmp;
}


ubigint& ubigint::operator%=(const ubigint &b)
{
	ubigint tmp;
	bigdiv(b, tmp);
	return *this;
}


void ubigint::bigmul(const ubigint &b, ubigint &w)
{
	register i = fn0b();
	w = 0;
	while(i >= 0) {
		w <<= 1;
		if (testbit(i))
			w.add(b);
		--i;
	}
}


ubigint& ubigint::operator*=(const ubigint &b)
{
	ubigint tmp;
	bigmul(b, tmp);
	return *this = tmp;
}


void crypt(const ubigint &z, const ubigint &n, FILE *src, FILE *dst)
{
	ubigint b, w;
	size_t nobj;
	while(!feof(src)) {
		nobj = fread((char *) b.head() + b.sizeb() / 2, 1, b.sizeb() / 2, src);
		printf("Wczyta�em %d\n", nobj);
		if (ferror(src))
			syserr("fread");
		fastexp(b, z, n, w);
		if (nobj > fwrite((char *) w.head() + b.sizeb() / 2, 1, nobj, dst))
			syserr("fwrite");
	}
}


/* �wiadomie zaw�am do 8 bit�w Jak wiem z w�asnego do�wiadczenie, na
 * niekt�rych systemach RAND_MAX nie jest wcale du�e. St�d to
 * samo-ograniczenie do o�miu bit�w, by zawsze by�o ok.
 */ 

void bigrand(ubigint &b)
{
	register udt x, *p = b.head() + N_DT / 2; /* chcia�e� tylko po�ow� */
	register uint i;
	b = 0;
	for(; p <= b.tail(); ++p) {
		for(x = i = 0; i < sizeof(udt); ++i) {
			x <<= CHAR_BIT;
			x |= rand() & 0xFF;
		}
		*p = x;
	}
}
