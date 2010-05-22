# Copyright (C) 2010, Steffen Knollmann
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

.PHONY: all clean dist-clean doc tarball statistics tests

ifeq ($(CONFIG_AVAILABLE),true)
include Makefile.config

all:
	scripts/insert_revision.sh
	$(MAKE) -C src

clean:
	touch version.h
	$(MAKE) -C src clean

dist-clean:
	$(MAKE) -C src dist-clean
	find . -name *.d.[0-9]* -exec rm {} \;
	@rm -f version.h Makefile.config config.h config.log

tests:
	$(MAKE) -C src tests
endif

tarball:
	@scripts/insert_revision.sh
	@scripts/make_tar.sh
	@rm -f version.h

statistics:
	@scripts/show-statistics.sh
