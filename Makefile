# Copyright (C) 2010, 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

###########################################################################
##  NOTE:   Normally you should not be required to edit this file.       ##
##          The setting of compilers and additional programs needed      ##
##          for the build process is done in 'Makefile.config' in        ##
##          this directory.  Compile time options are set in             ##
##          'config.h'.                                                  ##
###########################################################################

ifeq ($(shell if test -e Makefile.config ; then echo "true" ; fi),true)
CONFIG_AVAILABLE=true
else
CONFIG_AVAILABLE=false
endif

ifeq ($(shell if test -d .git ; then echo "true" ; fi),true)
GITDIR_AVAILABLE=true
else
GITDIR_AVAILABLE=false
endif

.PHONY: clean dist-clean \
        doc doc-clean doc-dist-clean \
        tests tests-clean \
        tags tarball statistics

ifeq ($(CONFIG_AVAILABLE),true)
include Makefile.config

all: version.h
	$(MAKE) -C src
	$(MAKE) -C tools

clean:
	touch version.h
	$(MAKE) -C src clean
	$(MAKE) -C tools clean

dist-clean:
	$(MAKE) -C src dist-clean
	$(MAKE) -C tools dist-clean
	find . -name *.d.[0-9]* -exec rm {} \;
ifeq ($(GITDIR_AVAILABLE),true)
	rm -f version.h
endif
	rm -f Makefile.config config.h config.log
	rm -rf $(BINDIR)

tests:
	$(MAKE) -C src tests
	$(MAKE) -C tools tests

tests-clean:
	$(MAKE) -C src tests-clean
	$(MAKE) -C tools tests-clean

install: version.h
	mkdir -p $(BINDIR)
	$(MAKE) -C src install
	$(MAKE) -C tools install

version.h: version.h.in
ifeq ($(GITDIR_AVAILABLE),true)
	scripts/insert_revision.sh
endif
	scripts/insert_builtinfo.sh

else
all:
	@echo -n "Please run ./configure first "
	@echo "(check ./configure --help for options)"
endif

ifeq ($(GITDIR_AVAILABLE),true)
tarball:
	@scripts/insert_revision.sh
	@scripts/make_tar.sh
	@rm -f version.h
endif

statistics:
	scripts/show-statistics.sh

doc:
	$(MAKE) -C doc

doc-clean:
	$(MAKE) -C doc clean

doc-dist-clean:
	$(MAKE) -C doc dist-clean

tags:
	etags --recurse src/ tools/
	cscope -b -R
