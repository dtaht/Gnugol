LIBS=-I/usr/local/include -lcgic
INCLUDES=-I../include
DEBUG=-DDEBUG
PROGS=gnugol.cgi parser_test fpipe # gnugold
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin

# Basic test makefile
OBJECTS := connect_client.o listen_server.o udp_server.o tcp_server.o udp_client.o tcp_client.o

all: $(PROGS)

all2: tcp_server udp_server udp_client tcp_client

gnugol.cgi: gnugol.o connect_client.o udp_client.o parser.o
	gcc gnugol.o udp_client.o connect_client.o parser.o -o gnugol.cgi ${LIBS}

fpipe: fpipe.c
	gcc fpipe.c -o fpipe

#gnugold: gnugold.o 
#	gcc gnugold.o -o gnugold ${LIBS}

parser_test: parser.c
	gcc -DTEST_PARSER parser.c -o parser_test

parser.o: parser.c
	gcc $(DEBUG) parser.c -c -o parser.o

install: gnugol.cgi
	cp gnugol.cgi $(CGIDIR)
	mkdir -p $(HTMLDIR)/images; cp doc/images/gnugol.png $(HTMLDIR)/images
	cp doc/*.html $(HTMLDIR)
	mkdir -p $(HOME)/bin; cp goog gnugol $(HOME)/bin

clean:
	rm -f $(OBJECTS) $(PROGS) *~

tcp_server: tcp_server.o listen_server.o
	g++ tcp_server.o listen_server.o -o tcp_server

udp_server: udp_server.o listen_server.o
	g++ $(DEBUG) udp_server.o listen_server.o -o udp_server

udp_server.o: listen_server.o udp_server.c
	gcc $(DEBUG) -c udp_server.c

tcp_server.o: listen_server.o tcp_server.c

connect_client.o: connect_client.c connect_client.h

udp_client: connect_client.o udp_client.c

