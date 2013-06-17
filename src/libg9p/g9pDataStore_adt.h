// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PDATASTORE_ADT_H
#define G9PDATASTORE_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pDataStore_adt.h
 * @ingroup  libg9pDatastore
 * @brief  Provides the main structure of the data store.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchy.h"
#include "../libutil/refCounter.h"
#include "../libutil/filename.h"


/*--- ADT implementation ------------------------------------------------*/

typedef filename_t filenameTuple_t[NDIM + 1];

struct g9pDataStore_struct {
	refCounter_t   refCounter;
	g9pHierarchy_t hierarchy;
	char           *name;
	char           *path;
};


#endif
