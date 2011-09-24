# Copyright (C) 2010, 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean tests tests-clean dist-clean

progName = makeMask

sources = main.c \
          $(progName).c \
          makeMaskSetup.c

include ../../Makefile.rules

all:
	$(MAKE) $(progName)

clean:
	rm -f $(progName) $(sources:.c=.o)

tests:
	@echo "No tests yet"

tests-clean:
	@echo "No tests yet to clean"

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d)

install: $(progName)
	mv -f $(progName) $(BINDIR)/

$(progName): $(sources:.c=.o) \
                     ../../src/liblare/liblare.a \
                     ../../src/libdata/libdata.a \
                     ../../src/libgrid/libgrid.a \
	                 ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $(progName) $(sources:.c=.o) \
                     ../../src/liblare/liblare.a \
	                 ../../src/libdata/libdata.a \
	                 ../../src/libgrid/libgrid.a \
	                 ../../src/libutil/libutil.a \
	                 $(LIBS)

-include $(sources:.c=.d)

../../src/liblare/liblare.a:
	$(MAKE) -C ../../src/liblare

../../src/libdata/libdata.a:
	$(MAKE) -C ../../src/libgrid

../../src/libgrid/libgrid.a:
	$(MAKE) -C ../../src/libgrid

../../src/libutil/libutil.a:
	$(MAKE) -C ../../src/libutil
