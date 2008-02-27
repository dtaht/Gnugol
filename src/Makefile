DIRS:=common lib clients server plugins prototypes scripts  

all: 
	make -C common all
	make -C clients all

install: 
	make -C clients install

install-stripped: 
	make -C clients install-stripped

clean:
	make -C common clean
	make -C clients clean

