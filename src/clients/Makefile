LIBS=-ljansson -lcurl
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=../common/handy.o ../common/utf8.o
ENGINES=../engines/googlev1.o

INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG:=-DDEBUG
PROGS=gnugol # gnugol.cgi 
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
#BINDIR=$(HOME)/bin
BINDIR=/usr/local/bin
OBJECTS=gnugol.o # gnugol_cgi.o
CC=gcc
STRIP=strip

#OBJECTS := connect_client.o listen_server.o parser.o 

all: $(PROGS) 

gnugol.cgi: gnugol_cgi.c $(HEADERS) $(COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol_cgi.c $(COMMON) $(LIBS) $(PLUGINS) $(ENGINES) $(CGILIB) -o gnugol.cgi

gnugol: gnugol.c $(HEADERS) $(COMMON) $(PLUGINS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol.c $(COMMON) $(PLUGINS) $(ENGINES) $(LIBS) -o gnugol

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