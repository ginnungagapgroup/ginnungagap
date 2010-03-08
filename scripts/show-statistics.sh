#!/bin/sh

# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

find . -name \*.[ch] -exec printf "%s\0" {} \; \
	| wc --files0-from=- > $$.1.dat

find . -name \*tests.[ch] -exec printf "%s\0" {} \; \
	| wc --files0-from=- > $$.2.dat

echo "Total source code size of ginnungagap:"
echo -n "  Lines: `grep total $$.1.dat | awk '{print $1}'`"
echo " (`grep total $$.2.dat | awk '{print $1}'`)"
echo -n "  Words: `grep total $$.1.dat | awk '{print $2}'`"
echo " (`grep total $$.2.dat | awk '{print $2}'`)"
echo -n "  Chars: `grep total $$.1.dat | awk '{print $3}'`"
echo " (`grep total $$.2.dat | awk '{print $3}'`)"
echo ""
echo "Numbers in parenthesis are test code only."
echo ""

rm -rf $$.1.dat $$.2.dat
