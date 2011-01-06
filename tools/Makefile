# Copyright (C) 2010, 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../Makefile.config

.PHONY: all clean tests tests-clean dist-clean

all:
	@echo ""
	@echo ""
	@echo "+-------------------------------+"
	@echo "|   Working on the tools        |"
	@echo "+-------------------------------+"
	@echo ""
	$(MAKE) -C estimateMemReq all
	$(MAKE) -C makeSiloRoot all
	$(MAKE) -C grafic2gadget all
	$(MAKE) -C grafic2bov all
	$(MAKE) -C showFreqs all
	@echo ""
	@echo "+-------------------------------+"
	@echo "|   Done with the tools         |"
	@echo "+-------------------------------+"
	@echo ""
	@echo ""

clean:
	$(MAKE) -C estimateMemReq clean
	$(MAKE) -C grafic2gadget clean
	$(MAKE) -C grafic2bov clean
	$(MAKE) -C makeSiloRoot clean
	$(MAKE) -C showFreqs clean

tests:
	$(MAKE) -C estimateMemReq tests
	$(MAKE) -C grafic2gadget tests
	$(MAKE) -C grafic2bov tests
	$(MAKE) -C makeSiloRoot tests
	$(MAKE) -C showFreqs tests

tests-clean:
	$(MAKE) -C estimateMemReq tests-clean
	$(MAKE) -C grafic2gadget tests-clean
	$(MAKE) -C grafic2bov tests-clean
	$(MAKE) -C makeSiloRoot tests-clean
	$(MAKE) -C showFreqs tests-clean

dist-clean:
	$(MAKE) -C estimateMemReq dist-clean
	$(MAKE) -C grafic2gadget dist-clean
	$(MAKE) -C grafic2bov dist-clean
	$(MAKE) -C makeSiloRoot dist-clean
	$(MAKE) -C showFreqs dist-clean
