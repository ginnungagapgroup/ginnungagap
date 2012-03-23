// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MAKEMASKSETUP_H
#define MAKEMASKSETUP_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file makeMask/makeMaskSetup.h
 * @ingroup  toolsMakeMaskSetup
 * @brief  Deals with parsing information from the ini file.
 */


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "../../src/libutil/parse_ini.h"
#include "../../src/liblare/lare.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Defines the handle for the setup structure. */
typedef struct makeMaskSetup_struct *makeMaskSetup_t;


/*--- Implemention of main structure ------------------------------------*/

/**
 * @brief  The actual setup structure.
 */
struct makeMaskSetup_struct {
	/** @brief  The size of grid for which to generate the mask. */
	uint32_t baseGridSize1D;
	/** @brief  The number of refinement levels to generate. */
	uint32_t numLevels;
	/** @brief  The refinement factor between the levels. */
	uint32_t refinementFactor;
	/** @brief  The base level of the mask. */
	uint32_t baseRefinementLevel;
	/** @brief  Gives the section name from which to construct the writer. */
	char     *outSecName;
	/** @brief  The Lagrangian region object holding the hi-res cells. */
	lare_t   lare;
#ifdef WITH_MPI
	/** @brief  Stores the MPI process grid to parallelize the mask. */
	int      nProcs[NDIM];
#endif
};


/*--- Prototypes of exported functions ----------------------------------*/
extern makeMaskSetup_t
makeMaskSetup_newFromIni(parse_ini_t ini, const char *maskSectionName);

extern void
makeMaskSetup_del(makeMaskSetup_t *setup);

extern uint32_t
makeMaskSetup_getBaseGridSize1D(const makeMaskSetup_t setup);

extern char *
makeMaskSetup_getOutSecName(const makeMaskSetup_t setup);

#endif
