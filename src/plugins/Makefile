LIBS=
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=../common/connect_client.o ../common/parser.o ../common/listen_server.o
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG
OBJECTS=gscrape.o
PROGS=gscrape_test
CC=gcc
STRIP=strip

all: $(OBJECTS) $(PROGS)

gscrape_test: gscrape.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -DTEST_PLUGIN gscrape.c $(LIBS) -o gscrape_test

gscrape.o: gscrape.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gscrape.c $(LIBS) -c
	
#install-stripped: $(PROGS)
#	$(STRIP) $(PROGS)
#	cp gnugol $(DESTDIR)$(BINDIR)
#	cp gnugol.cgi $(DESTDIR)$(CGIDIR)
	
clean:
	rm -f $(OBJECTS) $(PROGS) *~

