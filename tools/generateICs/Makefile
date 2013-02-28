# Copyright (C) 2012, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean tests tests-clean dist-clean

progName = generateICs

sources = main.c \
          $(progName).c \
          $(progName)Data.c \
          $(progName)Mode.c \
          $(progName)In.c \
          $(progName)Out.c \
          $(progName)Factory.c

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
                     ../../src/libg9p/libg9p.a \
                     ../../src/libgrid/libgrid.a \
                     ../../src/libdata/libdata.a \
	                 ../../src/libcosmo/libcosmo.a \
	                 ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $(progName) $(sources:.c=.o) \
                     ../../src/libg9p/libg9p.a \
	                 ../../src/libgrid/libgrid.a \
	                 ../../src/libdata/libdata.a \
	                 ../../src/libcosmo/libcosmo.a \
	                 ../../src/libutil/libutil.a \
	                 $(LIBS)

-include $(sources:.c=.d)

../../src/libg9p/libg9p.a:
	$(MAKE) -C ../../src/libg9p

../../src/libgrid/libgrid.a:
	$(MAKE) -C ../../src/libgrid

../../src/libdata/libdata.a:
	$(MAKE) -C ../../src/libdata

../../src/libcosmo/libcosmo.a:
	$(MAKE) -C ../../src/libcosmo

../../src/libutil/libutil.a:
	$(MAKE) -C ../../src/libutil
