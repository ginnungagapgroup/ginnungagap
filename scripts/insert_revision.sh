#!/bin/sh
# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.

if [ -d .svn ]
then
	REVISION="r`svnversion -h`"
elif [ -d .git ]
then
	REVISION="commit `git-show-ref | awk '{print $1}'`"
else
	REVISION=""
fi
sed s/__REVISION__/$REVISION/ version.h.in > version.h
