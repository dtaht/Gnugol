LIBS=../plugins/gscrape.o
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=../common/connect_client.o ../common/parser.o ../common/listen_server.o ../common/fit_results.o
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG
PROGS=gnugold
OBJECTS=udp_server.o
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin
CC=gcc
STRIP=strip

#OBJECTS := connect_client.o listen_server.o parser.o 

all: $(PROGS) 

gnugold: udp_server.o $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) udp_server.o $(COMMON) $(LIBS) $(CGILIB) -o gnugold

udp_server.o: udp_server.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) udp_server.c $(LIBS) -c
	
#gnugold: gnugold.o 
#	$(CC) gnugold.o -o gnugold ${LIBS}

install: $(PROGS)
	cp gnugold $(DESTDIR)$(BINDIR)

install-stripped: $(PROGS)
	$(STRIP) $(PROGS)
	cp gnugold $(DESTDIR)$(BINDIR)
	
clean:
	rm -f $(OBJECTS) $(PROGS) *~

