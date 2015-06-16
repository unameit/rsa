#include <stdio.h>
#include "ubigint.h"
#include "artm.h"
#include "prime.h"


void prime(ubigint *p)
{
	const ubigint x[] = { 2, 3, 5, 7 };
	ubigint pm1, tmp;
	unsigned i, ok;

	fprintf(stderr, "Generating prime..");

	*p |= 0x01;

	do {
		putc('.', stderr);
		fflush(stderr);

		ok = 1;
		*p += 2;
		pm1 = *p;
		--pm1;

		for(i = 0; i < sizeof x / sizeof x[0]; ++i) {
			fastexp(x[i], pm1, *p, &tmp);
			if (tmp != 1) {
				ok = 0;
				break;
			}
		}
	} while(!ok);

	fprintf(stderr, "ok\n");
}
