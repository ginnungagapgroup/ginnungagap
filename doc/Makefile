include ../Makefile.config

DOXYGEN?=doxygen

.PHONY: all doc clean dist-clean

all:
	$(MAKE) doc

doc:
	@../scripts/make_doxyfile.sh
	$(DOXYGEN) Doxyfile

clean:
	rm -f Doxyfile
	rm -rf ref/

dist-clean:
	$(MAKE) clean
