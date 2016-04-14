// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef refineGrid_H
#define refineGrid_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file refineGrid/refineGrid.h
 * @ingroup  toolsrefineGrid
 * @brief  Provides the interface to the refineGrid tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "refineGridConfig.h"
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Provides a handle for the refineGrid application. */
typedef struct refineGrid_struct *refineGrid_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creates a new refineGrid application from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file that should be used to set up the
 *                    application.
 *
 * @return  Returns a new refineGrid application.
 */
extern refineGrid_t
refineGrid_newFromIni(parse_ini_t ini);


/**
 * @brief  Executes the application.
 *
 * @param[in,out]  rsc
 *                    The refineGrid application to execute.
 *
 * @return  Returns nothing.
 */
extern void
refineGrid_run(refineGrid_t rsc);


/**
 * @brief  Deletes a refineGrid application and frees the
 *         associated memory.
 *
 * @param[in,out]  *rsc
 *                     The realSpaceApplication that should be deleted.
 *
 * @return  Returns nothing.
 */
extern void
refineGrid_del(refineGrid_t *rsc);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsrefineGrid refineGrid
 * @ingroup  tools
 * @brief  Provides the refineGrid tool.
 *
 * This is a tool that can either downsample a given white noise field, or
 * impose a given white noise field on a (generated on the fly) larger white
 * noise field.  Please see @ref toolsRSCSynopsis for how to use the program
 * and @ref toolsRSCSetupIniFormat for how to write input files.
 *
 * The following gives a sample application (imposing the constraints of a
 * given white noise field on a larger white noise field).
 *
 * @code
 * [Setup]
 * boxsizeInMpch = 200.
 * inputDim1D = 512
 * outputDim1D = 1024
 * useFileForInput = true
 * readerSecName = inputReader
 * writerSecName = outputWriter
 * seedIn = 124
 * seedOut = 6553
 *
 * [inputReader]
 * readerType = grafic
 * readerSection = inputReader
 * graficFileName = ../00512/wn_delta
 *
 * [outputWriter]
 * writerType = grafic
 * writerSection = outputWriter
 * prefix = wn_
 * isWhiteNoise = true
 * size = 1024, 1024, 1024
 * iseed = 6553
 * @endcode
 */


#endif
