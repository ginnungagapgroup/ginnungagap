// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridStatistics.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "gridVar.h"
#include "gridVarType.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridRegularDistrib.h"
#include "../libutil/xmem.h"
#include "../libutil/utilMath.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridStatistics_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_nullStat(gridStatistics_t stat);

static void
local_calcRegularCore(gridStatistics_t           stat,
                      const gridRegularDistrib_t distrib,
                      const gridRegular_t        grid,
                      const gridPatch_t          patch,
                      int                        idxOfVar);

static void
local_calcRegularMeanMinMax(gridStatistics_t           stat,
                            const gridRegularDistrib_t distrib,
                            const gridRegular_t        grid,
                            const gridPatch_t          patch,
                            int                        idxOfVar,
                            int                        numPatches,
                            uint64_t                   norm);

static void
local_calcRegularVarSkewKurt(gridStatistics_t           stat,
                             const gridRegularDistrib_t distrib,
                             const gridRegular_t        grid,
                             const gridPatch_t          patch,
                             int                        idxOfVar,
                             int                        numPatches,
                             uint64_t                   norm);

static void
local_calcProtoMeanMinMax(void      *data,
                          gridVar_t var,
                          uint64_t  len,
                          double    *protoMean,
                          double    *min,
                          double    *max);


static void
local_calcProtoVarSkewKurt(void         *data,
                           gridVar_t    var,
                           uint64_t     len,
                           double       *variance,
                           double       *skew,
                           double       *kurt,
                           const double mean);


#ifdef WITH_MPI
static void
local_mpiCalcProtoMeanMinMax(MPI_Comm comm,
                             double   *mean,
                             double   *min,
                             double   *max);

static void
local_mpiCalcProtoVarSkewKurt(MPI_Comm comm,
                              double   *var,
                              double   *skew,
                              double   *kurt);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern gridStatistics_t
gridStatistics_new(void)
{
	gridStatistics_t stat;

	stat = xmalloc(sizeof(struct gridStatistics_struct));
	local_nullStat(stat);

	return stat;
}

extern void
gridStatistics_del(gridStatistics_t *stat)
{
	assert(stat != NULL && *stat != NULL);

	xfree(*stat);

	*stat = NULL;
}

extern void
gridStatistics_calcGridPatch(gridStatistics_t  stat,
                             const gridPatch_t patch,
                             int               idxOfVar)
{
	assert(stat != NULL);
	assert(patch != NULL);

	local_nullStat(stat);
	local_calcRegularCore(stat, NULL, NULL, patch, idxOfVar);
	stat->valid = true;
}

extern void
gridStatistics_calcGridRegular(gridStatistics_t    stat,
                               const gridRegular_t grid,
                               int                 idxOfVar)
{
	assert(stat != NULL);
	assert(grid != NULL);

	local_nullStat(stat);
	local_calcRegularCore(stat, NULL, grid, NULL, idxOfVar);
	stat->valid = true;
}

extern void
gridStatistics_calcGridRegularDistrib(gridStatistics_t     stat,
                                      gridRegularDistrib_t distrib,
                                      int                  idxOfVar)
{
	assert(stat != NULL);
	assert(distrib != NULL);

	local_nullStat(stat);
	local_calcRegularCore(stat, distrib,
	                      gridRegularDistrib_getGridHandle(distrib),
	                      NULL, idxOfVar);
	stat->valid = true;
}

extern void
gridStatistics_invalidate(gridStatistics_t stat)
{
	assert(stat != NULL);

	stat->valid = false;
}

extern bool
gridStatistics_isValid(const gridStatistics_t stat)
{
	assert(stat != NULL);

	return stat->valid ? true : false;
}

extern double
gridStatistics_getMean(const gridStatistics_t stat)
{
	assert(stat != NULL);

	return stat->mean;
}

extern double
gridStatistics_getSD(const gridStatistics_t stat)
{
	assert(stat != NULL);

	return sqrt(stat->var);
}

extern double
gridStatistics_getVariance(const gridStatistics_t stat)
{
	assert(stat != NULL);

	return stat->var;
}

extern double
gridStatistics_getSkew(const gridStatistics_t stat)
{
	assert(stat != NULL);

	return stat->skew;
}

extern double
gridStatistics_getKurtosis(const gridStatistics_t stat)
{
	assert(stat != NULL);

	return stat->kurt;
}

extern double
gridStatistics_getMin(const gridStatistics_t stat)
{
	assert(stat != NULL);

	return stat->min;
}

extern double
gridStatistics_getMax(const gridStatistics_t stat)
{
	assert(stat != NULL);

	return stat->max;
}

extern void
gridStatistics_printPretty(const gridStatistics_t stat,
                           FILE                   *out,
                           const char             *prefix)
{
	if (prefix != NULL) {
		fprintf(out, "%s                mean  :  %15e\n", prefix,
		        gridStatistics_getMean(stat));
		fprintf(out, "%s  standard deviation  :  %15e\n", prefix,
		        gridStatistics_getSD(stat));
		fprintf(out, "%s            skewness  :  %15e\n", prefix,
		        gridStatistics_getSkew(stat));
		fprintf(out, "%s            kurtosis  :  %15e\n", prefix,
		        gridStatistics_getKurtosis(stat));
		fprintf(out, "%s             minimum  :  %15e\n", prefix,
		        gridStatistics_getMin(stat));
		fprintf(out, "%s             maximum  :  %15e\n", prefix,
		        gridStatistics_getMax(stat));
	} else {
		fprintf(out, "                mean  :  %15e\n",
		        gridStatistics_getMean(stat));
		fprintf(out, "  standard deviation  :  %15e\n",
		        gridStatistics_getSD(stat));
		fprintf(out, "            skewness  :  %15e\n",
		        gridStatistics_getSkew(stat));
		fprintf(out, "            kurtosis  :  %15e\n",
		        gridStatistics_getKurtosis(stat));
		fprintf(out, "             minimum  :  %15e\n",
		        gridStatistics_getMin(stat));
		fprintf(out, "             maximum  :  %15e\n",
		        gridStatistics_getMax(stat));
	}
}

/*--- Implementations of local functions --------------------------------*/
static void
local_nullStat(gridStatistics_t stat)
{
	stat->valid = false;
	stat->mean  = 0.0;
	stat->var   = 0.0;
	stat->skew  = 0.0;
	stat->kurt  = 0.0;
	stat->min   = 1e50;
	stat->max   = -1e50;
}

static void
local_calcRegularCore(gridStatistics_t           stat,
                      const gridRegularDistrib_t distrib,
                      const gridRegular_t        grid,
                      const gridPatch_t          patch,
                      int                        idxOfVar)
{
	uint64_t norm       = 2;
	int      numPatches = 1;

	if (grid != NULL) {
		numPatches = gridRegular_getNumPatches(grid);
		norm       = gridRegular_getNumCellsTotal(grid);
	} else if (patch != NULL) {
		norm = gridPatch_getNumCells(patch);
	}

	local_calcRegularMeanMinMax(stat, distrib, grid, patch,
	                            idxOfVar, numPatches, norm);
	local_calcRegularVarSkewKurt(stat, distrib, grid, patch,
	                             idxOfVar, numPatches, norm);
}

static void
local_calcRegularMeanMinMax(gridStatistics_t           stat,
                            const gridRegularDistrib_t distrib,
                            const gridRegular_t        grid,
                            const gridPatch_t          patch,
                            int                        idxOfVar,
                            int                        numPatches,
                            uint64_t                   norm)
{
	gridPatch_t myPatch;
	gridVar_t   gridVar;
	void        *data;
	uint64_t    len;
	double      min, max;

	for (int i = 0; i < numPatches; i++) {
		min = 1e50;
		max = -1e50;

		if (grid != NULL)
			myPatch = gridRegular_getPatchHandle(grid, i);
		else
			myPatch = patch;

		gridVar = gridPatch_getVarHandle(myPatch, idxOfVar);
		data    = gridPatch_getVarDataHandle(myPatch, idxOfVar);
		len     = gridPatch_getNumCells(myPatch);

		local_calcProtoMeanMinMax(data, gridVar, len, &(stat->mean),
		                          &min, &max);
		stat->min = (stat->min > min) ? min : stat->min;
		stat->max = (stat->max < max) ? max : stat->max;
	}

	if (distrib != NULL) {
#ifdef WITH_MPI
		MPI_Comm thisComm = gridRegularDistrib_getGlobalComm(distrib);
		local_mpiCalcProtoMeanMinMax(thisComm, &(stat->mean),
		                             &(stat->min), &(stat->max));
#endif
	}

	stat->mean /= norm;
} /* local_calcRegularMeanMinMax */

static void
local_calcRegularVarSkewKurt(gridStatistics_t           stat,
                             const gridRegularDistrib_t distrib,
                             const gridRegular_t        grid,
                             const gridPatch_t          patch,
                             int                        idxOfVar,
                             int                        numPatches,
                             uint64_t                   norm)
{
	gridPatch_t myPatch;
	gridVar_t   gridVar;
	void        *data;
	uint64_t    len;

	for (int i = 0; i < numPatches; i++) {
		if (grid != NULL)
			myPatch = gridRegular_getPatchHandle(grid, i);
		else
			myPatch = patch;

		gridVar = gridPatch_getVarHandle(myPatch, idxOfVar);
		data    = gridPatch_getVarDataHandle(myPatch, idxOfVar);
		len     = gridPatch_getNumCells(myPatch);

		local_calcProtoVarSkewKurt(data, gridVar, len, &(stat->var),
		                           &(stat->skew), &(stat->kurt),
		                           stat->mean);
	}

	if (distrib != NULL) {
#ifdef WITH_MPI
		MPI_Comm thisComm = gridRegularDistrib_getGlobalComm(distrib);
		local_mpiCalcProtoVarSkewKurt(thisComm, &(stat->var),
		                              &(stat->skew), &(stat->kurt));
#endif
	}

	stat->var  /= (norm - 1);
	stat->skew /= (norm * stat->var * sqrt(stat->var));
	stat->kurt  = stat->kurt / (norm * POW2(stat->var)) - 3;
}

static void
local_calcProtoMeanMinMax(void      *data,
                          gridVar_t var,
                          uint64_t  len,
                          double    *protoMean,
                          double    *min,
                          double    *max)
{
	union { double *lf;
		    int    *i;
		    int8_t *i8;
		    fpv_t  *fpv;
		    void   *v;
	}             tmp;
	gridVarType_t type = gridVar_getType(var);

	for (uint64_t i = 0; i < len; i++) {
		tmp.v = gridVar_getPointerByOffset(var, data, i);
		switch (type) {
		case GRIDVARTYPE_INT:
			*min        = (*(tmp.i) < *min) ? (double)*(tmp.i) : *min;
			*max        = (*(tmp.i) > *max) ? (double)*(tmp.i) : *max;
			*protoMean += (double)*(tmp.i);
			break;
		case GRIDVARTYPE_INT8:
			*min        = (*(tmp.i8) < *min) ? (double)*(tmp.i8) : *min;
			*max        = (*(tmp.i8) > *max) ? (double)*(tmp.i8) : *max;
			*protoMean += (double)*(tmp.i);
			break;
		case GRIDVARTYPE_DOUBLE:
			*min        = (*(tmp.lf) < *min) ? *(tmp.lf) : *min;
			*max        = (*(tmp.lf) > *max) ? *(tmp.lf) : *max;
			*protoMean += *(tmp.lf);
			break;
		case GRIDVARTYPE_FPV:
			*min        = (*(tmp.fpv) < *min) ? (double)*(tmp.fpv) : *min;
			*max        = (*(tmp.fpv) > *max) ? (double)*(tmp.fpv) : *max;
			*protoMean += (double)*(tmp.fpv);
			break;
		default:
			diediedie(999);
		}
	}
}

static void
local_calcProtoVarSkewKurt(void         *data,
                           gridVar_t    var,
                           uint64_t     len,
                           double       *variance,
                           double       *skew,
                           double       *kurt,
                           const double mean)
{
	union { double *lf;
		    int    *i;
		    int    *i8;
		    fpv_t  *fpv;
		    void   *v;
	}             tmp;
	double        tmpNo, tmpSqr;
	gridVarType_t type = gridVar_getType(var);

	for (uint64_t i = 0; i < len; i++) {
		tmp.v = gridVar_getPointerByOffset(var, data, i);
		switch (type) {
		case GRIDVARTYPE_INT:
			tmpNo = (double)*(tmp.i) - mean;
			break;
		case GRIDVARTYPE_INT8:
			tmpNo = (double)*(tmp.i8) - mean;
			break;
		case GRIDVARTYPE_DOUBLE:
			tmpNo = *(tmp.lf) - mean;
			break;
		case GRIDVARTYPE_FPV:
			tmpNo = (double)*(tmp.fpv) - mean;
			break;
		default:
			diediedie(999);
		}
		tmpSqr     = POW2(tmpNo);
		*variance += tmpSqr;
		*skew     += tmpSqr * tmpNo;
		*kurt     += POW2(tmpSqr);
	}
}

#ifdef WITH_MPI
static void
local_mpiCalcProtoMeanMinMax(MPI_Comm comm,
                             double   *mean,
                             double   *min,
                             double   *max)
{
	double myMean = *mean;
	double myMin  = *min;
	double myMax  = *max;

	MPI_Allreduce(&myMean, mean, 1, MPI_DOUBLE, MPI_SUM, comm);
	MPI_Allreduce(&myMin, min, 1, MPI_DOUBLE, MPI_MIN, comm);
	MPI_Allreduce(&myMax, max, 1, MPI_DOUBLE, MPI_MAX, comm);
}

static void
local_mpiCalcProtoVarSkewKurt(MPI_Comm comm,
                              double   *var,
                              double   *skew,
                              double   *kurt)
{
	double tmpIn[3], tmpOut[3];

	tmpIn[0] = *var;
	tmpIn[1] = *skew;
	tmpIn[2] = *kurt;

	MPI_Allreduce(tmpIn, tmpOut, 3, MPI_DOUBLE, MPI_SUM, comm);

	*var  = tmpOut[0];
	*skew = tmpOut[1];
	*kurt = tmpOut[2];
}

#endif
