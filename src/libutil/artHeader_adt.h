// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef ARTHEADER_ADT_H
#define ARTHEADER_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/artHeader_adt.h
 * @ingroup libutilFilesART
 * @brief  This file provides the main structure implementation for the
 *         ART header object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include "endian.h"


/*--- Local defines -----------------------------------------------------*/

/** @brief  Gives the length of the header string. */
#define ARTHEADER_HEADERSTRING_LENGTH 45

/**
 * @brief  Gives at which postition in the extra array the boxsize is
 *         stored.
 */
#define ARTHEADER_EXTRA_POS_BOXSIZE 99


/*--- ADT implementation ------------------------------------------------*/

/**
 * @brief  The main structure for the ART header file object.
 */
struct artHeader_struct {
	/**
	 * @name  Values stored in the file.
	 * @{
	 */

	/**
	 * @brief  Stores a descriptive header string.
	 *
	 * The string in the file is only #ARTHEADER_HEADERSTRING_LENGTH
	 * characters long, but we need one more for the terminating '\0' to
	 * make it a proper C string.
	 */
	char  headerString[ARTHEADER_HEADERSTRING_LENGTH + 1];
	/** @brief  The current expansion factor.  */
	float aexpn;
	/** @brief  The initial expansion factor. */
	float aexp0;
	/** @brief  An amplitude. */
	float amplt;
	/** @brief  The current expansion time step. */ 
	float astep;
	/** @brief  The current time step number. */ 
	int   istep;
	/** @brief  Unknown.  @todo Figure out what this is. */ 
	float partw;
	/** @brief  Unknown.  @todo Figure out what this is. */ 
	float tintg;
	/** @brief  Unknown.  @todo Figure out what this is. */ 
	float ekin;
	/** @brief  Unknown.  @todo Figure out what this is. */ 
	float ekin1;
	/** @brief  Unknown.  @todo Figure out what this is. */ 
	float ekin2;
	/** @brief  Unknown.  @todo Figure out what this is. */ 
	float au0;
	/** @brief  Unknown.  @todo Figure out what this is. */ 
	float aeu0;
	/** @brief  The dimension of the finest employable grind. */ 
	int   nrowc;
	/** @brief  The dimension of the coarsest grid. */ 
	int   ngridc;
	/** @brief  The number of particle species in the file. */ 
	int   nspecies;
	/** @brief  The initial condition random seed. */ 
	int   nseed;
	/** @brief  This is Omega_Matter_0. */ 
	float Om0;
	/** @brief  This is Omega_Lambda_0. */ 
	float Oml0;
	/** @brief  The Hubble parameter in units of 100 km/s/Mpc. */ 
	float hubble;
	/** @brief  Unknown.  @todo Figure out what this is. */ 
	float wp5;
	/** @brief  This is Omega_curv_0. */ 
	float Ocurv;
	/** @brief  An array holding a bunch of unnamed extra values. */ 
	float extras[100];

	/**
	 * @brief  The weight for each particle species.
     *
	 * Will be set to &(extras[0]), i.e. the first 10 value of extra
	 * contain the particle weights.
	 */
	float *wspecies;

	/**
	 * @brief  The number of particle of the different species.
	 *
	 * Will be set to &(extras[10]), i.e. 20 values of extra, starting
	 * at the eleventh entry will contain the number of particles.
	 */
	int64_t *lspecies;

	/** @} */

	/**
	 * @name  Handling information
	 * @{
	 */
	/** @brief  The endianess of the file. */
	endian_t fileEndianess;

	/**
	 *  @brief  The conversion factor from internal particle weights to M_sun/h.
	 */
	double factorFileWeightToMsunh;

	/**
	 * @brief  The conversion factor from internal particle positions to
	 *         Mpc/h.
	 */
	double factorFilePositionToMpch;

	/**
	 * @brief  The conversion factor from internal particle velocities
	 *         to km/s.
	 */
	double factorFileVelocityToKms;

	/** @} */
};


#endif
