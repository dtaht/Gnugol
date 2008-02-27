DIRS:=common lib clients server plugins prototypes scripts  

all: 
	foreach $(DIRS) make -C $(_)

install:
	cp gnugol.cgi $(CGIDIR)
	mkdir -p $(HTMLDIR)/images; cp doc/images/gnugol.png $(HTMLDIR)/images
	cp doc/*.html $(HTMLDIR)
	mkdir -p $(HOME)/bin; cp goog gnugol $(HOME)/bin

clean:
	foreach $(DIRS) make -C $(_) clean

