# currently we are not post processing these files, so all does nothing

PLATFORM:=$(shell uname -s)

ifeq ($(PLATFORM),FreeBSD)
DOCDIR=/usr/local/man/man1/
else
DOCDIR=/usr/local/share/man/man1/
endif

HTMLDIR=/var/www/gnugol
IMAGEDIR=/var/www/gnugol/images
HTML=*.html
IMAGES=images/*.png

all: 

install: 
	mkdir -p $(DESTDIR)$(HTMLDIR) $(DESTDIR)$(IMAGEDIR)
	cp $(HTML) $(DESTDIR)$(HTMLDIR)
	cp $(IMAGES) $(DESTDIR)$(IMAGEDIR)
	cp gnugol.1 /tmp
	gzip /tmp/gnugol.1 
	cp /tmp/gnugol.1.gz $(DOCDIR)gnugol.1.gz
	rm -f /tmp/gnugol.1.gz

install-stripped: install

clean:

