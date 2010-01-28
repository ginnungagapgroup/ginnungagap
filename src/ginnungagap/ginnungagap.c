// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "ginnungagap.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagap_struct {
#ifdef WITH_MPI
	struct {
		int rank;
		int size;
	} mpi;
#endif
	int empty;
};


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagap_t
ginnungagap_new(const char *iniFname, int flags)
{
	ginnungagap_t ginnungagap;

	assert(iniFname != NULL);
	assert(flags == 0);

	ginnungagap = xmalloc(sizeof(struct ginnungagap_struct));
	ginnungagap->empty = 0;

	return ginnungagap;
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	assert(ginnungagap != NULL);

	printf("%i\n", ginnungagap->empty);
}

extern void
ginnungagap_del(ginnungagap_t *ginnungagap)
{
	assert(ginnungagap != NULL);
	assert(*ginnungagap != NULL);

	xfree(*ginnungagap);
	*ginnungagap = NULL;
}


/*--- Implementations of local functions --------------------------------*/
