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


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridReaderBov_struct *gridReaderBov_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridReaderBov_t
gridReaderBov_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
gridReaderBov_del(gridReader_t *reader);

extern void
gridReaderBov_readIntoPatch(gridReader_t reader, gridPatch_t patch);

extern void
gridReaderBov_readIntoPatchForVar(gridReader_t reader,
                                  gridPatch_t  patch,
                                  int          idxOfVar);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInBov BOV Reader
 * @ingroup libgridIOIn
 * @brief  Provides the BOV reader.
 * 
 * @section libgridIOInBovIniFormat  Expected Format for Ini Files
 *
 * @code
 * [SectionName]
 * @endcode
 */


#endif
