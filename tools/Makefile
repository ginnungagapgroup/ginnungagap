# Copyright (C) 2010, 2011, 2012, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../Makefile.config

.PHONY: all clean tests tests-clean dist-clean install

all:
	@echo ""
	@echo ""
	@echo "+-------------------------------+"
	@echo "|   Working on the tools        |"
	@echo "+-------------------------------+"
	@echo ""
	$(MAKE) -C estimateMemReq all
	$(MAKE) -C makeSiloRoot all
	$(MAKE) -C generateICs all
	$(MAKE) -C grafic2gadget all
	$(MAKE) -C grafic2bov all
	$(MAKE) -C showFreqs all
	$(MAKE) -C makeMask all
	$(MAKE) -C realSpaceConstraints all
	$(MAKE) -C fileTools all
	@echo ""
	@echo "+-------------------------------+"
	@echo "|   Done with the tools         |"
	@echo "+-------------------------------+"
	@echo ""
	@echo ""

clean:
	$(MAKE) -C estimateMemReq clean
	$(MAKE) -C generateICs clean
	$(MAKE) -C grafic2gadget clean
	$(MAKE) -C grafic2bov clean
	$(MAKE) -C makeSiloRoot clean
	$(MAKE) -C showFreqs clean
	$(MAKE) -C makeMask clean
	$(MAKE) -C realSpaceConstraints clean
	$(MAKE) -C fileTools clean

tests:
	$(MAKE) -C estimateMemReq tests
	$(MAKE) -C generateICs tests
	$(MAKE) -C grafic2gadget tests
	$(MAKE) -C grafic2bov tests
	$(MAKE) -C makeSiloRoot tests
	$(MAKE) -C showFreqs tests
	$(MAKE) -C makeMask tests
	$(MAKE) -C realSpaceConstraints tests
	$(MAKE) -C fileTools tests

tests-clean:
	$(MAKE) -C estimateMemReq tests-clean
	$(MAKE) -C generateICs tests-clean
	$(MAKE) -C grafic2gadget tests-clean
	$(MAKE) -C grafic2bov tests-clean
	$(MAKE) -C makeSiloRoot tests-clean
	$(MAKE) -C showFreqs tests-clean
	$(MAKE) -C makeMask tests-clean
	$(MAKE) -C realSpaceConstraints tests-clean
	$(MAKE) -C fileTools tests-clean

dist-clean:
	$(MAKE) -C estimateMemReq dist-clean
	$(MAKE) -C generateICs dist-clean
	$(MAKE) -C grafic2gadget dist-clean
	$(MAKE) -C grafic2bov dist-clean
	$(MAKE) -C makeSiloRoot dist-clean
	$(MAKE) -C showFreqs dist-clean
	$(MAKE) -C makeMask dist-clean
	$(MAKE) -C realSpaceConstraints dist-clean
	$(MAKE) -C fileTools dist-clean

install:
	$(MAKE) -C estimateMemReq install
	$(MAKE) -C generateICs install
	$(MAKE) -C grafic2gadget install
	$(MAKE) -C grafic2bov install
	$(MAKE) -C makeSiloRoot install
	$(MAKE) -C showFreqs install
	$(MAKE) -C makeMask install
	$(MAKE) -C realSpaceConstraints install
	$(MAKE) -C fileTools install
