// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#include "startup.h"
#include "../../config.h"
#include "globalVariables.h"
#include "../libutil/xmem.h"
#include <stdlib.h>
#include <stdio.h>


static void
finalReport(void);


extern void
startup(void)
{
	atexit(&finalReport);
}


static void
finalReport(void)
{
#ifdef XMEM_TRACK_MEM
	printf("\n");
	xmem_info(stdout);
	printf("\n");
#endif
	printf("Vertu sæl/sæll...\n");
}
