// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADER_H
#define GRIDREADER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReader.h
 * @ingroup libgridIOIn
 * @brief  This file provides the interface to an abstract grid reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a reader object.
 */
typedef struct gridReader_struct *gridReader_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new reader object from an ini file.
 *
 * This will look in a specified section in the provided ini file for
 * construction information for the reader.  The information absolutely
 * required are two fields: @a readerType and @a readerSection.  The
 * first one must be a recognized type defined in gridIO.h and the
 * second a valid string that gives the name of the section in which the
 * detailed construction information for the specific type can be found.
 *
 * @param[in,out]  ini
 *                    The ini file in which to look for the construction
 *                    information for this reader.
 * @param[in]      *sectionName
 *                    The section name within the ini file that holds
 *                    the construction information.
 *
 * @return  Returns a new reader object that is constructed from the
 *          information in the ini file.
 */
extern gridReader_t
gridReader_newFromIni(parse_ini_t ini, const char *sectionName);


/**
 * @brief  Deletes a reader objects and frees the associated memory.
 *
 * @param[in,out]  *reader
 *                    A pointer to the external variable holding the
 *                    refernce to the reader object that should be
 *                    deleted.  The external variable will be set to
 *                    NULL after the reader has been deleted.
 *
 * @return  Returns nothing.
 */
extern void
gridReader_del(gridReader_t *reader);


/** @} */

/**
 * @name  Using
 *
 * @{
 */

/**
 * @brief  Reads from the file and fills a provided patch.
 *
 * @param[in,out]  reader
 *                    The reader that should be used.
 * @param[in,out]  patch
 *                    The patch which should be filled.
 *
 * @return  Returns nothing.
 */
extern void
gridReader_readIntoPatch(gridReader_t reader, gridPatch_t patch);


/**
 * @brief  Reads from the file and fills only a specific variable of the
 *         patch.
 *
 * This works like gridReader_readIntoPatch() but only one variable
 * (a patch can hold multiple variables) is read from the file.
 *
 * @param[in,out]  reader
 *                    The reader that should be used.
 * @param[in,out]  patch
 *                    The patch which should be filled.
 * @param[in]      idxOfVar
 *                    The index of the variable in the patch that should
 *                    be used.
 *
 * @return  Returns nothing.
 */
extern void
gridReader_readIntoPatchForVar(gridReader_t reader,
                               gridPatch_t  patch,
                               int          idxOfVar);


/** @} */

#endif
