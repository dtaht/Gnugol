LIBS=
CGILIB=-L/usr/local/lib -lcgic
HEADERS=../include/*.h
COMMON=../common/connect_client.o ../common/parser.o ../common/listen_server.o ../common/fit_results.o
INC=../include
INCLUDES=-I../include -I/usr/local/include
DEBUG=-DDEBUG 
PROGS=#gnugold
OBJECTS=udp_server.o
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=/usr/sbin
CC=gcc
STRIP=strip

#OBJECTS := connect_client.o listen_server.o parser.o 

all: $(PROGS) 

gnugold: udp_server.o server_options.o $(HEADERS) $(COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) udp_server.o server_options.o $(COMMON) $(LIBS) -o gnugold

udp_server.o: udp_server.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c udp_server.c 

server_options.o: server_options.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEBUG) -c server_options.c 

install: $(PROGS)
#	cp gnugold $(DESTDIR)$(BINDIR)

install-stripped: $(PROGS)
	$(STRIP) $(PROGS)
#	cp gnugold $(DESTDIR)$(BINDIR)
clean:
	rm -f $(OBJECTS) $(PROGS) *~

