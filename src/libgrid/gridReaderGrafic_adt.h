// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERGRAFIC_ADT_H
#define GRIDREADERGRAFIC_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderGrafic_adt.h
 * @ingroup  libgridIOInGrafic
 * @brief  Provides the main structure of the Grafic reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader_adt.h"
#include "../libutil/grafic.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure. */
struct gridReaderGrafic_struct {
	/** @brief  The base structure. */
	struct gridReader_struct base;
	/** @brief  The low level Grafic interface. */
	grafic_t grafic;
};


/*--- Prototypes of protected functions ---------------------------------*/

/**
 * @name  Creating and Deleting (Protected)
 *
 * @{
 */

/**
 * @brief  Allocates memory for a Grafic grid reader.
 *
 * @return  Returns a handle to a new (uninitialized) Grafic reader structure.
 */
extern gridReaderGrafic_t
gridReaderGrafic_alloc();


/**
 * @brief  Sets all required fields of the Grafic reader structure to safe
 *         initial values.
 *
 * @param[in,out]  reader
 *                    The reader to initialize.  This must be a valid reader
 *                    object.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderGrafic_init(gridReaderGrafic_t reader);


/**
 * @brief  Frees all members of the Reader writer structure.
 *
 * @param[in,out]  reader
 *                    The reader to work with,  This must be a valid reader,
 *                    passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderGrafic_free(gridReaderGrafic_t reader);

/** @} */

/**
 * @name  Setter (Protected)
 *
 * @{
 */

/**
 * @brief  Sets the underlying Grafic object to the provided one.
 *
 * @param[in,out]  reader
 *                    The reader to set the Grafic object for.  Must be a
 *                    proper Grafic grid reader object, passing @c NULL is
 *                    undefined.
 * @param[in]      grafic
 *                    The graphic module that should be set.  The caller
 *                    relinquishes control over the object.  Passing @c NULL
 *                    is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderGrafic_setGrafic(gridReaderGrafic_t reader, grafic_t grafic);

/** @} */


#endif
