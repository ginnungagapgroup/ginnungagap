# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../Makefile.config

.PHONY: all clean tests dist-clean

all:
	@echo ""
	@echo ""
	@echo "+-------------------------------+"
	@echo "|   Working on the tools        |"
	@echo "+-------------------------------+"
	@echo ""
	$(MAKE) -C makeSiloRoot all
	@echo ""
	@echo "+-------------------------------+"
	@echo "|   Done with the tools         |"
	@echo "+-------------------------------+"
	@echo ""
	@echo ""

clean:
	$(MAKE) -C makeSiloRoot clean

tests:
	$(MAKE) -C makeSiloRoot tests

dist-clean:
	$(MAKE) -C makeSiloRoot dist-clean
