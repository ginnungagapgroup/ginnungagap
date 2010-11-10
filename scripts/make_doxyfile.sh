#!/bin/bash

# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

# Verify that this is a proper checkout to work from
if [[ -a ../version.h ]]
then
	PROPER_VERSION_INFO="yes"
	VERSIONFILE=../version.h
else
	PROPER_VERSION_INFO="no"
	VERSIONFILE=../version.h.in
fi

# Extract the version info
PROJECT_NAME=$(  grep 'define PACKAGE_NAME' $VERSIONFILE \
               | awk '{print $3}')
VER_MAJOR=$(  grep 'define PACKAGE_VERSION_MAJOR' $VERSIONFILE \
            | awk '{print $3}')
VER_MINOR=$(  grep 'define PACKAGE_VERSION_MINOR' $VERSIONFILE \
            | awk '{print $3}')
VER_MICRO=$(  grep 'define PACKAGE_VERSION_MICRO' $VERSIONFILE \
            | awk '{print $3}')
if [[ x$PROPER_VERSION_INFO ==  x"yes" ]]
then
	VER_REVIS=$(  grep 'define PACKAGE_VERSION_REVISION' $VERSIONFILE \
	             | awk '{print "-r"$3}' | sed s/\"//g)
else
	echo "Warning!  Using incomplete version data, version.h does not exist."
	echo "          Probably you are not working with a release version"
	echo "          and hence know how to fix that."
	VER_REVIS=""
fi

# Get the complete version string
PROJECT_NUMBER="$VER_MAJOR.$VER_MINOR.$VER_MICRO$VER_REVIS"

# Insert that into the Doxyfile
cat Doxyfile.in \
	| sed -e "s/__PROJECT_NUMBER__/$PROJECT_NUMBER/" \
	      -e "s/__PROJECT_NAME__/$PROJECT_NAME/" > Doxyfile
