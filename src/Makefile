DIRS:=common lib clients server plugins prototypes scripts  

all: 
	make -C common all
	make -C engines all
	make -C clients all
	make -C server all

install: 
	make -C common all
	make -C engines all
	make -C clients install
	make -C server install
	make -C scripts install

install-stripped: 
	make -C common all
	make -C engines all
	make -C clients install-stripped
	make -C server install
	make -C scripts install

clean:
	make -C common clean
	make -C engines clean
	make -C clients clean
	make -C server clean
	rm TAGS
tags:
	find . -type f \( -name \*.c -o -name \*.h \) -print | xargs etags --append