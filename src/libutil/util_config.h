// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef UTIL_CONFIG_H
#define UTIL_CONFIG_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/util_config.h
 * @ingroup libutil
 * @brief This is the main configuration header that all files in this
 *        library include.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutil Utility library
 * @brief This library provides various small helper functions and
 *        modules.
 */

/**
 * @defgroup libutilFiles Files
 * @ingroup libutil
 * @brief Provides low-level interfaces to various file types.
 */

/**
 * @defgroup libutilFilesBov BOV
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to BOV.
 *
 * A BOV is a very simple text file describing a binary file that
 * contains a 'Brick Of Values'.  The file contains a set of key/value
 * pairs that give enough information to be able to have a minimal
 * interpretation of the binary blob.  This is very useful to dump
 * multidimensional (well, three dimensional) arrays for visualization
 * purposes.
 *
 * The following describes the content of a fiducial <tt>data.bov</tt>;
 * note the comment lines are allowed and start with a hash.  Also, the
 * file <b>is case-sensitive</b>:
 *
 * <pre> #           The following keys are required
 * # The time value of the data
 * TIME: 12.3
 * # The name of the file that contains the data.  Note that paths are
 * # allowed and the location of the data file is treated to be relative
 * # to the position of data.bov.
 * DATA_FILE: data.dat
 * # The size of data block, i.e. the dimensions of the array.  Note
 * # that for 1D or 2D data, one or two components should be 1.
 * DATA_SIZE: 45 34 23
 * # The format of the data. Allowed values: DOUBLE|FLOAT|INT|BYTE
 * DATA_FORMAT: DOUBLE
 * # The name of the variable
 * VARIABLE: density
 * # The endianess of the data.  Allowed values: LITTLE|BIG
 * DATA_ENDIAN: LITTLE
 * # The centering of the data.  Allowed values: zonal|nodal
 * CENTERING: zonal
 * # The physical coordinate of the origin of the data block.
 * BRICK_ORIGIN: 0. 0. 0.
 * # The physical extent of the data block.
 * BRICK_SIZE: 4.1 2.3 5.5
 * #           The following keys are optional
 * # The number of bytes to skip from the beginning of DATA_FILE to
 * # reach the beginning of the actual data in the file.  Very useful to
 * # skip over the Fortran block delimiter.
 * BYTE_OFFSET: 4
 * # The number of components of the data.  This is used to store vector
 * # data.
 * DATA_COMPONENTS: 1
 * # This can be used to split up the brick into smaller bricklets, note
 * # that this is parsed but not handled here.
 * DIVIDE_BRICK: false
 * DATA_BRICKLETS: 1 1 1</pre>
 *
 * @sa  The BOV format is described in more detail in the VisIt manual.
 */

/**
 * @defgroup libutilFilesGadget Gadget
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to Gadget files.
 *
 */

/**
 * @defgroup libutilFilesGrafic Grafic
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to Grafic files.
 *
 */

/**
 * @defgroup libutilFilesIni INI
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to INI files.
 *
 */

/**
 * @defgroup libutilMisc Miscellaneous
 * @ingroup libutil
 * @brief Provides miscellaneous functions.
 */

/**
 * @defgroup libutilCore Core
 * @ingroup libutil
 * @brief Provides useful functions that are not C99 and wraps around
 *        common functions to die on errors.
 */

/**
 * @defgroup libutilParallel Parallel
 * @ingroup libutil
 * @brief Provides useful things in an MPI environment.
 */

#endif
