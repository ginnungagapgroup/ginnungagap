# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../Makefile.config

.PHONY: all clean dist-clean

all:
	$(MAKE) -C libutil all
	$(MAKE) -C ginnungagap all

clean:
	$(MAKE) -C ginnungagap clean
	$(MAKE) -C libutil clean

dist-clean:
	$(MAKE) -C ginnungagap dist-clean
	$(MAKE) -C libutil dist-clean
