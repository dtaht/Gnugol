LIBS=../common/fit_results.o
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
SHAREDLIBDIR=/var/lib/gnugol
COMMON=
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG
OBJECTS=google.so dummy.so bing.so wikipedia.so dns.so engines.o # googlev2.so
CC=gcc
CFLAGS += -Wall -Wextra -pedantic -std=c99
SFLAGS := $(CFLAGS)
CFLAGS += -fPIC -shared
STRIP=strip
TESTDIR=../tests/
all: $(OBJECTS) $(PROGS)

google.so: googlev1.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev1.c -o google.so

googlev2.so: googlev2.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev2.c -o $@

bing.so: bing.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) bing.c -o $@

wikipedia.so: wikipedia.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) wikipedia.c -o $@

dummy.so: dummy.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dummy.c -o $@

engines.o: engines.c $(HEADERS)
	$(CC) $(SFLAGS) $(INCLUDES) $(DEBUG) engines.c -c -o $@

dns.so : dns.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dns.c -o $@

install: $(OBJECTS)
	@mkdir -p $(SHAREDLIBDIR)	
	cp $(OBJECTS) $(SHAREDLIBDIR)
clean:
	rm -f $(OBJECTS) $(PROGS) *~
