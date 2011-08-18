// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef CUBEPM_ADT_H
#define CUBEPM_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file cubepm_adt.h
 * @ingroup libutilFilesCubePM
 * @brief  This file provides the main structure of the CubePM object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "endian.h"
#include "cubepm.h"


/*--- ADT implementation ------------------------------------------------*/

/**
 * @brief  The main structure for the CubePM file object.
 */
struct cubepm_struct {
	/**
	 * @name  Handling Information
	 *
	 * @{
	 */
	/** @brief  The number of files in this set. */
	int          numFiles;
	/** @brief  The file names for each file. */
	char         **fileNames;
	/** @brief  Gives the endianess of the file. */
	endian_t     fileEndianess;
	/** @brief  The file pointer to the currently opened file in the set. */
	FILE         *f;
	/** @brief  Keeps track of the current mode of the file. */
	cubepmMode_t mode;
	/** @brief  Keeps track of the last file that was opened. */
	int          lastOpened;
	/** @brief  Stores the offset of the coordinates for the current file. */
	double       fileCoordOffset[3];
	/** @} */

	/**
	 * @name  Header Values Stored in the Files
	 *
	 * @{
	 */
	/** @brief  The number of particles in each file.  */
	int32_t *np_local;
	/** @brief  The current expansion factor.  */
	float   a;
	/** @brief  The current time?  */
	float   t;
	/** @brief  Unknown.  */
	float   tau;
	/** @brief  Unknown.  */
	int32_t nts;
	/** @brief  Unknown.  */
	float   dt_f_acc;
	/** @brief  Unknown.  */
	float   dt_pp_acc;
	/** @brief  Unknown.  */
	float   dt_c_acc;
	/** @brief  Unknown.  */
	int32_t cur_checkpoint;
	/** @brief  Unknown.  */
	int32_t cur_projection;
	/** @brief  Unknown.  */
	int32_t cur_halofind;
	/** @brief  The number of cells that constitute one particle.  */
	float   mass_p;
	/** @} */

	/**
	 * @name  Extra Header Information
	 *
	 * @{
	 */
	/** @brief  Gives Omega Matter. */
	double omega0;
	/** @brief  Gives Omega Lambda. */
	double lambda0;
	/** @brief  Gives the boxsize in Mpc/h. */
	double boxsize;
	/** @brief  Gives the number of nodes used to write the files. */
	int    nodesDim;
	/** @brief  The size of the top level grid. */
	int    ngrid;
	/** @} */
};


#endif
