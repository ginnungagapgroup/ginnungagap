# Copyright (C) 2011 Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `voelva'.

LIBNAME = util

include ../../Makefile.config

.PHONY: all clean dist-clean tests tests-clean

sources = xmem.c \
          xstring.c \
          xfile.c \
          parse_ini.c \
          byteswap.c \
          endian.c \
          cmdline.c \
          timer.c \
          refCounter.c \
          rng.c \
          bov.c \
          grafic.c \
          gadget.c \
          gadgetHeader.c \
          gadgetUtil.c \
          art.c \
          artHeader.c \
          stai.c \
          varArr.c \
          myTest.c

ifeq ($(WITH_MPI), "true")
sources += commScheme.c \
           commSchemeBuffer.c \
           groupi.c
endif

sourcesTests = lib${LIBNAME}_tests.c \
               refCounter_tests.c \
               xstring_tests.c \
               endian_tests.c \
               bov_tests.c \
               grafic_tests.c \
               stai_tests.c \
               varArr_tests.c

ifeq ($(WITH_MPI), "true")
sourcesTests += commScheme_tests.c \
                commSchemeBuffer_tests.c \
                groupi_tests.c
endif


include ../../Makefile.rules

all:
	${MAKE} lib${LIBNAME}.a

clean:
	rm -f lib${LIBNAME}.a $(sources:.c=.o)
	rm -f lib${LIBNAME}_tests $(sourcesTests:.c=.o)

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d) $(sourcesTests:.c=.d)

# The MPI tests must be run with 16 processes, otherwise the tests will
# produce bogus results
tests:
	$(MAKE) lib${LIBNAME}_tests
ifeq ($(WITH_MPI), "true")
	mpiexec -n 16 ./lib${LIBNAME}_tests
else
	./lib${LIBNAME}_tests
endif

tests-clean:
	rm -f lib${LIBNAME}_tests $(sourcesTests:.c=.o)
	rm -f writeTest.grafic writeWindowed.grafic empty.grafic gmon.out groupiTest.*

lib${LIBNAME}_tests: lib${LIBNAME}.a \
                     $(sourcesTests:.c=.o)
	$(CC) $(CFLAGS) $(LDFLAGS) -o lib${LIBNAME}_tests \
	   $(sourcesTests:.c=.o) \
	   lib${LIBNAME}.a

lib${LIBNAME}.a: $(sources:.c=.o)
	$(AR) -rs lib${LIBNAME}.a $(sources:.c=.o)

-include $(sources:.c=.d)

-include $(sourcesTests:.c=.d)
