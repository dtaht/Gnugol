LIBS=-I/usr/local/include -lcgic
INC=../include
INCLUDES=-I../include
DEBUG=-DDEBUG
PROGS= # $(TESTDIR)parser_test # fpipe # gnugold
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin
CC=gcc
TESTDIR=../tests/
CFLAGS+=-std=c99

OBJECTS := handy.o utf8.o format.o

all: $(PROGS) $(OBJECTS) 

$(TESTDIR)/multicast_test: multicast.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -DTEST_MULTICAST multicast.c -o $(TESTDIR)/multicast_test

fpipe: fpipe.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) fpipe.c -o fpipe

#gnugold: gnugold.o 
#	$(CC) gnugold.o -o gnugold ${LIBS}

$(TESTDIR)parser_test: parser.c $(INC)/query.h
	$(CC) $(CFLAGS) $(INCLUDES) -DDEBUG=1 -DTEST_PARSER parser.c -o $(TESTDIR)parser_test

parser.o: parser.c $(INC)/query.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) parser.c -c 

clean:
	rm -f $(OBJECTS) $(PROGS) *~

connect_client.o: connect_client.c $(INC)/connect_client.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c connect_client.c

listen_server.o: listen_server.c $(INC)/listen_server.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c listen_server.c

fit_results.o: fit_results.c $(INC)/query.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c fit_results.c

handy.o: handy.c $(INC)/query.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c handy.c

utf8.o: utf8.c $(INC)/utf8.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c utf8.c

multicast.o: multicast.c $(INC)/query.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c multicast.c

format.o: format.c $(INC)/formats.h
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c format.c
