// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef checkZero_H
#define checkZero_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file checkZero/checkZero.h
 * @ingroup  toolscheckZero
 * @brief  Provides the interface to the checkZero tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "checkZeroConfig.h"
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Provides a handle for the checkZero application. */
typedef struct checkZero_struct *checkZero_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creates a new checkZero application from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file that should be used to set up the
 *                    application.
 *
 * @return  Returns a new checkZero application.
 */
extern checkZero_t
checkZero_newFromIni(parse_ini_t ini);


/**
 * @brief  Executes the application.
 *
 * @param[in,out]  rsc
 *                    The checkZero application to execute.
 *
 * @return  Returns nothing.
 */
extern void
checkZero_run(checkZero_t rsc);


/**
 * @brief  Deletes a checkZero application and frees the
 *         associated memory.
 *
 * @param[in,out]  *rsc
 *                     The realSpaceApplication that should be deleted.
 *
 * @return  Returns nothing.
 */
extern void
checkZero_del(checkZero_t *rsc);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolscheckZero checkZero
 * @ingroup  tools
 * @brief  Provides the checkZero tool.
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
