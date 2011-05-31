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
	@mkdir -p ref/
#	This is a bit stupid, but doxygen needs to be executed from the
#	main directory so that $PWD during its execution is set in such a
#	way that the identical leading part of all investigated files is
#	set correctly; i.e. it should strip everything up to the main
#	directory and only lists the files as, e.g., src/libcosmos/bla.h
#	instead of /full/path/to/ginnungagap/src/libcosmo/bla.h.
#	For more details see documentation of FULL_PATH_NAMES and
#	STRIP_FROM_PATH of doxygen.
	@echo "Running doxygen."
	@echo "  logfile is at doc/doxygen.log"
	@echo "  warnings are at doc/doxygen.warning"
	@echo ""
	@echo -n "  This will now take a while, go fetch a coffee...  "
	@cd .. ; $(DOXYGEN) doc/Doxyfile > doc/doxygen.log ; cd doc
	@echo "done!"
	@echo ""
	@echo -n "  Total number of warnings:  "
	@cat doxygen.warning | wc -l
	@echo ""

clean:
	rm -f Doxyfile doxygen.warning doxygen.log
	rm -rf ref/

dist-clean:
	$(MAKE) clean
