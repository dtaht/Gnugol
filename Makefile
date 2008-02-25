LIBS=-I/usr/local/include -lcgic
PROGS=gnugol.cgi # gnugold
# Basic test makefile
OBJECTS := connect_client.o listen_server.o udp_server.o tcp_server.o udp_client.o tcp_client.o

all: gnugol.cgi # gnugold

all2: tcp_server udp_server udp_client tcp_client

gnugol.cgi: gnugol.o connect_client.o udp_client.o
	gcc gnugol.o udp_client.o connect_client.o -o gnugol.cgi ${LIBS}

gnugold: gnugold.o 
	gcc gnugold.o -o gnugold ${LIBS}

install: gnugol.cgi
	cp gnugol.cgi /usr/lib/cgi-bin

clean:
	rm -f $(OBJECTS)

tcp_server: tcp_server.o listen_server.o
	g++ tcp_server.o listen_server.o -o tcp_server

udp_server: udp_server.o listen_server.o
	g++ udp_server.o listen_server.o -o udp_server

udp_server.o: listen_server.o udp_server.c

tcp_server.o: listen_server.o tcp_server.c

connect_client.o: connect_client.c connect_client.h

udp_client: connect_client.o udp_client.c

