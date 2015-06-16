#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include "syserr.h"



void syserr(const char *text, ...)
{
	va_list	ap;

	extern int errno, sys_nerr;
	extern const char *sys_errlist[]; /* Comment it when compiling on Linux */

	va_start(ap, text);

	fprintf(stderr, "ERROR: ");			/* ERROR: */
	vfprintf(stderr, text, ap);			/* formated text */

	if (errno > 0 && errno < sys_nerr)	/* error message */
		fprintf(stderr, "(%d; %s)", errno, sys_errlist[errno]);

	fprintf(stderr, "\n");				/* EOL */

	va_end(ap);
	exit(EXIT_FAILURE);
}
