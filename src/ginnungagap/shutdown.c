// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#include "shutdown.h"
#include "../../config.h"
#include "globalVariables.h"
#include "../libutil/xmem.h"
#include <stdlib.h>
#include <stdio.h>


static void
freeGlobalVariables(void);


extern void
shutdown(void)
{
	freeGlobalVariables();
}


static void
freeGlobalVariables(void)
{
	if (globalIniFname != NULL)
		xfree(globalIniFname);
}
