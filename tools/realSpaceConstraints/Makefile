# Copyright (C) 2010, 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean tests tests-clean dist-clean

progName = realSpaceConstraints

sources = main.c \
          $(progName).c \
          realSpaceConstraintsSetup.c

ifeq ($(WITH_MPI), "true")
CC=$(MPICC)
endif

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
                     ../../src/libgrid/libgrid.a \
                     ../../src/libdata/libdata.a \
	                 ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $(progName) $(sources:.c=.o) \
	                 ../../src/libgrid/libgrid.a \
	                 ../../src/libdata/libdata.a \
	                 ../../src/libutil/libutil.a \
	                 $(LIBS)

-include $(sources:.c=.d)

../../src/libgrid/libgrid.a:
	$(MAKE) -C ../../src/libgrid

../../src/libdata/libdata.a:
	$(MAKE) -C ../../src/libdata

../../src/libutil/libutil.a:
	$(MAKE) -C ../../src/libutil
