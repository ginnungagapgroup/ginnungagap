// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef checkZeroSETUP_H
#define checkZeroSETUP_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file checkZero/checkZeroSetup.h
 * @ingroup  toolscheckZeroSetup
 * @brief  Provides the interface to the setup of the checkZero
 *         tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "checkZeroConfig.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct checkZeroSetup_struct *
checkZeroSetup_t;


/*--- Implemention of main structure ------------------------------------*/
struct checkZeroSetup_struct {
	double   boxsizeInMpch;
	uint32_t inputDim1D;
	uint32_t outputDim1D;
	char     *readerSecName;
	char     *writerSecName;
	char	 *varName;
	bool	 addFields;
	char     *reader2SecName;
	bool	 doPk;
	char	 *PkFile;
};


/*--- Prototypes of exported functions ----------------------------------*/
extern checkZeroSetup_t
checkZeroSetup_newFromIni(parse_ini_t ini,
                                     const char  *sectionName);

extern void
checkZeroSetup_del(checkZeroSetup_t *setup);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolscheckZeroSetup Setup
 * @ingroup  toolscheckZero
 * @brief  Provides the setup for the checkZero tool.
 *
 * @section toolsRSCSetupIniFormat  Ini Format for Real Space Constraints
 *
 * @code
 * [Setup]
 * #
 * # Gives the boxsize in Mpc/h
 * boxsizeInMpch = <double>
 * #
 * # Gives the size of the input field
 * inputDim1D = <positive integer>
 * #
 * # Gives the size of the output field, note that outputDim1D and
 * # inputDim1D must be compatible, i.e. the larger one must be an integer
 * # multiple of the smaller one.
 * outputDim1D = <positive integer>
 * #
 * # Flag if a file should be used the read the input, otherwise the input
 * # will be generate from a RNG.  If an input file is used, then
 * # readerSecName must be provided and the input field cannot be written
 * # (as it is already available as a file).
 * useFileForInput = <true|false>
 * #
 * # Gives the RNG seed for the input grid, if the input needs to be
 * # generate on the fly (useFileForInput = false).
 * seedIn = <integer>
 * #
 * # The seed for the output grid.  Note that this is only used if the input
 * # grid is used as a constraint for the output grid, i.e. if the input
 * # grid is smaller than the output grid.
 * seedOut = <integer>
 * #
 * # The name of the section in which to find the construction information
 * # for the reader.
 * # Either give this (when useFileForInput = true)..
 * readerSecName = <string>
 * #
 * # ..or this (useFileForInput = false).
 * # This gives the section in which the construction information for the
 * # input writer is given.
 * writerInSecName = <string>
 * #
 * # This is section in which the construction information for the writer of
 * # the output grid can be found.
 * writerSecName = <string>
 * #
 * @endcode
 *
 * Please see @ref libgridIOOutIniFormat and @ref libgridIOInIniFormat for
 * details on how to write correct writer/reader sections.
 *
 */


#endif
