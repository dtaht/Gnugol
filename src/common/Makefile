PLATFORM:=$(shell uname -s)
LIBS=-I/usr/local/include -lcgic
INC=../include
INCLUDES=-I../include
DEBUG=-DDEBUG
PROGS=
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin
TESTDIR=../tests/
CFLAGS+=-g
CFLAGS+= -Wall -Wextra -pedantic
CFLAGS+=-std=c99

ifeq ($(PLATFORM),FreeBSD)
CFLAGS += -fPIC -shared
EXT=so
CC=gcc
INCLUDES+=-I/usr/local/include
# FreeBSD doesn't have ENODATA. EIO is close enough, since gnugol is simply
# using this as a return value.
CFLAGS +=-DENODATA=EIO
endif

ifeq ($(PLATFORM),Linux)
CFLAGS += -fPIC -shared
EXT=so
CC=gcc
endif

ifeq ($(PLATFORM),Darwin)
CFLAGS+= -fno-common
EXT=dylib
endif


OBJECTS := handy.o utf8.o format.o utf8_urlencode.o nodelist.o engines.o

all: $(PROGS) $(OBJECTS) 

clean:
	rm -f $(OBJECTS) $(PROGS) *~

utf8_urlencode.o: utf8_urlencode.c $(INC)/utf8_urlencode.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c utf8_urlencode.c

handy.o: handy.c $(INC)/query.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c handy.c

utf8.o: utf8.c $(INC)/utf8.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c utf8.c

format.o: format.c $(INC)/formats.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c format.c

nodelist.o : nodelist.c $(INC)/nodelist.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c nodelist.c

engines.o: engines.c $(HEADERS)
	$(CC) -DSO_EXT='"$(EXT)"' $(CFLAGS) $(INCLUDES) $(DEBUG) engines.c -c -o $@
