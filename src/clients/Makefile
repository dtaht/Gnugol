LIBS=
CGILIB=-L/usr/local/lib -lcgic
COMMON=../common/connect_client.o ../common/parser.o
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG
PROGS=gnugol gnugol.cgi 
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin
CC=gcc
STRIP=strip

#OBJECTS := connect_client.o listen_server.o parser.o 

all: $(PROGS) 

gnugol.cgi: gnugol_cgi.c udp_client.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol_cgi.c udp_client.o $(COMMON) $(LIBS) $(CGILIB) -o gnugol.cgi

gnugol: gnugol.c udp_client.o
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gnugol.c udp_client.o $(COMMON) $(LIBS) -o gnugol

udp_client.o: udp_client.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) udp_client.c $(LIBS) -c
	
#gnugold: gnugold.o 
#	$(CC) gnugold.o -o gnugold ${LIBS}

install:
	cp $(PROGS) $(DESTDIR)$(BINDIR)

install-strip:
	$(STRIP) $(PROGS)
	cp $(PROGS) $(DESTDIR)$(BINDIR)
	
clean:
	rm -f $(OBJECTS) $(PROGS) *~

