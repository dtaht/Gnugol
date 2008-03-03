DIRS:=common lib clients server plugins prototypes scripts  

all: 
	make -C common all
	make -C plugins all
	make -C clients all
	make -C server all

install: 
	make -C clients install
	make -C plugins all
	make -C server install

install-stripped: 
	make -C clients install-stripped
	make -C server install

clean:
	make -C common clean
	make -C plugins clean
	make -C clients clean
	make -C server clean
