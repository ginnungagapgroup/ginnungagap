# Copyright (C) 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

include ../../Makefile.config

.PHONY: all clean tests tests-clean dist-clean \
        ../../src/libutil/libutil.a

sources = art_peekHeader.c \
          art_createTestFile.c \
          art_dumpSelection.c \
          art_describeFile.c \
          cubepm_describeFile.c \
          cubepm_dumpSelection.c \
          gadget_describeFile.c \
          gadget_peekHeader.c \
          gadget_dumpSelection.c


include ../../Makefile.rules

all:
	$(MAKE) $(sources:.c=)

clean:
	rm -f $(sources:.c=) $(sources:.c=.o)

tests:
	@echo "No tests for fileTools yet"

tests-clean:
	@echo "Nothing to clean for tests of fileTools"

dist-clean:
	$(MAKE) clean
	rm -f $(sources:.c=.d)

install: $(sources:.c=)
	mv $(sources:.c=) $(BINDIR)/

art_peekHeader: art_peekHeader.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)

art_createTestFile: art_createTestFile.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)

art_dumpSelection: art_dumpSelection.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)

art_describeFile: art_describeFile.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)

cubepm_describeFile: cubepm_describeFile.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)

cubepm_dumpSelection: cubepm_dumpSelection.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)

gadget_peekHeader: gadget_peekHeader.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)

gadget_describeFile: gadget_describeFile.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)

gadget_dumpSelection: gadget_dumpSelection.o \
                ../../src/libutil/libutil.a
	$(CC) $(LDFLAGS) $(CFLAGS) \
	  -o $@ $(@).o ../../src/libutil/libutil.a  $(LIBS)


-include $(sources:.c=.d)

../../src/libutil/libutil.a:
	$(MAKE) -C ../../src/libutil
