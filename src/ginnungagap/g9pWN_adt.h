// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PWN_ADT_H
#define G9PWN_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pWN_adt.h
 * @ingroup  ginnungagapWN
 * @brief  Provides the implementation of the WN structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdbool.h>
#include "../libgrid/gridReader.h"
#include "../libgrid/gridWriter.h"
#include "../libutil/rng.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief Main structure. */
struct g9pWN_struct {
	/** @brief  Flags whether the WN is read from a file.  */
	bool         useFile;
	/** @brief  Gives, if appropriate, the WN reader.  */
	gridReader_t reader;
	/** @brief  The RNG to use. */
	rng_t        rng;
	/** @brief  Flags whether the WN should be written to file. */
	bool         dumpWhiteNoise;
	/** @brief  Provides the reader, if appropriate. */
	gridWriter_t writer;
};


#endif
