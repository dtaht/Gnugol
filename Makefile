LIBS=-I/usr/local/include -lcgic
DEBUG=-DDEBUG -DDUMMY_SERVER -DDUMMY_CLIENT
PROGS=gnugol.cgi parser_test fpipe # gnugold
HTMLDIR=/var/www/gnugol
CGIDIR=/usr/lib/cgi-bin
BINDIR=$(HOME)/bin

all: 
	make -C src all
	make -C doc all

install: 
	make -C src install
	make -C doc install
	@echo "If you wish to run the server from xinet.d, a sample script is in etc"

install-stripped: 
	make -C src install-stripped
	make -C doc install
	@echo "If you wish to run the server from xinet.d, a sample script is in etc, probably doesn't work yet"

clean:
	make -C src clean

etags:	
	find src -type f \( -name \*.c -o -name \*.h \) | xargs etags 
