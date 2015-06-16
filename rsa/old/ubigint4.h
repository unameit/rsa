#ifndef	UBIGINT_H
#define	UBIGINT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <iostream.h>
#include <sys/types.h>

/* By osi±gn±æ jak najwiêksz± wydajno¶æ, niech dt bêdzie NAJWIÊKSZYM typem
 * ca³kowitym na danej realizacji jêzyka.
 */

#if 0
typedef	long long			dt;
typedef	unsigned long long	udt;
#else
typedef	long			dt;
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
#define	DIGITS	20

/* Liczba bitów potrzebna do zapisu liczby o DIGITS cyfrach dziesiêtnych */
#define	UBIGINT_BIT	((size_t)((double)3.321928095*(double)DIGITS)+1)

/* ile bêdzie potrzebnych pól dt, by zapisac liczbê */
#define	N_DT	(UBIGINT_BIT/DT_BIT +1)


class ubigint {
	friend ostream& operator<<(ostream&, const ubigint&);
	friend void crypt(const ubigint&, const ubigint&, FILE *, FILE *);
	friend ubigint& bigrand(ubigint&);
	friend int operator==(udt i, const ubigint &b) { return b.cmp(i) == 0; }
	friend int operator!=(udt i, const ubigint &b) { return b.cmp(i) != 0; }
	friend int operator>=(udt i, const ubigint &b) { return b.cmp(i) <= 0; }
	friend int operator<=(udt i, const ubigint &b) { return b.cmp(i) >= 0; }
	friend int operator>(udt i, const ubigint &b) { return b.cmp(i) < 0; }
	friend int operator<(udt i, const ubigint &b) { return b.cmp(i) > 0; }
	friend udt operator&(udt i, const ubigint &b) { /*cerr << "udt operator&(udt,ubigint);\n";*/ return i & *b.tail(); }
	friend udt operator|(udt i, const ubigint &b) { /*cerr << "udt operator|(udt,ubigint);\n";*/ return i | *b.tail(); }
	friend udt operator^(udt i, const ubigint &b) { /*cerr << "udt operator^(udt,ubigint);\n";*/ return i ^ *b.tail(); }
	friend udt operator&(const ubigint &b, udt i) { /*cerr << "udt operator&(ubigint,udt);\n";*/ return i & *b.tail(); }
	friend udt operator|(const ubigint &b, udt i) { /*cerr << "udt operator|(ubigint,udt);\n";*/ return i | *b.tail(); }
	friend udt operator^(const ubigint &b, udt i) { /*cerr << "udt operator^(ubigint,udt);\n";*/ return i ^ *b.tail(); }
//	friend ubigint operator*(udt i, const ubigint &b) const { ubigint r = b; return r *= i; }
	friend ubigint inv(const ubigint&, const ubigint&);
public:
	ubigint(void) { setmarkers(); }
	ubigint(udt i) {
//		cerr << "ubigint::ubigint(" << i << ");\n";
		*this = i;		/* korzystam z zdefiniowanego ju¿ operatora = */
		setmarkers();
	}

	void test(void) {
		cerr << "DT_BIT = " << DT_BIT
		     << "\nDIGITS = " << DIGITS
		     << "\nUBIGINT_BIT = " << UBIGINT_BIT
		     << "\nN_DT = " << N_DT
		     << "\nsizeof d = " << sizeof d << " bytes = " << (sizeof d >> 10)
		     << "KB = " << (sizeof d >> 20) << "MB"
		     << "\nsizeb() = " << sizeb()
		     << endl;
	}

	ubigint& operator=(udt i) {
//		cerr << "ubigint::operator=(" << i << ");\n";
		*tail() = i;
		memset(head(), 0, sizeb() - sizeof(udt));
		return *this;
	}

	operator udt(void) const { /*cerr << "ubigint::opeator udt();\n";*/ return *tail(); }
	ubigint& operator++(void) { add(1); return *this; }
	ubigint& operator--(void) { sub(1); return *this; }
	ubigint operator++(int) { ubigint r = *this; add(1); return r; }
	ubigint operator--(int) { ubigint r = *this; sub(1); return r; }
	ubigint operator~(void) const;
	ubigint& operator+=(const ubigint &b) { add(b); return *this; }
	ubigint& operator-=(const ubigint &b) { sub(b); return *this; }
	ubigint& operator*=(const ubigint &b) { ubigint r; mulu(b, r); return *this = r; }
	ubigint& operator/=(const ubigint &b) { ubigint r; divu(b, r); return *this = r; }
	ubigint& operator%=(const ubigint &b) { ubigint r; divu(b, r); return *this; }
	ubigint& operator&=(const ubigint&);
	ubigint& operator|=(const ubigint&);
	ubigint& operator^=(const ubigint&);
	ubigint& operator<<=(udt);
	ubigint& operator>>=(udt);
	ubigint operator+(const ubigint &b) const { ubigint r = *this; return r += b; }
	ubigint operator-(const ubigint &b) const { ubigint r = *this; return r -= b; }
	ubigint operator*(const ubigint &b) const { ubigint r = *this; return r *= b; }
	ubigint operator/(const ubigint &b) const { ubigint r, tmp = *this; tmp.divu(b, r); return r; }
	ubigint operator%(const ubigint &b) const { ubigint r, tmp = *this; tmp.divu(b, r); return tmp; }
	ubigint operator&(const ubigint &b) const { ubigint r = *this; /*cerr << "ubigint::operator&(bigint&);\n";*/ return r &= b; }
	ubigint operator|(const ubigint &b) const { ubigint r = *this; return r |= b; }
	ubigint operator^(const ubigint &b) const { ubigint r = *this; return r ^= b; }
	ubigint operator<<(udt i) const { ubigint tmp = *this; return tmp <<= i; }
	ubigint operator>>(udt i) const { ubigint tmp = *this; return tmp >>= i; }

	ubigint& operator+=(udt b) { add(b); return *this; }
	ubigint& operator-=(udt b) { sub(b); return *this; }
	ubigint& operator*=(udt i ) { ubigint r; mulu(i, r); return *this = r; }
	ubigint operator*(udt i) const { ubigint r = *this; return r *= i; }
// TODO:
//	ubigint& operator/=(udt);
//	ubigint& operator%=(udt);
	ubigint& operator&=(udt i) { *tail() &= i; return *this; }
	ubigint& operator|=(udt i) { *tail() |= i; return *this; }
	ubigint& operator^=(udt i) { *tail() ^= i; return *this; }

	ubigint operator+(udt i) const { ubigint tmp = *this; return tmp += i; }
	ubigint operator-(udt i) const { ubigint tmp = *this; return tmp -= i; }
// TODO:
//	ubigint operator*(udt i) const { ubigint tmp = *this; return tmp *= i; }
//	ubigint operator/(udt i) const { ubigint tmp = *this; return tmp /= i; }
//	ubigint operator%(udt i) const { ubigint tmp = *this; return tmp %= i; }
	ubigint operator&(udt i) const { ubigint tmp =*this; /*cerr << "ubigint::operator&(bigint&);\n";*/ return tmp &= i; }
	ubigint operator|(udt i) const { ubigint tmp =*this; return tmp |= i; }
	ubigint operator^(udt i) const { ubigint tmp =*this; return tmp ^= i; }

/* operatory relacji */
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
	int operator!(void) const { return cmp(0) == 0; }

/* Te metod sa w public, a nie w protected (gdzie znajdowaly sie pierwotnie),
 * gdyz lepiej jest je przenisc, niz robic inne funkcje zaprzyjaznione, lub
 * nawet tworzyc dodatkowa klase osobno obslugujaca signed bigint
 */
	void mulu(const ubigint&, ubigint&) const;
	void mulu(udt, ubigint&) const;
	void divu(ubigint, ubigint&);
	int negative(void) { return (dt) *head() < 0; }

protected:
	udt d[N_DT+2];	/* +2 na markery pocz±tku i koñca */

	void setmarkers(void)	{ d[0] = d[sizeof d / sizeof d[0] - 1] = udt(1); }

/* rozmiar tablicy d w BAJTACH (czê¶ci pamiêtaj±cej cyfry) */
	size_t sizeb(void) const { return sizeof d - 2 * sizeof d[0]; }

/* ilo¶æ elementów udt w tablicy d */
	size_t size(void) const { return N_DT; }

/* Wska¼niki na pierwsze i ostatnie dostêpne pola */
	udt *head(void) { return d + 1; }
	udt *tail(void) { return d + N_DT; }
	const udt *head(void) const { return d + 1; }
	const udt *tail(void) const { return d + N_DT; }
//	size_t ihead(void) { return 1; }
//	size_t itail(void) { return N_DT; }


/* Zwraca wska¼nik do pierwszego niezerowego elementu */
	udt *fn0(void) { register udt *p = head(); while(*p++ == 0); return --p; }
	const udt *fn0(void) const { register const udt *p = head(); while(*p++ == 0); return --p; }
//	size_t nn0(void) { return tail() - fn0(); }

/* Zwraca nr pierwszego znacz±cego zapalonego bitu. Je¶li wszystkie s± równe
 * zeru, to zwraca -1.
 */
	int fn0b(void) const {
		register udt m = (udt) 1 << DT_BIT - 1;
		register i = DT_BIT * size() - 1;
		register udt n;
		const udt *p = fn0();
		if (p > tail())
			return -1;
		i -= DT_BIT * (p - head());
		for(n = *p; (n & m) == 0; --i)
			m >>= 1;
		return i;
	}

	void clr(void) { memset(head(), 0, sizeb()); }

	int cmp(const ubigint&) const;
	int cmp(udt i) const;

	void neg(void);

	void add(const ubigint&);
	void sub(const ubigint&);


	void add(udt);
	void sub(udt);

	void setbit(int i) {
		div_t dv = div(i, DT_BIT);
		*(tail() - dv.quot) |= (udt) 1 << dv.rem;
	}
	int testbit(int i) const {
		if (i == 0)
			return *tail() & 0x01;
		div_t dv = div(i, DT_BIT);
		return *(tail() - dv.quot) & (udt) 1 << dv.rem;
	}
};

#endif

/* Celem tej klasy jest obs³uga du¿ych liczb, tj. o N cyfrach. Wystarczy
 * ustawiæ sta³± DIGITS na ¿±dan± liczbê cyfr dziesiêtnych.
 
 * OGRANICZENIA: Na niektórych systemach kompilator nie 'przepuszcza' zbyt
 * du¿ych warto¶ci sta³ej DIGITS (tj. powy¿ej o¶miu milionów). Na innych
 * program kompiluje siê bez b³êdów, natomiast zaraz po uruchomieniu dostaje
 * sygna³ SIGSEGV (np. Linux i586 - warto¶ci powy¿ej pó³tora miliona). 
 * Przypadki te jednak dotycz± skrajnie du¿ych liczb, przy których czas
 * wykonywania podstawowych operacji by³by ju¿ i tak zbyt du¿y.

 * Wyszczególnienie:
 * SPARCstation ... :
 * 8000000 (osiem milionów) cyfr dziesiêtnych. Przy wiêkszych warto¶ciach
 * kompilator zg³asza b³±d, i¿ sta³a jest poza dopuszczalnym zakresem.
 * Linux i586 32MB RAM + 64MB vm :

 * 1500000 (pó³tora milona) cyfr dziesiêtnych. Powy¿ej tej warto¶ci
 * nastêpuje 'Segmentation fault'.

 * AUTOR: Micha³ Jacykiewicz <yac@icslab.agh.edu.pl>
 * Kraków, maj/czerwiec 1998.
 */
