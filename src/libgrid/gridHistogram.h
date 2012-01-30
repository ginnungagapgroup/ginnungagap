// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDHISTOGRAM_H
#define GRIDHISTOGRAM_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridHistogram.h
 * @ingroup libgridAnalysisHisto
 * @brief  This file provides the interface to the histrograms for grids.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridRegularDistrib.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridHistogram_struct *gridHistogram_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridHistogram_t
gridHistogram_new(uint32_t numBins, double min, double max);

extern void
gridHistogram_del(gridHistogram_t *histo);

extern void
gridHistogram_calcGridPatch(gridHistogram_t   histo,
                            const gridPatch_t patch,
                            int               idxOfVar);

extern void
gridHistogram_calcGridRegular(gridHistogram_t     histo,
                              const gridRegular_t grid,
                              int                 idxOfVar);

extern void
gridHistogram_calcGridRegularDistrib(gridHistogram_t            histo,
                                     const gridRegularDistrib_t distrib,
                                     int                        idxOfVar);

extern uint32_t
gridHistogram_getCountInBin(const gridHistogram_t histo, uint32_t bin);

extern double
gridHistogram_getBinLimitLeft(const gridHistogram_t histo, uint32_t bin);

extern double
gridHistogram_getBinLimitRight(const gridHistogram_t histo, uint32_t bin);

extern void
gridHistogram_printPretty(const gridHistogram_t histo,
                          FILE                  *out,
                          const char            *prefix);

extern void
gridHistogram_printPrettyFile(const gridHistogram_t histo,
                              const char            *outFileName,
                              bool                  append,
                              const char            *prefix);

extern uint32_t
gridHistogram_estimateNumBinsFromCells(uint64_t numCells);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridAnalysisHisto Histograms
 * @ingroup libgridAnalysis
 * @brief This provides histograms for grids.
 */


#endif
