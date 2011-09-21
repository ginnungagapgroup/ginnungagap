// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef REALSPACECONSTRAINTSSETUP_H
#define REALSPACECONSTRAINTSSETUP_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file realSpaceConstraints/realSpaceConstraintsSetup.h
 * @ingroup  toolsRealSpaceConstraintsSetup
 * @brief  Provides the interface to the setup of the realSpaceConstraints
 *         tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "realSpaceConstraintsConfig.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct realSpaceConstraintsSetup_struct *
realSpaceConstraintsSetup_t;


/*--- Implemention of main structure ------------------------------------*/
struct realSpaceConstraintsSetup_struct {
	double   boxsizeInMpch;
	uint32_t inputDim1D;
	uint32_t outputDim1D;
	bool     useFileForInput;
	char     *readerSecName;
	char     *writerSecName;
	char     *writerInSecName;
	int      seedIn;
	int      seedOut;
};


/*--- Prototypes of exported functions ----------------------------------*/
extern realSpaceConstraintsSetup_t
realSpaceConstraintsSetup_newFromIni(parse_ini_t ini,
                                     const char  *sectionName);

extern void
realSpaceConstraintsSetup_del(realSpaceConstraintsSetup_t *setup);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsRealSpaceConstraintsSetup Setup
 * @ingroup  toolsRealSpaceConstraints
 * @brief  Provides the setup for the realSpaceConstraints tool.
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
