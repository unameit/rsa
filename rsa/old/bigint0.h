#include <iostream.h>
#include <sys/types.h>
#include <string.h>

/* Program ten jest napisany tak, by w miarê trzymaæ siê standardu, którego
 * w³a¶ciwie nie ma (tzn. chodzi o C++, bo przecie¿ jest standard ANSI C). 
 * Typ 'long long' nie jest (chyba) jeszcze w ¿adnym standardzie, wiêc,
 * je¶li dany kompilator go nie 'prze³knie' mo¿na bez obaw zast±piæ go tu
 * typem 'long'.
 */


/* Wogóle nas nieobchodz± nadmiary i przepe³nienia. Robimy tak, jak to jest
 * normalnie w arytmetyce ca³kowito-liczbowej w kompach, czyli bez sprawdzania
 * i komunikatów. Je¶li zaczn± wychodzic g³upoty tzn., ¿e nale¿y zwiêkszyæ
 * sta³± DIGITS
 */

#define	DIGITS	(2000U+2U)	/* +2 na markery koñca i pocz±tku */

class bigint {
	friend ostream& operator <<(ostream&, bigint&);
public:
	bigint(void) { setmarkers(); }	/* to i tak tylko dla tmp'ów */
	bigint(long long);
	bigint(const char *);
	bigint& operator ++(void);		/* prefix */
	bigint operator ++(int);		/* postfix*/
	bigint operator +(const bigint&);
	bigint& operator +=(const bigint&);
	bigint& operator --(void);
	bigint operator --(int);
	bigint operator -(const bigint&);
	bigint& operator -=(const bigint&);
	bigint operator *(const bigint&);
	bigint& operator *=(const bigint&);
	bigint operator /(const bigint&);
	bigint& operator /=(const bigint&);
	bigint operator %(const bigint&);
	bigint& operator %=(const bigint&);
	void div(const bigint &denom, bigint *quot= NULL, bigint *rem = NULL);
	int cmp(const bigint&);
	int operator ==(const bigint &bi)	{ return cmp(bi) == 0; }
	int operator >(const bigint &bi)	{ return cmp(bi) > 0; }
	int operator <(const bigint &bi)	{ return cmp(bi) < 0; }
	int operator >=(const bigint &bi)	{ return cmp(bi) >= 0; }
	int operator <=(const bigint &bi)	{ return cmp(bi) <= 0; }
private:
	char d[DIGITS];			/* digits */
	unchar negative;

	void setmarkers(void)	{ d[0] = d[sizeof d - 1] = 1; }
	size_t size(void)	{ return sizeof d - 2; }	/* rozmiar ca³ej tablcy */
	char *first(void)	{ return d + 1; }	/* zostawiamy miejsce na markery koñca */
	char *last(void)	{ return d + sizeof d - 1 - 1; }

/* czyli:
 * pierwszy wolny, który mo¿na u¿ywaæ, to first()
 * ostatni wolny, to last();
 * czyli wszystkie s± zawarte:  first() <= digits <= last();
 * nalê¿±cy jeszcze do tablicy, ale ju¿ nie do bezpo¶redniego u¿ycia jest last()+1;
 */
	void normalize1p(char *);
	void normalize1m(char *);
	void normalize1oncep(char *);
	void normalize1oncem(char *);
	void normalizeallp(char *);
	void normalizeallm(char *);
	void normalizealloncep(char *);
	void normalizealloncem(char *);

	void preadd(const bigint &);
	void presub(const bigint &);
	void add(const bigint &b, unchar ngtv = bi.negative);
	void sub(const bigint &b, unchar ngtv = bi.negative);
/* A je¶li nie zadzia³a to:
	void add(const bigint &b) { add(b, b.negative); }
	void sub(const bigint &b) { sub(b, b.negative); }
*/
};
