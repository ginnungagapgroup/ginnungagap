# Copyright (C) 2010, Steffen Knollmann
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. 


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
	$(MAKE) clean
	find . -name *.d.[0-9]* -exec rm {} \;
	@rm -f version.h

tarball:
	@scripts/insert_revision.sh
	@scripts/make_tar.sh
