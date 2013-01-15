// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PDATASTORE_H
#define G9PDATASTORE_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pDataStore.h
 * @ingroup  libg9pDatastore
 * @brief  Provides the interface to the data store.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pFieldID.h"
#include "g9pHierarchy.h"
#include "../libutil/filename.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle to the data store */
typedef struct g9pDataStore_struct *g9pDataStore_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern g9pDataStore_t
g9pDataStore_new(g9pHierarchy_t h, const char *name, const char *path);

extern g9pDataStore_t
g9pDataStore_ref(g9pDataStore_t ds);

extern void
g9pDataStore_del(g9pDataStore_t *ds);


extern char *
g9pDataStore_getDir(const g9pDataStore_t ds, uint8_t level);

extern filename_t
g9pDataStore_getFileName(const g9pDataStore_t ds,
                         uint8_t              level,
                         g9pFieldID_t         fid);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pDatastore Data Store
 * @ingroup libg9p
 * @brief Provides a data store that allows accessing the various relevant
 *        fields.
 */

#endif
