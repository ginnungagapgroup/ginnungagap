# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

LIBNAME = util

include ../../Makefile.config

.PHONY: all clean dist-clean

sources = xmem.c \
          xstring.c \
          xfile.c \
          parse_ini.c \
          byteswap.c \
          cmdline.c

include ../../Makefile.rules

all:
	${MAKE} lib${LIBNAME}.a

clean:
	rm -f lib${LIBNAME}.a $(sources:.c=.o)

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d)

lib${LIBNAME}.a: $(sources:.c=.o)
	$(AR) -rs lib${LIBNAME}.a $(sources:.c=.o)

-include $(sources:.c=.d)
