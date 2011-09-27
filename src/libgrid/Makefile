# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

LIBNAME = grid

include ../../Makefile.config

.PHONY: all clean dist-clean tests tests-clean

sources = gridRegular.c \
          gridRegularDistrib.c \
          gridRegularFFT.c \
          gridPatch.c \
          gridHistogram.c \
          gridStatistics.c \
          gridIO.c \
          gridReader.c \
          gridReaderBov.c \
          gridReaderGrafic.c \
          gridWriter.c \
          gridWriterGrafic.c \
          gridUtil.c

sourcesTests = lib${LIBNAME}_tests.c \
               gridRegular_tests.c \
               gridRegularDistrib_tests.c \
               gridRegularFFT_tests.c \
               gridPatch_tests.c \
               gridHistogram_tests.c \
               gridStatistics_tests.c \
               gridIO_tests.c \
               gridReader_tests.c \
               gridReaderBov_tests.c \
               gridUtil_tests.c

ifeq ($(WITH_SILO), "true")
sources += gridWriterSilo.c \
           gridReaderSilo.c

sourcesTests += gridReaderSilo_tests.c \
                gridWriterSilo_tests.c
endif

ifeq ($(WITH_HDF5), "true")
sources += gridWriterHDF5.c \
           gridReaderHDF5.c

sourcesTests += gridReaderHDF5_tests.c \
                gridWriterHDF5_tests.c
endif

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
	rm -rf siloTest*
	rm -rf transposeTest*
	rm -rf fftTest*


lib${LIBNAME}_tests: lib${LIBNAME}.a \
                     ../libdata/libdata.a \
                     ../libutil/libutil.a \
                     $(sourcesTests:.c=.o)
	$(CC) $(CFLAGS) $(LDFLAGS) -o lib${LIBNAME}_tests \
	   $(sourcesTests:.c=.o) \
	   lib${LIBNAME}.a \
	   ../libdata/libdata.a \
	   ../libutil/libutil.a \
	   $(LIBS)

lib${LIBNAME}.a: $(sources:.c=.o)
	$(AR) -rs lib${LIBNAME}.a $(sources:.c=.o)

../libdata/libdata.a:
	$(MAKE) -C ../libdata

../libutil/libutil.a:
	$(MAKE) -C ../libutil

-include $(sources:.c=.d)

-include $(sourcesTests:.c=.d)
