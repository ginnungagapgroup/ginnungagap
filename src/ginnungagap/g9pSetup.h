// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PSETUP_H
#define G9PSETUP_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pSetup.h
 * @ingroup  ginnungagapSetup
 * @brief  Provides the interface to the setup routine.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pNorm.h"
#include <stdint.h>
#include <stdbool.h>
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Defines the handle to the setup structure. */
typedef struct g9pSetup_struct *g9pSetup_t;


/*--- Implementation of main structure ----------------------------------*/

/** @brief  The setup structure */
struct g9pSetup_struct {
	/** @brief  The size of the target grid. */
	uint32_t       dim1D;
	/** @brief  Size of the simulation box in Mpc/h. */
	double         boxsizeInMpch;
	/** @brief  Initial redshift. */
	double         zInit;
	/** @brief  The name of the Grid (only relevant for some file types). */
	char           *gridName;
	/** @brief  Gives the employed normalisation mode. */
	g9pNorm_mode_t normalisationMode;
#ifdef WITH_MPI
	/** @brief  The process grid. */
	int nProcs[NDIM];
#endif
};


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creates a setup structure from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file to parse.
 *
 * @return  Returns a new setup structure.
 */
extern g9pSetup_t
g9pSetup_new(parse_ini_t ini);


/**
 * @brief  Deletes a setup structure and frees the associated memory.
 *
 * @param[in,out]  *setup
 *                    Pointer to the external variable holding the
 *                    setup.  Will be set to @c NULL after completion of
 *                    the function.
 *
 * @return  Returns nothing.
 */
extern void
g9pSetup_del(g9pSetup_t *setup);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagapSetup  Setup
 * @ingroup ginnungagap
 * @brief Provides the setup of ginnungagap (from an ini file).
 */


#endif
