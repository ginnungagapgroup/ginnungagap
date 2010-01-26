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

include Makefile.config

.PHONY: all clean dist-clean doc tarball

all:
	scripts/insert_revision.sh
	$(MAKE) -C src

clean:
	touch version.h
	$(MAKE) -C src clean

dist-clean:
	$(MAKE) -C src dist-clean
	find . -name *.d.[0-9]* -exec rm {} \;
	@rm -f version.h

tarball:
	@scripts/insert_revision.sh
	@scripts/make_tar.sh
