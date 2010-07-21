# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean dist-clean \
        ../libutil/libutil.a \
        ../libgrid/libgrid.a \
        ../libcosmo/licosmo.a

progName = ginnungagap

sources = main.c \
          $(progName).c \
          ginnungagapSetup.c \
          ginnungagapInit.c \
          ginnungagapWN.c \
          ginnungagapIC.c

include ../../Makefile.rules

all:
	$(MAKE) $(progName)

clean:
	rm -f $(progName) $(sources:.c=.o)

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d)

$(progName): $(sources:.c=.o) \
	                 ../libgrid/libgrid.a \
	                 ../libcosmo/libcosmo.a \
	                 ../libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $(progName) $(sources:.c=.o) \
	                 ../libgrid/libgrid.a \
	                 ../libcosmo/libcosmo.a \
	                 ../libutil/libutil.a \
	                 $(LIBS)

-include $(sources:.c=.d)

../libgrid/libgrid.a:
	$(MAKE) -C ../libgrid

../libcosmo/libcosmo.a:
	$(MAKE) -C ../libcosmo

../libutil/libutil.a:
	$(MAKE) -C ../libutil
