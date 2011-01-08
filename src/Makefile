DIRS:=common lib clients server plugins prototypes scripts  

all: 
	$(MAKE) -C common all
	$(MAKE) -C engines all
	$(MAKE) -C clients all

install: 
	$(MAKE) -C common all
	$(MAKE) -C engines install
	$(MAKE) -C clients install
	$(MAKE) -C scripts install

install-stripped: 
	$(MAKE) -C common all
	$(MAKE) -C engines install
	$(MAKE) -C clients install-stripped
	$(MAKE) -C scripts install

clean:
	$(MAKE) -C common clean
	$(MAKE) -C engines clean
	$(MAKE) -C clients clean
	@rm -f TAGS
tags:
	find . -type f \( -name \*.c -o -name \*.h \) -print | xargs etags --append
