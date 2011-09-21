// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef ESTIMATEMEMREQ_H
#define ESTIMATEMEMREQ_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file estimateMemReq/estimateMemReq.h
 * @ingroup  toolsEstimateMemReq
 * @brief  Provides the interface to the estimateMemReq tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "estimateMemReqConfig.h"
#include <stdlib.h>
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct estimateMemReq_struct *estimateMemReq_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern estimateMemReq_t
estimateMemReq_new(int dim1D, bool isDouble);

extern void
estimateMemReq_del(estimateMemReq_t *emr);

extern void
estimateMemReq_run(estimateMemReq_t emr,
                   int              npTot,
                   int              npY,
                   int              npZ,
                   size_t           memPerProcessInBytes,
                   int              processesPerNode);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsEstimateMemReq estimateMemReq
 * @ingroup  tools
 * @brief  Provides the estimateMemReq tool.
 */


#endif
