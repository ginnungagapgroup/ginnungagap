# Copyright (C) 2012, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

LIBNAME = g9p

include ../../Makefile.config

.PHONY: all clean dist-clean tests tests-clean

sources = g9pHierarchy.c \
          g9pHierarchyIO.c \
          g9pMask.c \
          g9pMaskIO.c

sourcesTests = lib${LIBNAME}_tests.c \
               g9pHierarchy_tests.c \
               g9pHierarchyIO_tests.c \
               g9pMask_tests.c \
               g9pMaskIO_tests.c

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
	$(MPIEXEC) -n 5 ./lib${LIBNAME}_tests
else
	./lib${LIBNAME}_tests
endif

tests-clean:
	rm -f lib${LIBNAME}_tests $(sourcesTests:.c=.o) 

lib${LIBNAME}_tests: lib${LIBNAME}.a \
                     ../libgrid/libgrid.a \
                     ../libdata/libdata.a \
                     ../libutil/libutil.a \
                     $(sourcesTests:.c=.o)
	$(CC) $(CFLAGS) $(LDFLAGS) -o lib${LIBNAME}_tests \
	   $(sourcesTests:.c=.o) \
	   lib${LIBNAME}.a \
	   ../libgrid/libgrid.a \
	   ../libdata/libdata.a \
	   ../libutil/libutil.a \
	   $(LIBS)

lib${LIBNAME}.a: $(sources:.c=.o)
	$(AR) -rs lib${LIBNAME}.a $(sources:.c=.o)

../libutil/libutil.a:
	$(MAKE) -C ../libutil

../libgrid/libgrid.a:
	$(MAKE) -C ../libgrid

../libdata/libdata.a:
	$(MAKE) -C ../libdata

-include $(sources:.c=.d)

-include $(sourcesTests:.c=.d)
