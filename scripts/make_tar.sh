#!/bin/bash

# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

# Verify that this is a proper checkout to work from
if [[ ! -d .git ]]
then
	echo "This does not seem to be a proper repository, hence we cannot"
	echo "obtain a proper file list.  Please have a git clone of the"
	echo "repository before using this script."
	exit 1
fi

# Set the temporary directory
if [[ x$TEMPDIR != x ]]
then
	MYTEMPDIR=$TEMPDIR
else
	MYTEMPDIR=/tmp
fi
echo "Using $MYTEMPDIR as temporary directory."

# Extract the version info
NAME=$(  grep 'define PACKAGE_NAME' version.h.in | awk '{print $3}' \
       | sed 's/"\(.*\)"/\1/')
VER_MAJOR=$(  grep 'define PACKAGE_VERSION_MAJOR' version.h.in \
            | awk '{print $3}')
VER_MINOR=$(  grep 'define PACKAGE_VERSION_MINOR' version.h.in \
            | awk '{print $3}')
VER_MICRO=$(  grep 'define PACKAGE_VERSION_MICRO' version.h.in \
            | awk '{print $3}')
PACKAGE_STATE=$(  grep 'define PACKAGE_STATE' version.h.in \
                | awk '{print $3}')

# Get the tarball name prefix
case $PACKAGE_STATE in
	0)
		TARBALLNAME=$NAME-$VER_MAJOR.$VER_MINOR.$VER_MICRO-alpha
		;;
	1)
		TARBALLNAME=$NAME-$VER_MAJOR.$VER_MINOR.$VER_MICRO-beta
		;;
	*)
		TARBALLNAME=$NAME-$VER_MAJOR.$VER_MINOR.$VER_MICRO
esac

# Get a temporary name
TMPNAME=$(date +"%y%m%M-%H%M%S")

# Print what will happen:
echo -n "Generating $TARBALLNAME"

# Generate the temporary directories
mkdir -p $MYTEMPDIR/$TMPNAME/$TARBALLNAME

# Get a list of all files in the repository
git ls-files | grep -v .gitignore > $MYTEMPDIR/$TMPNAME/files

# And add the processed version.h
echo "version.h" >> $MYTEMPDIR/$TMPNAME/files

## Generate the ChangeLog file
#git log --stat > ChangeLog
#echo "ChangeLog" >> $MYTEMPDIR/$TMPNAME/files

# Sync the repository to the temporary directory
rsync -aq --files-from=$MYTEMPDIR/$TMPNAME/files . $MYTEMPDIR/$TMPNAME/$TARBALLNAME

# Go there, create the tarball and return
pushd $MYTEMPDIR/$TMPNAME/ > /dev/null
tar cjf $TARBALLNAME.tar.bz2 $TARBALLNAME
popd > /dev/null

# Move the tarball over
mv $MYTEMPDIR/$TMPNAME/$TARBALLNAME.tar.bz2 .

# Clean up
rm -rf $MYTEMPDIR/$TMPNAME

# ..and say bye
SIZE=$(ls -sh ${TARBALLNAME}.tar.bz2 | awk '{print $1}')
echo "       done ($SIZE)."
