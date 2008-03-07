CGIDIR=/usr/lib/cgi-bin
#BINDIR=$(HOME)/bin
BINDIR=/usr/sbin
PROGS=googd

all: $(PROGS) 

install: $(PROGS)
	cp googd $(DESTDIR)$(BINDIR)
	chmod a+rx $(DESTDIR)$(BINDIR)/googd

install-stripped: $(PROGS)
	cp googd $(DESTDIR)$(BINDIR)
	chmod a+rx $(DESTDIR)$(BINDIR)/googd
	
clean:
	rm -f $(OBJECTS) $(PROGS) *~

