#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <sys/types.h>
#include "ubigint.h"
#include "syserr.h"
#include "fastexp.h"

/* Niektóre przekszta³cenia typów do udt s± niezbêdne, gdy¿ kompilator
 * (przynajmniej ten mój Linuxowy) bardzo nie³adnie je obcina do int, a wiêc
 * z 64 bitów robi siê 32, co oczywi¶cie daje bzdurne wyniki.
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


ubigint ubigint::operator~(void) const
{
	ubigint tmp;
	register const udt *p = head();
	register udt *r = tmp.head();
	do {
		*r++ = ~*p++;
	} while(r <= tmp.tail());
	return tmp;
}


void ubigint::neg(void)
{
	register udt *p = head();
	do {
		*p = ~*p;
		++p;
	} while(p <= tail());
}


ubigint& ubigint::operator&=(const ubigint &b)
{
	register udt *p = head();
	register const udt *r = b.head();
	do {
		*p++ &= *r++;
	} while(p <= tail());
	return *this;
}


ubigint& ubigint::operator|=(const ubigint &b)
{
	register udt *p = head();
	register const udt *r = b.head();
	do {
		*p++ |= *r++;
	} while(p <= tail());
	return *this;
}


ubigint& ubigint::operator^=(const ubigint &b)
{
	register udt *p = head();
	register const udt *r = b.head();
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
			return *p > *r? 1: -1;
	return 0;
}


int ubigint::cmp(udt i) const
{
	if (fn0() < tail())
		return 1;
	if (*tail() == i)
		return 0;
	return *tail() > i? 1: -1;
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
	if (i == 0)
		return *this;
//	cerr << "ubigint::operator>>=(" << i << ");\n";
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
	if (i == 0)
		return *this;
//	cerr << "ubigint::operator<<=(" << i << ");\n";
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


void ubigint::divu(ubigint b, ubigint &w)
{
	int i = fn0b(), j = b.fn0b();
	register k;
	if (j == -1)
		syserr("ubigint::div()\nDivision by zero!");
	w.clr();
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



void ubigint::mulu(const ubigint &b, ubigint &w) const
{
	register i = fn0b();
	w.clr();
	while(i >= 0) {
		w <<= 1;
		if (testbit(i))
			w.add(b);
		--i;
	}
}

/*
void crypt(const ubigint &z, const ubigint &n, FILE *src, FILE *dst)
{
	ubigint b, w;
	size_t nobj;
	while(!feof(src)) {
		nobj = fread((char *) b.head() + b.sizeb() / 2, 1, b.sizeb() / 2, src);
		printf("Wczyta³em %d\n", nobj);
		if (ferror(src))
			syserr("fread");
		fastexp(b, z, n, w);
		if (nobj > fwrite((char *) w.head() + b.sizeb() / 2, 1, nobj, dst))
			syserr("fwrite");
	}
}
*/

/* ¦wiadomie zawê¿am do 8 bitów Jak wiem z w³asnego do¶wiadczenie, na
 * niektórych systemach RAND_MAX nie jest wcale du¿e. St±d to
 * samo-ograniczenie do o¶miu bitów, by zawsze by³o ok.
 */ 

ubigint& bigrand(ubigint &b)
{
	register udt x, *p = b.tail();// b.head() + 3 * (N_DT+1) / 4; /* chcia³e¶ tylko po³owê */
	register uint i;
	b.clr();
	cerr << "wypelnie tylko " << b.tail() - p << " pozycji\n";
	for(; p <= b.tail(); ++p) {
		for(x = i = 0; i < sizeof(udt); ++i) {
			x <<= CHAR_BIT;
			x |= rand() & 0xFF;
		}
		*p = x;
	}
	return b;
}
