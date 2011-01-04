# Copyright (C) 2010, 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

ifeq ($(shell if test -e ../Makefile.config ; then echo "true" ; fi),true)
CONFIG_AVAILABLE=true
else
CONFIG_AVAILABLE=false
endif

ifeq ($(CONFIG_AVAILABLE), true)
include ../Makefile.config
endif

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
