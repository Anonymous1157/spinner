LDFLAGS=-lm
DESTDIR=$(HOME)/.local

spinner: spinner.c

.PHONY: clean install uninstall
clean:
	rm -rf spinner
install: spinner
	install -m755 spinner $(DESTDIR)/bin
uninstall:
	rm -rf $(DESTDIR)/bin/spinner
