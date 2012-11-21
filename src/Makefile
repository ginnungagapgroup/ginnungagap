# Copyright (C) 2010, 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../Makefile.config

.PHONY: all clean tests tests-clean dist-clean install

all:
	@echo ""
	@echo ""
	@echo "+-------------------------------+"
	@echo "|   Working on the main code    |"
	@echo "+-------------------------------+"
	@echo ""
	$(MAKE) -C libutil all
	$(MAKE) -C libcosmo all
	$(MAKE) -C libdata all
	$(MAKE) -C libgrid all
	$(MAKE) -C libpart all
	$(MAKE) -C liblare all
	$(MAKE) -C libg9p all
	$(MAKE) -C ginnungagap all
	@echo ""
	@echo "+-------------------------------+"
	@echo "|   Done with the main code     |"
	@echo "+-------------------------------+"
	@echo ""
	@echo ""

clean:
	$(MAKE) -C ginnungagap clean
	$(MAKE) -C libg9p clean
	$(MAKE) -C liblare clean
	$(MAKE) -C libpart clean
	$(MAKE) -C libgrid clean
	$(MAKE) -C libdata clean
	$(MAKE) -C libcosmo clean
	$(MAKE) -C libutil clean

tests:
	$(MAKE) -C libutil tests
	$(MAKE) -C libcosmo tests
	$(MAKE) -C libdata tests
	$(MAKE) -C libgrid tests
	$(MAKE) -C libpart tests
	$(MAKE) -C liblare tests
	$(MAKE) -C libg9p tests

tests-clean:
	$(MAKE) -C libutil tests-clean
	$(MAKE) -C libcosmo tests-clean
	$(MAKE) -C libdata tests-clean
	$(MAKE) -C libgrid tests-clean
	$(MAKE) -C libpart tests-clean
	$(MAKE) -C liblare tests-clean
	$(MAKE) -C libg9p tests-clean

dist-clean:
	$(MAKE) -C ginnungagap dist-clean
	$(MAKE) -C libg9p dist-clean
	$(MAKE) -C liblare dist-clean
	$(MAKE) -C libpart dist-clean
	$(MAKE) -C libgrid dist-clean
	$(MAKE) -C libdata dist-clean
	$(MAKE) -C libcosmo dist-clean
	$(MAKE) -C libutil dist-clean

install:
	$(MAKE) -C ginnungagap install
