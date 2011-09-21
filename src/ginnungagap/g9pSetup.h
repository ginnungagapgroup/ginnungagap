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
	/** @brief  Flags whether the density field should be written. */
	bool writeDensityField; ///< Defaults to @c true.
	/** @brief  Gives the name of the P(k) of the white noise. */
	char *namePkWN; ///< Defaults to #local_namePkWn.
	/** @brief  Gives the name of the P(k) of the overdensity field. */
	char *namePkDeltak; ///< Defaults to #local_namePkDeltak.
	/** @brief  Gives the name of the P(k) of the input P(k). */
	char *namePkInput; ///< Defaults to #local_namePkInput.
	/** @brief  Gives the name of the P(k) of the input P(k) at z_init. */
	char *namePkInputZinit; ///< Defaults to #local_namePkInputZinit.
	/** @brief  Gives the name of the P(k) of the input P(k) at z=0. */
	char *namePkInputZ0; ///< Defaults to #local_namePkInputZ0.
	/** @brief  Flags whether histograms should be calculated. */
	bool doHistograms; ///< Defaults to @c true.
	/** @brief  The name for the density histogram. */
	char *nameHistogramDens; ///< Defaults to #local_nameHistoDens;
	/** @brief  The name for the x-velocity histogram. */
	char *nameHistogramVelx; ///< Defaults to #local_nameHistoVelx.
	/** @brief  The name for the y-velocity histogram. */
	char *nameHistogramVely; ///< Defaults to #local_nameHistoVely.
	/** @brief  The name for the z-velocity histogram. */
	char *nameHistogramVelz; ///< Defaults to #local_nameHistoVelz.
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
