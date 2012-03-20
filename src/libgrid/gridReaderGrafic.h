// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERGRAFIC_H
#define GRIDREADERGRAFIC_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderGrafic.h
 * @ingroup  libgridIOInGrafic
 * @brief  Provides the interface for the Grafic reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include "../libutil/grafic.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the Grafic reader. */
typedef struct gridReaderGrafic_struct *gridReaderGrafic_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting (Virtual)
 *
 * @{
 */

/** @copydoc gridReader_del() */
extern void
gridReaderGrafic_del(gridReader_t *reader);

/** @} */

/**
 * @name  Using (Virtual)
 *
 * @{
 */

/** @copydoc gridReader_readIntoPatch() */
extern void
gridReaderGrafic_readIntoPatch(gridReader_t reader, gridPatch_t patch);

/** @copydoc gridReader_readIntoPatchForVar() */
extern void
gridReaderGrafic_readIntoPatchForVar(gridReader_t reader,
                                     gridPatch_t  patch,
                                     int          idxOfVar);

/** @} */


/*--- Prototypes of final functions -------------------------------------*/

/**
 * @name  Creating and Deleting (Final)
 *
 * @{
 */

/**
 * @brief  Creates a new empty Grafic reader.
 *
 * @return  The new writer.
 */
extern gridReaderGrafic_t
gridReaderGrafic_new(void);

/** @} */

/**
 * @name  Getter (Final)
 *
 * @{
 */

/**
 * @brief  Retrieves a the underlying Grafic object from a Grafic grid
 *         reader.
 *
 * @param[in]  reader
 *                The reader that should be queried, passing @c NULL is
 *                undefined.
 *
 * @return  Returns a handle to the internal Grafic object, the caller must
 *          not try free the object.
 */
extern grafic_t
gridReaderGrafic_getGrafic(const gridReaderGrafic_t writer);

/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInGrafic Grafic Reader
 * @ingroup libgridIOIn
 * @brief  Provides the Grafic reader.
 */

#endif
