PLATFORM:=$(shell uname -s)
LIBS=../common/fit_results.o
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
SHAREDLIBDIR=/var/lib/gnugol
COMMON=
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG
CFLAGS += -g
CFLAGS += -Wall -Wextra -pedantic -std=c99 -rdynamic
SFLAGS := $(CFLAGS)
LDFLAGS += -L/usr/local/lib -ljansson -lcurl

ifeq ($(PLATFORM),Linux)
CFLAGS += -fPIC -shared
EXT = so
CC=gcc
endif

ifeq ($(PLATFORM),Darwin)
CFLAGS += -fno-common
LDFLAGS += -dynamiclib -dynamic ../common/*.o engines.o
EXT = dylib
endif

OBJECTS=google.$(EXT) dummy.$(EXT) bing.$(EXT) wikipedia.$(EXT) dns.$(EXT) credits.$(EXT) engines.o # googlev2.so


STRIP=strip
TESTDIR=../tests/
all: $(OBJECTS) $(PROGS)

google.$(EXT): googlev1.c $(HEADERS) engines.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev1.c -o google.$(EXT) $(LDFLAGS)

googlev2.$(EXT): googlev2.c $(HEADERS) engines.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev2.c -o $@ $(LDFLAGS)

bing.$(EXT): bing.c $(HEADERS) engines.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) bing.c -o $@ $(LDFLAGS)

wikipedia.$(EXT): wikipedia.c $(HEADERS) engines.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) wikipedia.c -o $@ $(LDFLAGS)

dummy.$(EXT): dummy.c $(HEADERS) engines.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dummy.c -o $@ $(LDFLAGS)

dns.$(EXT) : dns.c $(HEADERS) engines.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dns.c -o $@ $(LDFLAGS)

credits.$(EXT) : credits.c $(HEADERS) engines.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) credits.c -o $@ $(LDFLAGS)

engines.o: engines.c $(HEADERS)
	$(CC) -DSO_EXT='"$(EXT)"' $(SFLAGS) $(INCLUDES) $(DEBUG) engines.c -c -o $@

install: $(OBJECTS) 
	@mkdir -p $(SHAREDLIBDIR)	
	cp *.$(EXT) $(SHAREDLIBDIR)
clean:
	rm -f $(OBJECTS) $(PROGS) *~
