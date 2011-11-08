// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pSetup.c
 * @ingroup  ginnungagapSetup
 * @brief  Provides the implemenation of the setup structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pSetup.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/diediedie.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  Default name for the WN P(k). */
static const char *local_namePkWN = "Pk.wn.dat";

/** @brief  Default name for the delta P(k). */
static const char *local_namePkDeltak = "Pk.deltak.dat";

/** @brief  Default name for the input P(k). */
static const char *local_namePkInput = "Pk.input.dat";

/** @brief  Default name for the input P(k) at z_init. */
static const char *local_namePkInputZinit = "Pk.input_zinit.dat";

/** @brief  Default name for the input P(k) at z_0. */
static const char *local_namePkInputZ0 = "Pk.input_z0.dat";

/** @brief  Default name for the density histogram. */
static const char *local_nameHistoDens = "histogram.dens.dat";

/** @brief  Default name for the x-velocity histogram. */
static const char *local_nameHistoVelx = "histogram.velx.dat";

/** @brief  Default name for the y-velocity histogram. */
static const char *local_nameHistoVely = "histogram.vely.dat";

/** @brief  Default name for the z-velocity histogram. */
static const char *local_nameHistoVelz = "histogram.velz.dat";


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Parses the required parameters.
 *
 * @param[in,out]  s
 *                    The setup structure to be filled.
 * @param[in,out]  ini
 *                    The ini file to use.
 *
 * @return Returns nothing.
 */
static void
local_parseRequired(g9pSetup_t s, parse_ini_t ini);


/**
 * @brief  Parses the optinal parameters.
 *
 * @param[in,out]  s
 *                    The setup structure to be filled.
 * @param[in,out]  ini
 *                    The ini file to use.
 *
 * @return Returns nothing.
 */
static void
local_parseOptional(g9pSetup_t s, parse_ini_t ini);


/**
 * @brief  Retrieves the normalisation mode from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file to read from.
 *
 * @return  Returns the mode.
 */
static g9pNorm_mode_t
local_getNormModeFromIni(parse_ini_t ini);


#ifdef WITH_MPI

/**
 * @brief  Helper function to parse the MPI setup.
 *
 * @param[in,out]  setup
 *                    The setup structure to fill.
 * @patam[in,out]  ini
 *                    The ini file to use.
 *
 * @return  Returns nothing.
 */
static void
local_parseMPIStuff(g9pSetup_t setup, parse_ini_t ini);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern g9pSetup_t
g9pSetup_new(parse_ini_t ini)
{
	g9pSetup_t setup;

	assert(ini != NULL);

	setup = xmalloc(sizeof(struct g9pSetup_struct));
	local_parseRequired(setup, ini);
#ifdef WITH_MPI
	local_parseMPIStuff(setup, ini);
#endif
	local_parseOptional(setup, ini);

	return setup;
}

extern void
g9pSetup_del(g9pSetup_t *setup)
{
	assert(setup != NULL && *setup != NULL);

	xfree((*setup)->nameHistogramVelz);
	xfree((*setup)->nameHistogramVely);
	xfree((*setup)->nameHistogramVelx);
	xfree((*setup)->nameHistogramDens);
	xfree((*setup)->namePkWN);
	xfree((*setup)->namePkDeltak);
	xfree((*setup)->namePkInput);
	xfree((*setup)->namePkInputZinit);
	xfree((*setup)->namePkInputZ0);
	xfree((*setup)->gridName);

	xfree(*setup);
	*setup = NULL;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_parseRequired(g9pSetup_t s, parse_ini_t ini)
{
	getFromIni(&(s->dim1D), parse_ini_get_uint32,
	           ini, "dim1D", "Ginnungagap");
	getFromIni(&(s->boxsizeInMpch), parse_ini_get_double,
	           ini, "boxsizeInMpch", "Ginnungagap");
	getFromIni(&(s->zInit), parse_ini_get_double,
	           ini, "zInit", "Ginnungagap");
	getFromIni(&(s->gridName), parse_ini_get_string,
	           ini, "gridName", "Ginnungagap");
	s->normalisationMode = local_getNormModeFromIni(ini);
}

static void
local_parseOptional(g9pSetup_t s, parse_ini_t ini)
{
	if (!(parse_ini_get_bool(ini, "do2LPTCorrections", "Ginnungagap",
	                         &(s->do2LPTCorrections))))
		s->do2LPTCorrections = false;

	if (!(parse_ini_get_bool(ini, "writeDensityField", "Ginnungagap",
	                         &(s->writeDensityField))))
		s->writeDensityField = true;

	if (!(parse_ini_get_string(ini, "namePkWN", "Ginnungagap",
	                           &(s->namePkWN))))
		s->namePkWN = xstrdup(local_namePkWN);
	if (!(parse_ini_get_string(ini, "namePkDeltak", "Ginnungagap",
	                           &(s->namePkDeltak))))
		s->namePkDeltak = xstrdup(local_namePkDeltak);
	if (!(parse_ini_get_string(ini, "namePkInput", "Ginnungagap",
	                           &(s->namePkInput))))
		s->namePkInput = xstrdup(local_namePkInput);
	if (!(parse_ini_get_string(ini, "namePkInputZinit", "Ginnungagap",
	                           &(s->namePkInputZinit))))
		s->namePkInputZinit = xstrdup(local_namePkInputZinit);
	if (!(parse_ini_get_string(ini, "namePkInputZ0", "Ginnungagap",
	                           &(s->namePkInputZ0))))
		s->namePkInputZ0 = xstrdup(local_namePkInputZ0);

	if (!(parse_ini_get_bool(ini, "doHistograms", "Ginnungagap",
	                         &(s->doHistograms))))
		s->doHistograms = false;
	if (s->doHistograms) {
		getFromIni(&(s->histogramNumBins), parse_ini_get_uint32,
		           ini, "histogramNumBins", "Ginnungagap");
		getFromIni(&(s->histogramExtremeDens), parse_ini_get_double,
		           ini, "histogramExtremeDens", "Ginnungagap");
		getFromIni(&(s->histogramExtremeVel), parse_ini_get_double,
		           ini, "histogramExtremeVel", "Ginnungagap");
	}
	if (!(parse_ini_get_string(ini, "nameHistogramDens", "Ginnungagap",
	                           &(s->nameHistogramDens))))
		s->nameHistogramDens = xstrdup(local_nameHistoDens);
	if (!(parse_ini_get_string(ini, "nameHistogramVelx", "Ginnungagap",
	                           &(s->nameHistogramVelx))))
		s->nameHistogramVelx = xstrdup(local_nameHistoVelx);
	if (!(parse_ini_get_string(ini, "nameHistogramVely", "Ginnungagap",
	                           &(s->nameHistogramVely))))
		s->nameHistogramVely = xstrdup(local_nameHistoVely);
	if (!(parse_ini_get_string(ini, "nameHistogramVelz", "Ginnungagap",
	                           &(s->nameHistogramVelz))))
		s->nameHistogramVelz = xstrdup(local_nameHistoVelz);
} /* local_parseOptional */

static g9pNorm_mode_t
local_getNormModeFromIni(parse_ini_t ini)
{
	char           *name;
	g9pNorm_mode_t mode;

	getFromIni(&name, parse_ini_get_string,
	           ini, "normalisationMode", "Ginnungagap");
	mode = g9pNorm_getModeFromName(name);
	if (mode == G9PNORM_MODE_UNKNOWN) {
		fprintf(stderr, "Normalisation mode %s unknown\n", name);
		diediedie(EXIT_FAILURE);
	}

	xfree(name);

	return mode;
}

#ifdef WITH_MPI
static void
local_parseMPIStuff(g9pSetup_t setup, parse_ini_t ini)
{
	int32_t *nProcs;
	bool    rtn;

	rtn = parse_ini_get_int32list(ini, "nProcs", "MPI", NDIM, &nProcs);
	if (!rtn) {
		fprintf(stderr, "Could not get nProcs from section MPI.\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < NDIM; i++)
		setup->nProcs[i] = (int)(nProcs[i]);
	xfree(nProcs);
}

#endif
