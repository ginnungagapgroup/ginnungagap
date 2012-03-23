// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETHEADER_ADT_H
#define GADGETHEADER_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetHeader_adt.h
 * @ingroup libutilFilesGadgetHeader
 * @brief  This file provides the implementation of the header structure
 *         of Gadget files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include <stdbool.h>


/*--- ADT implementation ------------------------------------------------*/

/** @brief  Main structure for the Gadget Header object. */
struct gadgetHeader_struct {
	/** The number of particles of each type in the file. */
	uint32_t np[6];
	/** The masses for the different particle types. */
	double   massarr[6];
	/** The time of the snapshot (expansion factor for cosmo runs). */
	double   time;
	/** The redshift of the snapshot. */
	double   redshift;
	/** Flags whether the simulation included star formation. */
	int32_t  flagsfr;
	/** Flags whether feedback was enables. */
	int32_t  flagfeedback;
	/** The total number of particles of each type in the snapshot. */
	uint32_t nall[6];
	/** Flags whether cooling was enabled. */
	int32_t  flagcooling;
	/** The number of files used for storing the snapshot. */
	int32_t  numfiles;
	/** The boxsize of the simulation. */
	double   boxsize;
	/** The matter density in units of the critical density. */
	double   omega0;
	/** The cosmological constant density.  */
	double   omegalambda;
	/** The Hubble parameter in units of 100 km/s/Mpc. */
	double   hubbleparameter;
	/** Flags whether the file contains formation times of stars. */
	int32_t  flagstellarage;
	/** Flags whether the file contains metallicities for gas and stars. */
	int32_t  flagmetal;
	/** Stores the missing leading 32bits of the total number of particles. */
	uint32_t nallhighw[6];
	/** Flags whether the (IC-)file contains entropy instead of u. */
	int32_t  flagentropy;
	/** Flags whether the snapshots is in double precision. */
	int32_t  flagdoubleprecision;

	/**
	 * Flags which methods was used to generate the IC (see Gadget3
	 * sources for more information on what this means).
	 */
	int32_t flagicinfo;
	/** Gives the scaling factor for 2lpt ICs. */
	float   lptscalingfactor;

	/** Flags whether the IDs are 32 or 64bit. */
	bool useLongIDs;
};

#endif
