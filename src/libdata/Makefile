# Copyright (C) 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

LIBNAME = data

include ../../Makefile.config

.PHONY: all clean dist-clean tests tests-clean

sources = dataVar.c \
          dataVarType.c \
          dataParticle.c

sourcesTests = lib${LIBNAME}_tests.c \
               dataVar_tests.c \
               dataVarType_tests.c \
               dataParticle_tests.c

ifeq ($(WITH_MPI), "true")
CC=$(MPICC)
endif

include ../../Makefile.rules

all:
	${MAKE} lib${LIBNAME}.a

clean:
	$(MAKE) tests-clean
	rm -f lib${LIBNAME}.a $(sources:.c=.o)

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d) $(sourcesTests:.c=.d)

tests:
	$(MAKE) lib${LIBNAME}_tests
ifeq ($(WITH_MPI), "true")
	$(MPIEXEC) -n 16 ./lib${LIBNAME}_tests
else
	./lib${LIBNAME}_tests
endif

tests-clean:
	rm -f lib${LIBNAME}_tests $(sourcesTests:.c=.o) 


lib${LIBNAME}_tests: lib${LIBNAME}.a \
                     ../libutil/libutil.a \
                     $(sourcesTests:.c=.o)
	$(CC) $(CFLAGS) $(LDFLAGS) -o lib${LIBNAME}_tests \
	   $(sourcesTests:.c=.o) \
	   lib${LIBNAME}.a \
	   ../libutil/libutil.a \
	   $(LIBS)

lib${LIBNAME}.a: $(sources:.c=.o)
	$(AR) -rs lib${LIBNAME}.a $(sources:.c=.o)

../libutil/libutil.a:
	$(MAKE) -C ../libutil

-include $(sources:.c=.d)

-include $(sourcesTests:.c=.d)
