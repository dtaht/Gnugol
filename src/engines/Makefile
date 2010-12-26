LIBS=../common/fit_results.o
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG
OBJECTS=googlev1.so dummy.so bing.so wikipedia.so engines.so dns.so # googlev2.so
CC=gcc
CFLAGS += -fPIC -shared
STRIP=strip
TESTDIR=../tests/
CFLAGS+=-std=c99
all: $(OBJECTS) $(PROGS)

googlev1.so: googlev1.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev1.c -o google.so

googlev2.so: googlev2.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev2.c -o $@

bing.so: bing.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) bing.c -o $@

wikipedia.so: wikipedia.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) wikipedia.c -o $@

dummy.so: dummy.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dummy.c -o $@

engines.so: engines.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) engines.c -o $@

dns.so : dns.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dns.c -o $@

clean:
	rm -f $(OBJECTS) $(PROGS) *~
