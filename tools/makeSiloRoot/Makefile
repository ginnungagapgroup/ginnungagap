# Copyright (C) 2010, 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean tests tests-clean dist-clean

progName = makeSiloRoot

sources = main.c \
          $(progName).c \
          siloRoot.c

include ../../Makefile.rules

ifeq ($(WITH_SILO), "true")
all:
	$(MAKE) $(progName)
else
all:
	@echo "To use \`$(progName)' --with-silo has to be defined."
	@echo "See ./configure --help for details."
	@echo ""
	@echo "I will omit building it."
endif

clean:
	rm -f $(progName) $(sources:.c=.o)

tests:
	@echo "No tests yet"

tests-clean:
	@echo "No tests yet to clean"

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d)

ifeq ($(WITH_SILO), "true")
$(progName): $(sources:.c=.o) \
	                 ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $(progName) $(sources:.c=.o) \
	                 ../../src/libutil/libutil.a \
	                 $(LIBS)

-include $(sources:.c=.d)

../../src/libutil/libutil.a:
	$(MAKE) -C ../../src/libutil
endif
