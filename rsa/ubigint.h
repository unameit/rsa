#ifndef	UBIGINT_H
#define	UBIGINT_H

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <iostream.h>

/* By osi±gn±æ jak najwiêksz± wydajno¶æ, niech dt bêdzie NAJWIÊKSZYM typem
 * ca³kowitym na danej realizacji jêzyka. Oczywiscie z uwzglednieniem specyfiki
 * procesora.
 */

#if 0
typedef	signed long long	dt;
typedef	unsigned long long	udt;
#else
typedef	signed long		dt;
typedef	unsigned long	udt;
#endif


/* ile bitów ma dt */
#define	DT_BIT	(sizeof(dt) / sizeof(char) * CHAR_BIT)

/* Nie obchodz± nas nadmiary i przepe³nienia. Robimy tak, jak to jest
 * normalnie w arytmetyce ca³kowito-liczbowej, czyli bez sprawdzania i
 * komunikatów.  Je¶li zaczn± wychodzic g³upoty tzn., ¿e nale¿y zwiêkszyæ
 * sta³± DIGITS
 */

/* Sta³a okre¶laj±ca ilo¶æ cyfr DZIESIÊTNYCH potrzebnych do zapisu liczby
 * Tê sta³± i tylko tê mo¿esz zmieniaæ!
 */
#define	DIGITS	200

/* Liczba bitów potrzebna do zapisu liczby o DIGITS cyfrach dziesiêtnych */
#define	UBIGINT_BIT	((size_t)((double)3.321928095*(double)DIGITS)+1)

/* ile bêdzie potrzebnych pól dt, by zapisac liczbê */
#define	N_DT	(UBIGINT_BIT/DT_BIT +1)


class ubigint {

	friend ostream& operator<<(ostream&, const ubigint&);
	friend void bigrand(ubigint *);
	friend void crypt(const ubigint&, const ubigint&, FILE *, FILE *, int);

	friend int operator==(udt i, const ubigint &b) { return b.cmp(i) == 0; }
	friend int operator!=(udt i, const ubigint &b) { return b.cmp(i) != 0; }
	friend int operator>=(udt i, const ubigint &b) { return b.cmp(i) <= 0; }
	friend int operator<=(udt i, const ubigint &b) { return b.cmp(i) >= 0; }
	friend int operator>(udt i, const ubigint &b) { return b.cmp(i) < 0; }
	friend int operator<(udt i, const ubigint &b) { return b.cmp(i) > 0; }

public:

	ubigint(void) { setmarkers(); }
	ubigint(udt i) { setmarkers(); *this = i; }

	ubigint& operator=(udt i) { *lsu() = i; memset(msu(), 0, sizeb() - sizeof(udt)); return *this; }

	ubigint& operator++(void) { add(1); return *this; }
	ubigint& operator--(void) { sub(1); return *this; }

	ubigint& operator+=(const ubigint &b) { add(b); return *this; }
	ubigint& operator-=(const ubigint &b) { sub(b); return *this; }
	ubigint& operator*=(const ubigint &b) { ubigint r; mulu(b, &r); return *this = r; }
	ubigint& operator/=(const ubigint &b) { ubigint q, r; divu(b, &q, &r); return *this = q; }
	ubigint& operator%=(const ubigint &b) { ubigint q, r; divu(b, &q, &r); return *this = r; }

	ubigint operator+(const ubigint &b) { ubigint tmp = *this; tmp.add(b); return tmp; }
	ubigint operator-(const ubigint &b) { ubigint tmp = *this; tmp.sub(b); return tmp; }
	ubigint operator*(const ubigint &b) { ubigint r; mulu(b, &r); return r; }
	ubigint operator/(const ubigint &b) { ubigint q, r; divu(b, &q, &r); return q; }
	ubigint operator%(const ubigint &b) { ubigint q, r; divu(b, &q, &r); return r; }

	ubigint& operator+=(udt i) { add(i); return *this; }
	ubigint& operator-=(udt i) { sub(i); return *this; }
	ubigint& operator*=(udt i) { ubigint r; mulu(i, &r); return *this = r; }
//	ubigint& operator/=(udt i) { ubigint q, r; divu(i, &q, &r); return *this = q; }
//	ubigint& operator%=(udt i) { ubigint q, r; divu(i, &q, &r); return *this = r; }

	ubigint operator+(udt i) { ubigint tmp = *this; tmp.add(i); return tmp; }
	ubigint operator-(udt i) { ubigint tmp = *this; tmp.sub(i); return tmp; }
	ubigint operator*(udt i) { ubigint r; mulu(i, &r); return r; }
//	ubigint operator/(udt i) { ubigint q, r; divu(i, &q, &r); return q; }
//	ubigint operator%(udt i) { ubigint q, r; divu(i, &q, &r); return r; }

	ubigint& operator>>=(unsigned);
	ubigint& operator<<=(unsigned);

	ubigint& operator|=(udt i) { *lsu() |= i; return *this; }
	ubigint& operator^=(udt i) { *lsu() ^= i; return *this; }

	int operator==(const ubigint &b) const { return cmp(b) == 0; }
	int operator!=(const ubigint &b) const { return cmp(b) != 0; }
	int operator>=(const ubigint &b) const { return cmp(b) >= 0; }
	int operator<=(const ubigint &b) const { return cmp(b) <= 0; }
	int operator>(const ubigint &b) const { return cmp(b) > 0; }
	int operator<(const ubigint &b) const { return cmp(b) < 0; }

	int operator==(udt i) const { return cmp(i) == 0; }
	int operator!=(udt i) const { return cmp(i) != 0; }
	int operator>=(udt i) const { return cmp(i) >= 0; }
	int operator<=(udt i) const { return cmp(i) <= 0; }
	int operator>(udt i) const { return cmp(i) > 0; }
	int operator<(udt i) const { return cmp(i) < 0; }

	void divu(const ubigint&, ubigint *, ubigint *) const;

	int odd(void) { return *lsu() & 0x01; }
	int negative(void) { return (dt) *msu() < 0; }
	unsigned bits(void) const;

	void fput(FILE *) const;
	void fget(FILE *);

protected:

	udt d[N_DT+2];						/* place where we store digits */

	void setmarkers(void) { d[0] = d[N_DT+1] = 1; }

	udt *msu(void) { return d + 1; }	/* most significant unit */
	udt *lsu(void) { return d + N_DT; }	/* least siginificant unit */

	const udt *msu(void) const { return d + 1; }
	const udt *lsu(void) const { return d + N_DT; }

	size_t sizeb(void) const { return N_DT * sizeof(udt); }

	void add(const ubigint&);			/* 'this += arg' treats both as unsigned */
	void sub(const ubigint&);			/* 'this -= arg' also unsigned */
	int cmp(const ubigint&) const;		/* compares this and b */
	
	void add(udt);			/* 'this += arg' treats both as unsigned */
	void sub(udt);			/* 'this -= arg' also unsigned */
	int cmp(udt) const;		/* compares this and b */
	void mulu(const ubigint&, ubigint *) const;
	void mulu(udt, ubigint *) const;

	inline udt *fn0(void);					/* returns pointer to first non-zero unit or NULL */
	inline const udt *fn0(void) const;		/* version with 'const' specifier */

	inline udt *fn0b(udt *);					/* returns pointer to first non-zero unit and sets the most */
	inline const udt *fn0b(udt *mask) const;	/* significant bit in m as it is in p */
};

#endif
