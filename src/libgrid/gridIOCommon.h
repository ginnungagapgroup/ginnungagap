// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDIOCOMMON_H
#define GRIDIOCOMMON_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridIOCommon.h
 * @ingroup  libgridIOCommon
 * @brief  Provides common functionality for grid reader and writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridIO.h"
#include "../libutil/parse_ini.h"
#include "../libutil/filename.h"


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  INI Files
 *
 * @{
 */

/**
 * @brief  Retrieves the IO type from an ini file.
 *
 * This checks the filed @c type.
 *
 * @param[in,out]  ini
 *                    The ini file from which to read.  This must be a valid
 *                    ini file object.  Passing @c NULL is undefined.
 * @param[in]      *secName
 *                    The section in which to look.
 *
 * @return  Returns the IO type.
 */
extern gridIO_type_t
gridIOCommon_getType(parse_ini_t ini, const char *secName);

/**
 * @brief  Retrieves the name of a file from an ini file.
 *
 * This checks the fileds @c path, @c prefix, @c qualifier, and @c suffix.
 *
 * @param[in,out]  ini
 *                    The ini file from which to read.  This must be a valid
 *                    ini file object.  Passing @c NULL is undefined.
 * @param[in]      *secName
 *                    The section in which to look.
 * @param[in]      allowEmpty
 *                    If @c true than no field is required, if @c false,
 *                    then at least the @c prefix field must be set.
 *
 * @return  Returns the filename object according to the information in the
 *          ini file.
 */
extern filename_t
gridIOCommon_getFileName(parse_ini_t ini, const char *secName, bool allowEmpty);

/**
 * @brief  Checks the value of the overwrite field in the ini file.
 *
 * This checks the filed @c overwriteFileIfExists.
 *
 * @param[in,out]  ini
 *                    The ini file from which to read.  This must be a valid
 *                    ini file object.  Passing @c NULL is undefined.
 * @param[in]      *secName
 *                    The section in which to look.
 *
 * @return  Returns the value of the field @c overwriteFileIfExists, or
 *          @c false if it didn't exist.
 */
extern bool
gridIOCommon_getOverwrite(parse_ini_t ini, const char *secName);

/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOCommon  Common IO Functionality
 * @ingroup libgridIO
 * @brief This provides functions used by writers and readers.
 */


#endif
