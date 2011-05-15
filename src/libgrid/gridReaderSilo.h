// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERSILO_H
#define GRIDREADERSILO_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderSilo.h
 * @ingroup libgridIOIn
 * @brief  This file provides the SILO file reader definitions.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a reader object.
 */
typedef struct gridReaderSilo_struct *gridReaderSilo_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  This implements gridReader_newFromIni() for Silo files.
 *
 * This creates a new reader object for Silo files from the information
 * found in an ini file.  The ini section needs to contain the following
 * information: BLABLABLA
 *
 * @param[in,out]  ini
 *                    The ini file to read from.
 * @param[in]      *sectionName
 *                    The section within the ini file to parse for
 *                    information.
 *
 * @return  Returns a new reader for Silo files initialized from the
 *          information in the ini file.
 */
extern gridReaderSilo_t
gridReaderSilo_newFromIni(parse_ini_t ini, const char *sectionName);


/**
 * @brief  This implements gridReader_del() for Silo files.
 *
 * @param[in,out]  *reader
 *                    The reader to delete.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderSilo_del(gridReader_t *reader);


/** @} */

/**
 * @name  Using
 *
 * @{
 */

/**
 * @brief  Implements  gridReader_readIntoPatch() for Silo files.
 *
 * @param[in,out]  reader
 *                    The reader to use.
 * @param[in,out]  patch
 *                    The patch to read into.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderSilo_readIntoPatch(gridReader_t reader, gridPatch_t patch);


/**
 * @brief  Implements gridReader_readIntoPatchForVar() for Silo files.
 *
 * @param[in,out]  reader
 *                    The reader to use.
 * @param[in,out]  patch
 *                    The patch to read into.
 * @param[in]      idxOfVar
 *                    The variable of the patch that should be read.
 *
 * @return
 */
extern void
gridReaderSilo_readIntoPatchForVar(gridReader_t reader,
                                   gridPatch_t  patch,
                                   int          idxOfVar);


/** @} */


#endif
