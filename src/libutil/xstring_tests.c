// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "xstring_tests.h"
#include "xstring.h"
#include <stdio.h>
#ifdef XMEM_TRACK_MEM
#  include "xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
xstring_xdirname_test(void)
{
	bool     hasPassed = true;
	char     *tmp;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	tmp = xdirname("/usr/lib");
	if (strcmp(tmp, "/usr") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xdirname("/usr/");
	if (strcmp(tmp, "/") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xdirname("usr");
	if (strcmp(tmp, ".") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xdirname("/");
	if (strcmp(tmp, "/") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xdirname(".");
	if (strcmp(tmp, ".") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xdirname("..");
	if (strcmp(tmp, ".") != 0)
		hasPassed = false;
	xfree(tmp);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
xstring_xbasename_test(void)
{
	bool     hasPassed = true;
	char     *tmp;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	tmp = xbasename("/usr/lib");
	if (strcmp(tmp, "lib") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xbasename("/usr/");
	if (strcmp(tmp, "usr") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xbasename("usr");
	if (strcmp(tmp, "usr") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xbasename("/");
	if (strcmp(tmp, "/") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xbasename(".");
	if (strcmp(tmp, ".") != 0)
		hasPassed = false;
	xfree(tmp);

	tmp = xbasename("..");
	if (strcmp(tmp, "..") != 0)
		hasPassed = false;
	xfree(tmp);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
