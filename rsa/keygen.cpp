#include <stdio.h>
#include <stdlib.h>
#include "ubigint.h"
#include "prime.h"
#include "artm.h"
#include "keygen.h"


void keygen(ubigint *n, ubigint *d, ubigint *e)
{
	ubigint p, q, fi;
	unsigned nbits;

	bigrand(&p);
	bigrand(&q);

	prime(&p);
	prime(&q);

	*n = p * q;
	nbits = n->bits();
	fi = (p - 1) * (q - 1);

	*d = (max(p, q) + *n) / 2;

	*d |= 0x01;

	fprintf(stderr, "Calculating d and e..");

	do {
		putc('.', stderr);
		fflush(stderr);

		do {
			*d += 2;
			putc('.', stderr);
			fflush(stderr);
		} while(gcd(*d, fi) != 1);

		*e = inv(*d, fi);

	} while(*e < nbits);

	fprintf(stderr, "ok\n");
}
