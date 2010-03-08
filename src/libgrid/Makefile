# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

LIBNAME = grid

include ../../Makefile.config

.PHONY: all clean dist-clean tests

sources = gridRegular.c \
          gridVar.c \
          gridVarTypes.c

sourcesTests = lib${LIBNAME}_tests.c \
               gridRegular_tests.c \
               gridVar_tests.c \
               gridVarTypes_tests.c

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
ifeq ($(WITH_MPI), "true")
	mpiexec -n 16 ./lib${LIBNAME}_tests
else
	./lib${LIBNAME}_tests
endif


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
