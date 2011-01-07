# GNU Make only, please
PLATFORM=$(shell uname -s)

LIBS=-ljansson -lcurl -ldl
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=../common/handy.o ../common/utf8.o ../common/format.o ../common/utf8_urlencode.o ../common/nodelist.o
#ENGINES=../engines/googlev1.o  ../engines/dummy.o ../engines/bing.o ../engines/wikipedia.o ../engines/engines.o # ../engines/googlev2.o
ENGINES=../engines/engines.o

INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG:=-DDEBUG
PROGS=gnugol #gnugol_fcgi.cgi 
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
#BINDIR=$(HOME)/bin
BINDIR=/usr/local/bin
OBJECTS=gnugol.o #gnugol_fcgi.o
CFLAGS += -std=c99 -Wall -Wextra -pedantic
STRIP=strip

ifeq "$(PLATFORM)" "Darwin"
CFLAGS += -fno-common
LDFLAGS  = -dynamiclib
else
CC=gcc
CFLAGS += -rdynamic
endif

CFLAGS+=-g

#OBJECTS := connect_client.o listen_server.o parser.o 

all: $(PROGS) 

gnugol.cgi: gnugol_cgi.c $(HEADERS) $(COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol_cgi.c $(COMMON) $(LIBS) $(PLUGINS) $(ENGINES) $(CGILIB) -o gnugol.cgi

gnugol: gnugol.c $(HEADERS) $(COMMON) $(PLUGINS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol.c $(COMMON) $(PLUGINS) $(ENGINES) $(LIBS) -o gnugol

gnugol_fcgi.cgi: gnugol_fcgi.c $(HEADERS) $(COMMON) $(PLUGINS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol_fcgi.c $(COMMON) $(PLUGINS) $(ENGINES) $(LIBS) -lfcgi -o gnugol_fcgi.cgi

#gnugold: gnugold.o 
#	$(CC) gnugold.o -o gnugold ${LIBS}

install: $(PROGS)
	cp gnugol $(DESTDIR)$(BINDIR)
#	cp gnugol.cgi $(DESTDIR)$(CGIDIR)

install-stripped: $(PROGS)
	$(STRIP) $(PROGS)
	cp gnugol $(DESTDIR)$(BINDIR)
#	cp gnugol.cgi $(DESTDIR)$(CGIDIR)

clean:
	rm -f $(OBJECTS) $(PROGS) *~
