#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <iostream.h>
#include <sys/types.h>
#include "artm.h"
#include "syserr.h"
#include "ubigint.h"


inline udt *ubigint::fn0(void)
{
	register udt *p;
	for(p = msu(); *p == 0; ++p);
	return p <= lsu()? p: (udt *) NULL;
}


inline const udt *ubigint::fn0(void) const
{
	register const udt *p;
	for(p = msu(); *p == 0; ++p);
	return p <= lsu()? p: (udt *) NULL;
}


inline udt *ubigint::fn0b(udt *mask)
{
	register udt *p = fn0();
	register udt tmp, m = (udt) 1 << DT_BIT - 1;
	if (p == NULL)
		return NULL;
			/* at this point we are sure that p points to the non-zero unit
			 * because we have just checked it ( we compared p to NULL )
			 */
	for(tmp = *p; (tmp & m) == 0; m >>= 1)
	*mask = m;
	return p;
}


inline const udt *ubigint::fn0b(udt *mask) const
{
	register const udt *p = fn0();
	register udt tmp, m = (udt) 1 << DT_BIT - 1;
	if (p == NULL)
		return NULL;
	for(tmp = *p; (tmp & m) == 0; m >>= 1);
	*mask = m;
	return p;
}


			/* adds 'b' to 'this', result in 'this'  same as  this += b */
void ubigint::add(const ubigint &b)
{
	register udt tmp;
	register udt *p = lsu();	/* ptr to least significant unit */
	register udt *k = msu();	/* ptr to the most significant unit */
	register const udt *r = b.lsu();
	register c = 0;				/* carry */

	while(p >= k) {
		if (c != 0) {
			tmp = *p + *r + 1;
			c = *r >= (udt) ~*p;
		} else {
			tmp = *p + *r;
			c = tmp < *p;
		}
		*p-- = tmp;
		--r;
	}
}


void ubigint::add(udt i)
{
	register udt *p = lsu();	/* ptr to least significant unit */
	*p += i;
	if (*p-- >= i)			/* nie bylo przeniesienia */
		return;
	while(++*p-- == 0);
	setmarkers();
}


void ubigint::sub(const ubigint &b)
{
	register udt tmp;
	register udt *p = lsu();	/* ptr to least significant unit */
	register udt *k = msu();	/* ptr to the most significant unit */
	register const udt *r = b.lsu();
	register c = 0;				/* borrow */

	while(p >= k) {
		if (c != 0) {
			tmp = *p - *r - 1;
			c = *p <= *r;
		} else {
			tmp = *p - *r;
			c = *p < *r;
		}
		*p-- = tmp;
		--r;
	}
}


void ubigint::sub(udt i)
{
	register udt *p = lsu();	/* ptr to least significant unit */
	register c = *p < i;		/* borrow */
	*p-- -= i;
	if (c == 0)
		return;
	while(--*p-- == (udt) -1);
	setmarkers();
}


ubigint& ubigint::operator>>=(register unsigned i)
{
	register udt tmp, mask, c = 0, d;
	register udt *p = msu(), *k = lsu();

	if (i == 0)
		return *this;

	if (i >= DT_BIT) {
		div_t dv = div(i, DT_BIT);
		size_t j = dv.quot * sizeof(udt);
		i = dv.rem;
		memmove(p + dv.quot, p, sizeb() - j);
		memset(p, 0, j);
		if (i == 0)
			return *this;
		p += dv.quot;
	}

	mask = ((udt) 1 << i) - (udt) 1;

	p = fn0();

//	p = msu();

	while(p <= k) {
		tmp = *p;
		d = tmp & mask;
		tmp >>= i;
		tmp |= c << DT_BIT - i;
		c = d;
		*p++ = tmp;
	}

	return *this;
}


ubigint& ubigint::operator<<=(register unsigned i)
{
	register udt x;
	register udt *p = lsu(), *k = msu();

	if (i >= DT_BIT) {
		div_t dv = div(i, DT_BIT);
		size_t j = dv.quot * sizeof(udt);
		i = dv.rem;
		memmove(msu(), msu() + dv.quot, sizeb() - j);
		memset(lsu() + 1  - dv.quot, 0, j);
	}

	switch(i) {			/* This is because with condtant masks and constant bit shfits is _much_ faster */

		case 0:
			break;

		case 1: {
			register c = 0, d;	/* we dont't need here to have udt size of register */
			while(p >= k) {
				x = *p;
				d = (dt) x < 0;
				x <<= 1;
				*p-- = x | c;
				c = d;
			}
			break; }

		default: {
			register udt c = 0, d, mask = ~(( (udt) 1 << DT_BIT - i) - 1);
			while(p >= k) {
				x = *p;
				d = x & mask;
				x <<= i;
				*p-- = x | (udt) c >> (DT_BIT - i);
				c = d;
			}
			break; }
	}

	return *this;
}




int ubigint::cmp(const ubigint &b) const
{
	register const udt *p = msu(), *k = lsu();
	register const udt *r = b.msu();
	while(p <= k) {
		if (*p < *r)
			return -1;
		if (*p++ > *r++)
			return 1;
	}
	return 0;
}


int ubigint::cmp(udt i) const
{
	register const udt *p = fn0();
	if (p == NULL)				/* only when *this==0 */
		return i == 0? 0: -1;
	if (p > lsu())				/* *this > max_of_udt */
		return 1;
	if (*p == i)
		return 0;
	return *p > i? 1: -1;
}


void ubigint::mulu(const ubigint &b, ubigint *result) const
{
	udt m;
	register const udt *p = fn0b(&m);	/* pointer to first non-zero unit */
	register udt mask = m;				/* mask for operations with bits */
	register udt tmp;
	register const udt *k = lsu();
	*result = 0;
	if (p == NULL)			/* we must be sure, that we are at */
		return;				/* exactly non-zero unit */
	do {
		tmp = *p++;
		do {
			*result <<= 1;
			if (tmp & mask)
				*result += b;
			mask >>= 1;					/* shift right our bitmask */
		} while(mask != 0);				/* if that was last bit */
		mask = (udt) 1 << DT_BIT - 1;	/* set the highest bit of the mask to 1 */
	} while(p <= k);
}


void ubigint::mulu(register udt i, ubigint *result) const
{
	register udt mask = (udt) 1 << DT_BIT - 1;
	*result = 0;
	do {
		*result <<= 1;
		if (i & mask)
			*result += *this;
		mask >>= 1;					/* shift right our bitmask */
	} while(mask != 0);				/* if that was last bit */
}


void ubigint::divu(const ubigint &b, ubigint *quot, ubigint *rem) const
{
	udt m;
	register const udt *p = fn0b(&m), *k = lsu();
	register udt *q = quot->msu();
	register udt mask = m;

	if (b == 0)
		cerr << "Division by zero";

	*quot = 0;

	if (p == NULL) {
		*rem = *this;
		return;
	}
	*rem = 0;

	q += p - msu();

	while(p <= k) {
		do {

			*rem <<= 1;
			if (*p & mask)
				*rem->lsu() |= 1;

			if (*rem >= b)	{
				*rem -= b;
				*q |= mask;
			}
			mask >>= 1;
		} while(mask != 0);
		mask = (udt) 1 << DT_BIT - 1;
		++p;
		++q;
	}
}


ostream& operator<<(ostream &os, const ubigint &b)
{
	udt m;
	const udt *p = b.fn0();
//	os << *b.lsu() << "\t\t";
	if (p == NULL)
		return os;
	for(p = b.fn0(); p <= b.lsu(); p++) {
		for(m = (udt) 1 << DT_BIT-1; m != 0; m >>= 1)
			os << char((m & *p)? '1': '0');
		os << ".";
	}
	os << b.bits();
	return os;
}


void bigrand(ubigint *b)
{
	register udt tmp, *p = b->lsu(), *k = b->msu() + 3 * N_DT / 4;
	register unsigned i;

	for(*b = 0; p > k; --p) {
		for(tmp = i = 0; i < sizeof(udt); ++i) {
			tmp <<= CHAR_BIT;
			tmp |= rand() & 0xFF;
		}
		*p = tmp;
	}
}


void crypt(const ubigint &z, const ubigint &n, FILE *src, FILE *dst, int decr)
{
	ubigint b = 0, w = 0;
	size_t nu = n.lsu() - n.fn0();
	size_t nur = nu + (decr? 1: 0);	// jesli odczytujemy raw text to decr=0 wiec wczytujemy +0, zapisujemy zas +1
	size_t nuw = nu + (decr? 0: 1);	// jesli odczytujemy zakodowany, to +1, zas zapisujemy +0

			/* if n == 0 it will crash */
			/* This mean that n must be at least sizeof two units */
			/* Because we need to read at least one unit */

	while(!feof(src)) {

	//	b = w = 0;

		fread(b.lsu() + 1 - nur, sizeof(udt), nur, src);

		if (ferror(src))
			syserr("fread");

//		cout << "READ  " << b << endl;

		putc('.', stderr);
		fflush(stderr);

		fastexp(b, z, n, &w);

//		cout << "WRITE " << w << endl;

		if (fwrite(w.lsu() + 1 - nuw, sizeof(udt), nuw, dst) < nuw)
			syserr("fwrite");

	}
	putc('\n', stderr);
}


void ubigint::fput(FILE *f) const
{
	if (sizeb() > fwrite(msu(), 1, sizeb(), f))
		syserr("fwrite");
}


void ubigint::fget(FILE *f)
{
	fread(msu(), 1, sizeb(), f);
	if (ferror(f))
		syserr("fread");
}


unsigned ubigint::bits(void) const
{
	register udt x, mask = (udt) 1 << DT_BIT - 1;
	register unsigned n = DT_BIT;
	const udt *p = fn0();
	if (p == NULL)
		return 0;
	for(x = *p; (mask & x) == 0; --n, mask >>= 1);
	return (lsu() - p) * DT_BIT + n;
}
