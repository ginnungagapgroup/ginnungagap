// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#include "byteswap.h"


extern void
byteswap(void *p, size_t s)
{
	int           n;
	unsigned char ptmp, *pc;

	pc = (unsigned char *)p;

	for (n = 0; n < s / 2; n++) {
		ptmp          = pc[n];
		pc[n]         = pc[s - n - 1];
		pc[s - n - 1] = ptmp;
	}

	return;
}
