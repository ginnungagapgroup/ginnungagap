// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERBOV_ADT_H
#define GRIDREADERBOV_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderBov_adt.h
 * @ingroup  libgridIOInBov
 * @brief  Provides the main structure of the BOV reader.
 */

/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader_adt.h"
#include "../libutil/bov.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure for the BOV reader. */
struct gridReaderBov_struct {
	/** @brief  The base structure. */
	struct gridReader_struct base;
	/** @brief  The low level BOV interface. */
	bov_t bov;
};


/*--- Prototypes of protected functions ---------------------------------*/

/**
 * @name  Creating and Deleting (Protected)
 *
 * @{
 */

/**
 * @brief  Allocates memory for a BOV grid reader.
 *
 * @return  Returns a handle to a new (uninitialized) BOV reader structure.
 */
extern gridReaderBov_t
gridReaderBov_alloc();


/**
 * @brief  Sets all required fields of the BOV reader structure to safe
 *         initial values.
 *
 * @param[in,out]  reader
 *                    The reader to initialize.  This must be a valid reader
 *                    object.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderBov_init(gridReaderBov_t reader);


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
gridReaderBov_free(gridReaderBov_t reader);

/** @} */

/**
 * @name  Setter (Protected)
 *
 * @{
 */

/**
 * @brief  Sets the underlying bov object.
 *
 * @param[in,out]  reader
 *                    The reader object for which to set the bov.  If a bov
 *                    object is already attached, it will be freed first.
 * @param[in]      bov
 *                    The new bov object that should be used.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderBov_setBov(gridReaderBov_t reader, bov_t bov);

/** @} */

#endif
