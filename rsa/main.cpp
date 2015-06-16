#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include "ubigint.h"
#include "keygen.h"
#include "syserr.h"

#define	PUB_KEY_FILE	"public.key"
#define	PRIV_KEY_FILE	"private.key"


void usage(void);
void write_keys(const ubigint&, const ubigint&, const ubigint&);
void read_keys(ubigint *, ubigint *, const char *);


int main(int argc, char **argv)
{
	FILE *src, *dst;
	ubigint n, d, e;
	int cr;

	srand(time(NULL));

	++argv;

	if (argc == 2) {
		if (strcmp("-g", *argv) != 0)
			usage();

		keygen(&n, &d, &e);
		write_keys(n, e, d);

	} else if (argc == 4 || argc == 6) {

		if (argc == 6) {

			if (strcmp(*argv, "-k") == 0) {
				read_keys(&n, &e, argv[1]);
				argv += 2;
			} else if (strcmp(argv[3], "-k") == 0) {
				read_keys(&n, &e, argv[4]);
			} else usage();
		}

		if (strcmp(*argv, "-e") != 0 && strcmp(*argv, "-d") != 0)
			usage();

		cr = argv[0][1] == 'e';
		++argv;

		if (argc == 4)
			read_keys(&n, &e, cr? PRIV_KEY_FILE: PUB_KEY_FILE);

		if (strcmp(*argv, "-") == 0)
			src = stdin;
		else if ((src = fopen(*argv, "rb")) == NULL)
			syserr("fopen(\"%s\", \"rb\")\n", *argv);

		if (strcmp(*++argv, "-") == 0)
			dst = stdout;
		else if ((dst = fopen(*argv, "wb")) == NULL)
			syserr("fopen(\"%s\", \"wb\")\n", *argv);

		crypt(e, n, src, dst, !cr);

	} else usage();


	return EXIT_SUCCESS;
}


void usage(void)
{
	fprintf(stderr, "Usage: prog OPTION\n"
		"  -g\t\t\tgenerate key pair\n"
		"  -e FILE1 FILE2\tencrypt FILE1 to FILE2 (using \'"PRIV_KEY_FILE"\')\n"
		"  -d FILE1 FILE2\tdecrypt FILE1 to FILE2 (using \'"PUB_KEY_FILE"\')\n"
		"  -k FILE\tread keys from FILE instead of \'"PRIV_KEY_FILE"\' or \'"PUB_KEY_FILE"\'\n"
		"When FILE is -, read standard input or/and write standard output.\n"
		"(It doesnt\'t work with -k option)\n\n"
		"Author: Michal Jacykiewicz <yac@icslab.agh.edu.pl>\n");
	exit(EXIT_FAILURE);
}


void write_keys(const ubigint &n, const ubigint &e, const ubigint &d)
{
	FILE *file;
	if ((file = fopen(PRIV_KEY_FILE, "wb")) == NULL)
		syserr("fopen(\""PRIV_KEY_FILE"\", \"wb\")\n");

	n.fput(file);
	e.fput(file);

	if (EOF == fclose(file))
		syserr("fclose");

	if ((file = fopen(PUB_KEY_FILE, "wb")) == NULL)
		syserr("fopen(\""PUB_KEY_FILE"\", \"wb\")\n");

	n.fput(file);
	d.fput(file);

	if (EOF == fclose(file))
		syserr("fclose");
}


void read_keys(ubigint *n, ubigint *k, const char *fname)
{
	FILE *file;
	if ((file = fopen(fname, "rb")) == NULL)
		syserr("fopen(\"%s\", \"rb\")\n", fname);

	n->fget(file);
	k->fget(file);

	if (EOF == fclose(file))
		syserr("fclose");
}
