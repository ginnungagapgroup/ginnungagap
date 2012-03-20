// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERBOV_H
#define GRIDREADERBOV_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderBov.h
 * @ingroup  libgridIOInBov
 * @brief  Provides the interface for the BOV reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include "../libutil/parse_ini.h"
#include "../libutil/bov.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the BOV reader */
typedef struct gridReaderBov_struct *gridReaderBov_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting (Virtual)
 *
 * @{
 */

/** @copydoc gridReader_del() */
extern void
gridReaderBov_del(gridReader_t *reader);


/** @} */

/**
 * @name  Using (Virtual)
 *
 * @{
 */

/** @copydoc gridReader_readIntoPatch() */
extern void
gridReaderBov_readIntoPatch(gridReader_t reader, gridPatch_t patch);


/** @copydoc gridReader_readIntoPatchForVar() */
extern void
gridReaderBov_readIntoPatchForVar(gridReader_t reader,
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
 * @brief  Creates a new empty Bov reader.
 *
 * @return  The new writer.
 */

extern gridReaderBov_t
gridReaderBov_new(void);


/** @} */

/**
 * @name  Getter (Final)
 *
 * @{
 */

/**
 * @brief  Retrieves a the underlying BOV object from a BOV grid
 *         reader.
 *
 * @param[in]  reader
 *                The reader that should be queried, passing @c NULL is
 *                undefined.
 *
 * @return  Returns a handle to the internal BOV object, the caller must
 *          not try free the object.
 */

extern bov_t
gridReaderBov_getBov(const gridReaderBov_t reader);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInBov BOV Reader
 * @ingroup libgridIOIn
 * @brief  Provides the BOV reader.
 */


#endif
