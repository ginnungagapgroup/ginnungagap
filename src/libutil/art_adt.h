// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef ART_ADT_H
#define ART_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/art_adt.h
 * @ingroup libutilFilesART
 * @brief  This file provides the main structure implementation for the
 *         ART file object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "artHeader.h"
#include <stdbool.h>


/*--- ADT implementation ------------------------------------------------*/

/**
 * @brief  The main structure for the ART file object.
 */
struct art_struct {
	/** @brief  Stores the filename of the PMcrd.DAT header file. */
	char *fileNameHeader;
	/** @brief  Stores the filenames of the PMcrsX.DAT data files. */
	char **fileNamesData;
	/** @brief  Stores the number of PMcrsX.DAT files in this set. */
	int  numFiles;

	/**
	 * @brief  Toggles whether the nrowc is truncated to a maximum size
	 *         when calculating the page size.
	 */
	bool        truncateNrowc;
	/** @brief  Stores the file pointer. */
	FILE        *f;
	/** @brief  Keeps track of the current mode of the file. */
	artMode_t   mode;
	/** @brief  This keeps track of the last file that was opened. */
	int         lastOpened;
	/** @brief  Keeps a reference to the content of the header. */
	artHeader_t header;
	/** @brief  The total number of pages. */
	int         numTotalPages;
	/** @brief  The number of pages in each file (except last). */
	int         numPagesInFile;
	/** @brief  The number of pages in the last file. */
	int         numPagesInLastFile;
	/** @brief  The number of particles per page. */
	int         numParticlesInPage;
	/** @brief  The number of particles used in the last page. */
	int         numParticlesInLastPage;
	/** @brief  The number of particles in the files. */
	int         numParticlesInFile;
	/** @brief  The number of particles in the last file. */
	int         numParticlesInLastFile;
	/** @brief  Stores the number of pages in the currently opened file. */
	int         numPagesInThisFile;
	/** @brief  Stores the number of particles in the currently opened file. */
	int         numParticlesInThisFile;
};


#endif
