# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean tests dist-clean \
        ../../src/libcosmo/libcosmo.a \
        ../../src/libutil/libutil.a

progName = grafic2gadget

sources = main.c \
          $(progName).c

include ../../Makefile.rules

all:
	$(MAKE) $(progName)

clean:
	rm -f $(progName) $(sources:.c=.o)

tests:
	@echo "No tests yet"

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d)

$(progName): $(sources:.c=.o) \
	                 ../../src/libcosmo/libcosmo.a \
	                 ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $(progName) $(sources:.c=.o) \
	                 ../../src/libcosmo/libcosmo.a \
	                 ../../src/libutil/libutil.a \
	                 $(LIBS)

-include $(sources:.c=.d)

../../src/libcosmo/libcosmo.a:
	$(MAKE) -C ../../src/libcosmo

../../src/libutil/libutil.a:
	$(MAKE) -C ../../src/libutil