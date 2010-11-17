LIBS=-ljansson -lcurl
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=../common/connect_client.o ../common/parser.o ../common/fit_results.o udp_client.o ../common/handy.o ../common/utf8.o
PLUGINS=../plugins/googlev1.o

INC=../include
INCLUDES=-I../include -I/usr/local/include
#DEBUG:=-DDEBUG -DDUMMY_SERVER
DEBUG:=-DDEBUG
PROGS=gnugol gnugol.cgi 
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
#BINDIR=$(HOME)/bin
BINDIR=/usr/local/bin
OBJECTS=gnugol.o gnugol_cgi.o
CC=gcc
STRIP=strip

#OBJECTS := connect_client.o listen_server.o parser.o 

all: $(PROGS) 

gnugol.cgi: gnugol_cgi.c udp_client.o $(HEADERS) $(COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol_cgi.c $(COMMON) $(LIBS) $(CGILIB) -o gnugol.cgi

gnugol: gnugol.c udp_client.o $(HEADERS) $(COMMON) $(PLUGINS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol.c $(COMMON) $(PLUGINS) $(LIBS) -o gnugol

udp_client.o: udp_client.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) udp_client.c $(LIBS) -c

#gnugold: gnugold.o 
#	$(CC) gnugold.o -o gnugold ${LIBS}

install: $(PROGS)
	cp gnugol $(DESTDIR)$(BINDIR)
	cp gnugol.cgi $(DESTDIR)$(CGIDIR)

install-stripped: $(PROGS)
	$(STRIP) $(PROGS)
	cp gnugol $(DESTDIR)$(BINDIR)
	cp gnugol.cgi $(DESTDIR)$(CGIDIR)

clean:
	rm -f $(OBJECTS) $(PROGS) *~

