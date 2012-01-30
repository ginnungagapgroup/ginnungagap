// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDSTATISTICS_ADT_H
#define GRIDSTATISTICS_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridStatistics_adt.h
 * @ingroup libgridAnalysisStat
 * @brief  This file provides the main structure of the statistics module.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure for the grid statistics. */
struct gridStatistics_struct {
	/** @brief  Flags if the statistics is valid or not. */
	bool   valid;
	/** @brief  The mean of the data. */
	double mean;
	/** @brief  The variance of the data. */
	double var;
	/** @brief  The skewness of the data. */
	double skew;
	/** @brief  The kurtosis of the data. */
	double kurt;
	/** @brief  The minimum value in the data. */
	double min;
	/** @brief  The maximum value in the data. */
	double max;
};


#endif
