LIBS=-I/usr/local/include -lcgic
DEBUG=-DDEBUG 
#DEBUG=-DDEBUG -DDUMMY_SERVER -DDUMMY_CLIENT
PROGS=gnugol.cgi parser_test fpipe # gnugold
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin

.PHONY: install

all: 
	make -C src all
	make -C doc all

install: 
	make -C src install
	make -C doc install

install-stripped: 
	make -C src install-stripped
	make -C doc install

clean:
	make -C src clean

etags:	
	find src -type f \( -name \*.c -o -name \*.h \) | xargs etags --append
