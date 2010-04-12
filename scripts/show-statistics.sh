#!/bin/bash

# Copyright (C) 2010, Steffen Knollmann
# Released under the terms of the GNU General Public License version 3.
# This file is part of `ginnungagap'.

function showForDir ()
{
	find $1 -name \*.[ch] -exec printf "%s\0" {} \; \
		| wc --files0-from=- > $$.1.dat

	find $1 -name \*tests.[ch] -exec printf "%s\0" {} \; \
		| wc --files0-from=- > $$.2.dat

	if [[ x$1 == x. ]]
	then
		echo "Total:"
	else
		echo "$1:"
	fi
	echo -n "  Files: $(grep -v total $$.1.dat | wc -l)"
	echo " ($(grep -v total $$.2.dat | wc -l))"
	echo -n "  Lines: $(grep total $$.1.dat | awk '{print $1}')"
	echo " ($(grep total $$.2.dat | awk '{print $1}'))"
	echo -n "  Words: $(grep total $$.1.dat | awk '{print $2}')"
	echo " ($(grep total $$.2.dat | awk '{print $2}'))"
	echo -n "  Chars: $(grep total $$.1.dat | awk '{print $3}')"
	echo " ($(grep total $$.2.dat | awk '{print $3}'))"
	echo ""

	rm -rf $$.1.dat $$.2.dat
}

echo ""
showForDir src/ginnungagap
showForDir src/libgrid
showForDir src/libcosmo
showForDir src/libutil
showForDir src
echo ""
echo "Numbers in parenthesis are test code only."
echo ""
