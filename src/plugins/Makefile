LIBS=../common/fit_results.o
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=../common/connect_client.o ../common/parser.o ../common/listen_server.o
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG
OBJECTS=gscrape.o
PROGS=$(TESTDIR)gscrape_test
CC=gcc
STRIP=strip
TESTDIR=../tests/

all: $(OBJECTS) $(PROGS)

$(TESTDIR)gscrape_test: gscrape.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -DTEST_PLUGIN gscrape.c $(LIBS) -o $(TESTDIR)gscrape_test

gscrape.o: gscrape.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) gscrape.c -c
	
#install-stripped: $(PROGS)
#	$(STRIP) $(PROGS)
#	cp gnugol $(DESTDIR)$(BINDIR)
#	cp gnugol.cgi $(DESTDIR)$(CGIDIR)
	
clean:
	rm -f $(OBJECTS) $(PROGS) *~

