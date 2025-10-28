# Copyright (C) 2010, 2011, Steffen Knollmann, 2025, Sergey Pilipenko
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean tests tests-clean dist-clean

progName = graficCoord

sources = $(progName).c

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

$(progName): $(sources)
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $(progName) $(sources) -lm
	

-include $(sources:.c=.d)

