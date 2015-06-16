#include "ubigint.h"
#include "artm.h"

#define	im1	(i == 0? 2: i-1)
#define	ip1	(i == 2? 0: i+1)


ubigint gcd(const ubigint &a, const ubigint &n)
{
	ubigint g[3] = { n, a };
	register i = 1;
	while(g[i] != 0) {
		g[ip1] = g[im1] % g[i];
		i = ip1;
	}
	return g[im1];
}


ubigint inv(const ubigint &a, const ubigint &n)
{
	register i = 1;
	ubigint g[3] = { n, a };
	ubigint v[3] = { 0, 1 };
	ubigint y;
	while(g[i] != 0) {
//		y = g[im1] / g[i];
//		g[ip1] = g[im1] - y * g[i];
		g[im1].divu(g[i], &y, &g[ip1]);
		v[ip1] = v[im1] - y * v[i];
		i = ip1;
	}
	return v[im1].negative()? v[im1] + n: v[im1];
}

#undef	im1
#undef	ip1


void fastexp(ubigint a, ubigint z, const ubigint &n, ubigint *x)
{
	*x = 1;
	while(z != 0) {
		while(!z.odd()) {
			z >>= 1;
			a *= a;
			a %= n;
		}
		--z;
		*x *= a;
		*x %= n;
	}
}


ubigint& max(const ubigint &a, const ubigint &b)
{
	return a > b? a: b;
}
