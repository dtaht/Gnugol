LIBS=../common/fit_results.o
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG
OBJECTS=googlev1.o dummy.o bing.o # googlev2.o
CC=gcc
STRIP=strip
TESTDIR=../tests/
CFLAGS+=-std=c99
all: $(OBJECTS) $(PROGS)

googlev1.o: googlev1.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev1.c -c

googlev2.o: googlev2.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev2.c -c

bing.o: bing.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) bing.c -c

dummy.o: dummy.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dummy.c -c

clean:
	rm -f $(OBJECTS) $(PROGS) *~