# currently we are not post processing these files, so all does nothing

HTMLDIR=/var/www/gnugol
IMAGEDIR=/var/www/gnugol/images
HTML=*.html
IMAGES=images/*.png

all: 

install: 
	mkdir -p $(DESTDIR)$(HTMLDIR) $(DESTDIR)$(IMAGEDIR)
	cp $(HTML) $(DESTDIR)$(HTMLDIR)
	cp $(IMAGES) $(DESTDIR)$(IMAGEDIR)

install-stripped: install

clean:

