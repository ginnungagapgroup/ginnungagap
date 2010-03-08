# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

LIBNAME = util

include ../../Makefile.config

.PHONY: all clean dist-clean tests

sources = xmem.c \
          xstring.c \
          xfile.c \
          parse_ini.c \
          byteswap.c \
          cmdline.c \
          siloTools.c \
          varArr.c

sourcesTests = lib${LIBNAME}_tests.c \
               varArr_tests.c

include ../../Makefile.rules

all:
	${MAKE} lib${LIBNAME}.a

clean:
	rm -f lib${LIBNAME}.a $(sources:.c=.o)
	rm -f lib${LIBNAME}_tests $(sourcesTests:.c=.o)

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d) $(sourcesTests:.c=.d)

tests:
	$(MAKE) lib${LIBNAME}_tests
	./lib${LIBNAME}_tests

lib${LIBNAME}_tests: lib${LIBNAME}.a \
                     $(sourcesTests:.c=.o)
	$(CC) $(CFLAGS) $(LDFLAGS) -o lib${LIBNAME}_tests \
	   $(sourcesTests:.c=.o) \
	   lib${LIBNAME}.a

lib${LIBNAME}.a: $(sources:.c=.o)
	$(AR) -rs lib${LIBNAME}.a $(sources:.c=.o)

-include $(sources:.c=.d)

-include $(sourcesTests:.c=.d)
