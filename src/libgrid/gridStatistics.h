// Copyright (C) 2010, 2012 Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDSTATISTICS_H
#define GRIDSTATISTICS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridStatistics.h
 * @ingroup libgridAnalysisStat
 * @brief  This file provides the statistics module for grids.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridRegularDistrib.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the statistics object. */
typedef struct gridStatistics_struct *gridStatistics_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridStatistics_t
gridStatistics_new(void);

extern void
gridStatistics_del(gridStatistics_t *stat);

extern void
gridStatistics_calcGridPatch(gridStatistics_t  stat,
                             const gridPatch_t patch,
                             int               idxOfVar);

extern void
gridStatistics_calcGridRegular(gridStatistics_t    stat,
                               const gridRegular_t grid,
                               int                 idxOfvar);

extern void
gridStatistics_calcGridRegularDistrib(gridStatistics_t     stat,
                                      gridRegularDistrib_t distrib,
                                      int                  idxOfVar);

extern void
gridStatistics_invalidate(gridStatistics_t stat);

extern bool
gridStatistics_isValid(const gridStatistics_t stat);

extern double
gridStatistics_getMean(const gridStatistics_t stat);

extern double
gridStatistics_getSD(const gridStatistics_t stat);

extern double
gridStatistics_getVariance(const gridStatistics_t stat);

extern double
gridStatistics_getSkew(const gridStatistics_t stat);

extern double
gridStatistics_getKurtosis(const gridStatistics_t stat);

extern double
gridStatistics_getMin(const gridStatistics_t stat);

extern double
gridStatistics_getMax(const gridStatistics_t stat);

extern void
gridStatistics_printPretty(const gridStatistics_t stat,
                           FILE *out,
                           const char *prefix);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridAnalysisStat  Grid Statistics
 * @ingroup libgridAnalysis
 * @brief This provides statistics for the data in grids.
 */


#endif
