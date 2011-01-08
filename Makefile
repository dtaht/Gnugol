LIBS=-I/usr/local/include -lcgic
DEBUG=-DDEBUG 
#DEBUG=-DDEBUG -DDUMMY_SERVER -DDUMMY_CLIENT
PROGS=gnugol.cgi parser_test fpipe # gnugold
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin

.PHONY: install

all: 
	$(MAKE) -C src all
	$(MAKE) -C doc all

install: 
	$(MAKE) -C src install
	$(MAKE) -C doc install

install-stripped: 
	$(MAKE) -C src install-stripped
	$(MAKE) -C doc install

clean:
	$(MAKE) -C src clean

etags:	
	find src -type f \( -name \*.c -o -name \*.h \) | xargs etags --append
