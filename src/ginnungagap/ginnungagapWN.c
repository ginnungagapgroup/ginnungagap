// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagapWN.h"
#include <assert.h>
#include "../libutil/parse_ini.h"
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "ginnungagapWN_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagapWN_t
ginnungagapWN_newFromIni(parse_ini_t ini, const char *sectionName)
{
	ginnungagapWN_t wn;

	assert(ini != NULL);
	assert(sectionName != NULL);

	wn = xmalloc(sizeof(struct ginnungagapWN_struct));
	getFromIni(&(wn->useFile), parse_ini_get_bool,
	           ini, "useFile", sectionName);

	wn->whiteNoiseFileName = NULL;
#ifdef WITH_SPRNG
	wn->rng = NULL;
#endif

	if (wn->useFile) {
		getFromIni(&(wn->whiteNoiseFileName), parse_ini_get_string,
		           ini, "whiteNoiseFileName", sectionName);
	} else {
#ifndef WITH_SPRNG
		fprintf(stderr, "No file and no RNG.. hence no white noise :(\n");
		diediedie(EXIT_FAILURE);
#else
		wn->rng = rng_newFromIni(ini, "rng");
#endif
	}

	return wn;
}

extern void
ginnungagapWN_del(ginnungagapWN_t *wn)
{
	assert(wn != NULL && *wn != NULL);

	if ((*wn)->whiteNoiseFileName != NULL)
		xfree((*wn)->whiteNoiseFileName);
#ifdef WITH_SPRNG
	if ((*wn)->rng != NULL)
		rng_del(&((*wn)->rng));
#endif
	xfree(*wn);

	*wn = NULL;
}

extern void
ginnungagapWN_setup(ginnungagapWN_t wn)
{
	;
}

/*--- Implementations of local functions --------------------------------*/
