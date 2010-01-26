#!/bin/sh
# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
#
# This script inserts the revision retrieved from a RCS into version.h,
# using version.h.in as a template.
#
# Works for:
#   - subversion
#   - git
#

SVNVERSION=`which svnversion`
GIT=`which git`

if [ -d .svn ] && [ x$SVNVERSION != x ] && [ -x $SVNVERSION ]
then
	REVISION="r`$SVNVERSION -h`"
elif [ -d .git ] && [ x$GIT != x ] && [ -x $GIT ]
then
	REVISION="\\\\ncommit `$GIT show-ref | awk '{print $1}'`"
else
	REVISION=""
fi
sed s/__REVISION__/"$REVISION"/ version.h.in > version.h
