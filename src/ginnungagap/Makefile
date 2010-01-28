# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean dist-clean ../libutil/libutil.a

progName = ginnungagap

sources = main.c \
          $(progName).c

include ../../Makefile.rules

all:
	$(MAKE) $(progName)

clean:
	rm -f $(progName) $(sources:.c=.o)

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d)

$(progName): $(sources:.c=.o)
	$(CC) $(CFLAGS) -o $(progName) $(sources:.c=.o) \
	                               ../libutil/libutil.a

-include $(sources:.c=.d)

../libutil/libutil.a:
	$(MAKE) -C ../libutil
