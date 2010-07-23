// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef ESTIMATEMEMREQ_H
#define ESTIMATEMEMREQ_H


/*--- Includes ----------------------------------------------------------*/
#include "estimateMemReqConfig.h"
#include <stdlib.h>
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct estimateMemReq_struct *estimateMemReq_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern estimateMemReq_t
estimateMemReq_new(int    dim1D,
                   int    npTot,
                   int    npY,
                   size_t memPerProcInBytes,
                   bool   isDouble);

extern void
estimateMemReq_run(estimateMemReq_t emr);

extern void
estimateMemReq_del(estimateMemReq_t *emr);


#endif
