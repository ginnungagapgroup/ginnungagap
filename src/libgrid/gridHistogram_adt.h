// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDHISTOGRAM_ADT_H
#define GRIDHISTOGRAM_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridHistogram_adt.h
 * @ingroup libgridAnalysisHisto
 * @brief  This file provides the main structure for grid histograms.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure of the grid histogram. */
struct gridHistogram_struct {
	/** @brief  Holds the number of bins. */
	uint32_t numBins;
	/** @brief  Holds the number of bins that are used. */
	uint32_t numBinsReal;
	/** @brief  Stores the corners of the bins. */
	double   *binLimits;
	/** @brief  Stores the number of counts in each bin. */
	uint32_t *binCounts;
	/** @brief  Stores the total number of counts. */
	uint64_t totalCounts;
	/** @brief  Stores the total number of counts within the covered range. */
	uint64_t totalCountsInRange;
};


#endif
