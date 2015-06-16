#ifndef	UBIGINT_H
#define	UBIGINT_H

/* Celem tej klasy jest obs�uga du�ych liczb, tj. o N cyfrach. Wystarczy
 * ustawi� sta�� DIGITS na ��dan� liczb� cyfr dziesi�tnych.
 
 * OGRANICZENIA:

 * SPARCstation ... :
 * 8000000 (osiem milion�w) cyfr dziesi�tnych. Przy wi�kszych warto�ciach
 * kompilator zg�asza b��d, i� sta�a jest poza dopuszczalnym zakresem.

 * Linux i586 32MB RAM + 64MB vm :

 * 1500000 (p�tora milona) cyfr dziesi�tnych. Powy�ej tej warto�ci program
 * nast�puje 'Segmentation fault'.

 * AUTOR: Micha� Jacykiewicz <yac@icslab.agh.edu.pl>
 * Krak�w, maj 1998.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <iostream.h>
#include <sys/types.h>

/* By osi�gn�� jak najwi�ksz� wydajno��, niech dt b�dzie najwi�kszym typem
 * ca�kowitym na danej realizacji j�zyka.
 */

#if 1
typedef	long long			dt;
typedef	unsigned long long	udt;
#else
typedef	long			dt;
typedef	unsigned long	udt;
#endif

/* ile bit�w ma dt */
#define	DT_BIT	(sizeof(dt) / sizeof(char) * CHAR_BIT)

/* Nie obchodz� nas nadmiary i przepe�nienia. Robimy tak, jak to jest
 * normalnie w arytmetyce ca�kowito-liczbowej, czyli bez sprawdzania i
 * komunikat�w.  Je�li zaczn� wychodzic g�upoty tzn., �e nale�y zwi�kszy�
 * sta�� DIGITS
 */

/* Sta�a okre�laj�ca ilo�� cyfr DZIESI�TNYCH potrzebnych do zapisu liczby */
#define	DIGITS	20

/* Liczba bit�w potrzebna do zapisu liczby o DIGITS cyfrach dziesi�tnych */
#define	UBIGINT_BIT	((size_t)((double)3.321928095*(double)DIGITS)+1)

/* ile b�dzie potrzebnych p�l dt, by zapisac liczb� */
#define	N_DT	(UBIGINT_BIT/DT_BIT +1)


class ubigint {
	friend ostream& operator<<(ostream&, const ubigint&);
	friend void crypt(const ubigint &, const ubigint &, FILE *, FILE *);
	friend void bigrand(ubigint&);
public:
	ubigint(void) { setmarkers(); }
	ubigint(udt);

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
		*tail() = i;
//		cerr << "=" << i << endl;
		memset(head(), 0, sizeb() - sizeof(udt));
		return *this;
	}

	ubigint& operator++(void) { add(1); return *this; }
	ubigint& operator--(void) { sub(1); return *this; }
	ubigint operator++(int) { ubigint tmp = *this; add(1); return tmp; }
	ubigint operator--(int) { ubigint tmp = *this; sub(1); return tmp; }
	ubigint operator~(void) const;

	ubigint& operator+=(const ubigint &b) { add(b); return *this; }
	ubigint& operator-=(const ubigint &b) { sub(b); return *this; }
	ubigint& operator*=(const ubigint&);
	ubigint& operator/=(const ubigint&);
	ubigint& operator%=(const ubigint&);

	ubigint& operator+=(udt b) { add(b); return *this; }
	ubigint& operator-=(udt b) { sub(b); return *this; }
//	ubigint& operator*=(udt);
//	ubigint& operator/=(udt);
//	ubigint& operator%=(udt);

	ubigint& operator<<=(udt);
	ubigint& operator>>=(udt);
	ubigint& operator&=(udt i) { *tail() &= i; return *this; }
	ubigint& operator|=(udt i) { *tail() |= i; return *this; }
	ubigint& operator^=(udt i) { *tail() ^= i; return *this; }
	ubigint& operator&=(const ubigint&);
	ubigint& operator|=(const ubigint&);
	ubigint& operator^=(const ubigint&);

	ubigint operator+(const ubigint &b) const { ubigint tmp = *this; return tmp += b; }
	ubigint operator-(const ubigint &b) const { ubigint tmp = *this; return tmp -= b; }
	ubigint operator*(const ubigint &b) const { ubigint tmp = *this; return tmp *= b; }
	ubigint operator/(const ubigint &b) const { ubigint tmp = *this; return tmp /= b; }
	ubigint operator%(const ubigint &b) const { ubigint tmp = *this; return tmp %= b; }

	ubigint operator+(udt i) const { ubigint tmp = *this; return tmp += i; }
	ubigint operator-(udt i) const { ubigint tmp = *this; return tmp -= i; }
//	ubigint operator*(udt i) const { ubigint tmp = *this; return tmp *= i; }
//	ubigint operator/(udt i) const { ubigint tmp = *this; return tmp /= i; }
//	ubigint operator%(udt i) const { ubigint tmp = *this; return tmp %= i; }

	ubigint operator<<(udt i) const { ubigint tmp = *this; return tmp <<= i; }
	ubigint operator>>(udt i) const { ubigint tmp = *this; return tmp >>= i; }

	ubigint operator&(udt i) const { ubigint tmp =*this; return tmp &= i; }
	ubigint operator|(udt i) const { ubigint tmp =*this; return tmp |= i; }
	ubigint operator^(udt i) const { ubigint tmp =*this; return tmp ^= i; }
	ubigint operator&(const ubigint &b) const { ubigint tmp =*this; return tmp &= b; }
	ubigint operator|(const ubigint &b) const { ubigint tmp =*this; return tmp |= b; }
	ubigint operator^(const ubigint &b) const { ubigint tmp =*this; return tmp ^= b; }

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

	operator udt(void) const { return *tail(); }
protected:
	udt d[N_DT+2];	/* +2 na markery pocz�tku i ko�ca */

	void setmarkers(void)	{ d[0] = d[sizeof d / sizeof d[0] - 1] = udt(-1); }

/* rozmiar tablicy d w BAJTACH (cz�ci pami�taj�cej cyfry) */
	size_t sizeb(void) const { return sizeof d - 2 * sizeof d[0]; }

/* Wska�niki na pierwsze i ostatnie dost�pne pola */
	udt *head(void) { return d + 1; }
	udt *tail(void) { return d + N_DT; }
	const udt *head(void) const { return d + 1; }
	const udt *tail(void) const { return d + N_DT; }
	size_t ihead(void) { return 1; }
	size_t itail(void) { return N_DT; }


/* Zwraca wska�nik do pierwszego niezerowego elementu */
	udt *fn0(void) { udt *p = head(); while(*p++ == 0); return --p; }
	const udt *fn0(void) const { const udt *p = head(); while(*p++ == 0); return --p; }
	size_t nn0(void) { return tail() - fn0(); }

/* Zwraca nr pierwszego znacz�cego zapalonego bitu. Je�li wszystkie s� r�wne
 * zeru, to zwraca -1.
 */
	int fn0b(void) {	/* inline, bo chyba tylko raz wywo�ywana=rozwijana */
		register udt m = (udt) 1 << DT_BIT - 1;
		register i = DT_BIT - 1;
		register udt n;
		udt *p = fn0();
		if (p > tail())
			return -1;
		for(n = *p; (n & m) == 0; --i)
			m >>= 1;
		return i;
	}

	int cmp(const ubigint&) const;
	int cmp(udt i) const { return fn0() < tail()? 1: *tail() - i; }

	void add(const ubigint&);
	void sub(const ubigint&);
	void add(udt);
	void sub(udt);
	void bigdiv(ubigint, ubigint&);
	void bigmul(const ubigint&, ubigint&);

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
