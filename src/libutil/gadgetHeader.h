// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETHEADER_H
#define GADGETHEADER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetHeader.h
 * @ingroup libutilFilesGadget
 * @brief  This file provides the interface to Gadget headers.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>
#include <stdint.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct gadgetHeader_struct *gadgetHeader_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gadgetHeader_t
gadgetHeader_new(void);

extern gadgetHeader_t
gadgetHeader_clone(const gadgetHeader_t gadgetHeader);

extern void
gadgetHeader_del(gadgetHeader_t *gadgetHeader);

extern void
gadgetHeader_setNp(gadgetHeader_t gadgetHeader, uint32_t np[6]);

extern void
gadgetHeader_setMassArr(gadgetHeader_t gadgetHeader, double massarr[6]);

extern void
gadgetHeader_setExpansion(gadgetHeader_t gadgetHeader, double expansion);

extern void
gadgetHeader_setRedshift(gadgetHeader_t gadgetHeader, double redshift);

extern void
gadgetHeader_setFlagSfr(gadgetHeader_t gadgetHeader, int32_t flag);

extern void
gadgetHeader_setFlagFeedback(gadgetHeader_t gadgetHeader, int32_t flag);

extern void
gadgetHeader_setNall(gadgetHeader_t gadgetHeader, uint64_t nall[6]);

extern void
gadgetHeader_setFlagCooling(gadgetHeader_t gadgetHeader, int32_t flag);

extern void
gadgetHeader_setNumFiles(gadgetHeader_t gadgetHeader, int32_t numFiles);

extern void
gadgetHeader_setBoxsize(gadgetHeader_t gadgetHeader, double boxsize);

extern void
gadgetHeader_setOmega0(gadgetHeader_t gadgetHeader, double omega0);

extern void
gadgetHeader_setOmegaLambda(gadgetHeader_t gadgetHeader, double omegaLambda);

extern void
gadgetHeader_setHubbleParameter(gadgetHeader_t gadgetHeader, double h);

extern void
gadgetHeader_setFlagStellarAge(gadgetHeader_t gadgetHeader, int32_t flag);

extern void
gadgetHeader_setFlagMetal(gadgetHeader_t gadgetHeader, int32_t flag);

extern void
gadgetHeader_setFlagEntropy(gadgetHeader_t gadgetHeader, int32_t flag);

extern uint64_t
gadgetHeader_getNumPartsInFile(const gadgetHeader_t gadgetHeader);

extern void
gadgetHeader_write(gadgetHeader_t gadgetHeader, FILE *f);

#endif
