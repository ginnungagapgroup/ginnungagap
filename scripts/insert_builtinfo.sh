#!/bin/sh

# Copyright (C) 2011, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

# This script inserts the built information retrieved by using the shell
# commands 'date', 'whoami', 'hostname', and 'pwd' into version.h.
#

BUILT_USER=`whoami`
BUILT_DATE=`date`
BUILT_HOST=`hostname`
BUILT_DIR=`pwd`


sed -i.bak -e "s@__BUILT_USER__@$BUILT_USER@" \
           -e "s@__BUILT_DATE__@$BUILT_DATE@" \
           -e "s@__BUILT_HOST__@$BUILT_HOST@" \
           -e "s@__BUILT_DIR__@$BUILT_DIR@" version.h
rm version.h.bak
