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
ENGINES = ../common/engines.o
STRIP=strip
TESTDIR=../tests/

ifeq ($(PLATFORM),FreeBSD)
CFLAGS += -fPIC -shared
EXT = so
CC=gcc
SHAREDLIBDIR=/usr/local/lib/gnugol
endif

ifeq ($(PLATFORM),Linux)
CFLAGS += -fPIC -shared
EXT = so
CC=gcc
endif

ifeq ($(PLATFORM),Darwin)
CFLAGS += -fno-common
LDFLAGS += -dynamiclib -dynamic ../common/*.o
EXT = dylib
endif

OBJECTS=google.$(EXT) dummy.$(EXT) bing.$(EXT) wikipedia.$(EXT) dns.$(EXT) credits.$(EXT) \
	stackapps.$(EXT) # googlev2.so

all: $(OBJECTS) $(PROGS)

google.$(EXT): googlev1.c $(HEADERS) $(ENGINES)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev1.c -o google.$(EXT) $(LDFLAGS)

googlev2.$(EXT): googlev2.c $(HEADERS) $(ENGINES)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) googlev2.c -o $@ $(LDFLAGS)

bing.$(EXT): bing.c $(HEADERS) $(ENGINES)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) bing.c -o $@ $(LDFLAGS)

stackapps.$(EXT): stackapps.c $(HEADERS) $(ENGINES)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) stackapps.c -o $@ $(LDFLAGS)

wikipedia.$(EXT): wikipedia.c $(HEADERS) $(ENGINES)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) wikipedia.c -o $@ $(LDFLAGS)

dummy.$(EXT): dummy.c $(HEADERS) $(ENGINES)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dummy.c -o $@ $(LDFLAGS)

dns.$(EXT) : dns.c $(HEADERS) $(ENGINES)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) dns.c -o $@ $(LDFLAGS)

credits.$(EXT) : credits.c $(HEADERS) $(ENGINES)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) credits.c -o $@ $(LDFLAGS)

install: $(OBJECTS)
	@mkdir -p $(SHAREDLIBDIR)
	cp *.$(EXT) $(SHAREDLIBDIR)
clean:
	rm -f $(OBJECTS) $(PROGS) *~
