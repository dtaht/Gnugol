LIBS=-I/usr/local/include -lcgic
INC=../include
INCLUDES=-I../include
DEBUG=-DDEBUG
PROGS=
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin
CC=gcc
TESTDIR=../tests/
CFLAGS += -Wall -Wextra -pedantic
CFLAGS+=-std=c99

OBJECTS := handy.o utf8.o format.o utf8_urlencode.o nodelist.o

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

