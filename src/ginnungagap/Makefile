# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean dist-clean

progName = ginnungagap

# g9p is short for ginnungagap; g, 9 other characters, p
sources = main.c \
          $(progName).c \
          g9pSetup.c \
          g9pInit.c \
          g9pWN.c \
          g9pIC.c \
          g9pNorm.c

include ../../Makefile.rules

all:
	$(MAKE) $(progName)

clean:
	rm -f $(progName) $(sources:.c=.o)

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d)

install: $(progName)
	mv -f $(progName) $(BINDIR)/

$(progName): $(sources:.c=.o) \
	                 ../libdata/libdata.a \
	                 ../libgrid/libgrid.a \
	                 ../libcosmo/libcosmo.a \
	                 ../libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $(progName) $(sources:.c=.o) \
	                 ../libdata/libdata.a \
	                 ../libgrid/libgrid.a \
	                 ../libcosmo/libcosmo.a \
	                 ../libutil/libutil.a \
	                 $(LIBS)

-include $(sources:.c=.d)

../libdata/libdata.a:
	$(MAKE) -C ../libdata

../libgrid/libgrid.a:
	$(MAKE) -C ../libgrid

../libcosmo/libcosmo.a:
	$(MAKE) -C ../libcosmo

../libutil/libutil.a:
	$(MAKE) -C ../libutil
