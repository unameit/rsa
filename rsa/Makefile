CFLAGS = -O3
LFLAGS =
CC = cc
CPP = c++
SHELL =	/bin/sh

####### Files

HEADERS = syserr.h	ubigint.h	artm.h		prime.h		keygen.h
SOURCES	= syserr.cpp	ubigint.cpp	artm.cpp	prime.cpp	keygen.cpp	main.cpp
OBJECTS	= syserr.o	ubigint.o	artm.o		prime.o		keygen.o	main.o
TARGET	= prog

####### Implicit rules

.SUFFIXES: .c .cpp

.c.o:
	$(CC) -c $(CFLAGS) $<

.cpp.o:
	$(CC) -c $(CFLAGS) $<

####### Build rules

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CPP) $(OBJECTS) -o $(TARGET) $(LFLAGS) $(CFLAGS)

depend:
#	@makedepend -- $(CLAGS) -- $(SOURCES) 2> /dev/null
	@makedepend -- $(CLAGS) -I/usr/opt/FSFlibg++/lib/g++-include/ -- $(SOURCES) 2> /dev/null	# wersja dla work.uci.agh.edu.pl

showfiles:
	@echo $(SOURCES) $(HEADERS) Makefile

clean:
	-rm -f *.o *.bak *~ *% #*
	-rm -f $(TARGET) a.out core out gmon.out private.key public.key gprof.out encr decr

# DO NOT DELETE THIS LINE -- make depend depends on it.

ubigint.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
ubigint.o: /usr/include/sys/va_list.h /usr/include/stdlib.h
ubigint.o: /usr/include/string.h /usr/include/limits.h
ubigint.o: /usr/include/sys/isa_defs.h
ubigint.o: /usr/opt/FSFlibg++/lib/g++-include/iostream.h
ubigint.o: /usr/opt/FSFlibg++/lib/g++-include/streambuf.h
ubigint.o: /usr/opt/FSFlibg++/lib/g++-include/libio.h
ubigint.o: /usr/opt/FSFlibg++/lib/g++-include/_G_config.h
ubigint.o: /usr/include/sys/types.h /usr/include/sys/machtypes.h artm.h
ubigint.o: ubigint.h syserr.h
artm.o: ubigint.h /usr/include/stdio.h /usr/include/sys/feature_tests.h
artm.o: /usr/include/sys/va_list.h /usr/include/string.h
artm.o: /usr/include/limits.h /usr/include/sys/isa_defs.h
artm.o: /usr/opt/FSFlibg++/lib/g++-include/iostream.h
artm.o: /usr/opt/FSFlibg++/lib/g++-include/streambuf.h
artm.o: /usr/opt/FSFlibg++/lib/g++-include/libio.h
artm.o: /usr/opt/FSFlibg++/lib/g++-include/_G_config.h artm.h
prime.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
prime.o: /usr/include/sys/va_list.h ubigint.h /usr/include/string.h
prime.o: /usr/include/limits.h /usr/include/sys/isa_defs.h
prime.o: /usr/opt/FSFlibg++/lib/g++-include/iostream.h
prime.o: /usr/opt/FSFlibg++/lib/g++-include/streambuf.h
prime.o: /usr/opt/FSFlibg++/lib/g++-include/libio.h
prime.o: /usr/opt/FSFlibg++/lib/g++-include/_G_config.h artm.h prime.h
keygen.o: /usr/include/stdio.h /usr/include/sys/feature_tests.h
keygen.o: /usr/include/sys/va_list.h /usr/include/stdlib.h ubigint.h
keygen.o: /usr/include/string.h /usr/include/limits.h
keygen.o: /usr/include/sys/isa_defs.h
keygen.o: /usr/opt/FSFlibg++/lib/g++-include/iostream.h
keygen.o: /usr/opt/FSFlibg++/lib/g++-include/streambuf.h
keygen.o: /usr/opt/FSFlibg++/lib/g++-include/libio.h
keygen.o: /usr/opt/FSFlibg++/lib/g++-include/_G_config.h prime.h artm.h
keygen.o: keygen.h
main.o: /usr/include/time.h /usr/include/sys/feature_tests.h
main.o: /usr/include/stdio.h /usr/include/sys/va_list.h /usr/include/stdlib.h
main.o: /usr/include/string.h /usr/opt/FSFlibg++/lib/g++-include/iostream.h
main.o: /usr/opt/FSFlibg++/lib/g++-include/streambuf.h
main.o: /usr/opt/FSFlibg++/lib/g++-include/libio.h
main.o: /usr/opt/FSFlibg++/lib/g++-include/_G_config.h ubigint.h
main.o: /usr/include/limits.h /usr/include/sys/isa_defs.h keygen.h syserr.h
