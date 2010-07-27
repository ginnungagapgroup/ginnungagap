// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "estimateMemReqConfig.h"
#include "estimateMemReq.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "../../src/libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "estimateMemReq_adt.h"


/*--- Prototypes of local functions -------------------------------------*/
static void
local_getProcessGrid(int *npTot, int pGrid[3]);

static void
local_printProcessGrid(int pGrid[3]);

static void
local_checkProcessNumbers(const int npTot, const int npY, const int npZ);

static int
local_getClosestFactor(const int npTot, const int np);

static void
local_getFactors(const int npTot, int *npY, int *npZ);

static void
local_printMem(size_t bytes);


/*--- Implementations of exported functios ------------------------------*/
extern estimateMemReq_t
estimateMemReq_new(int dim1D, bool isDouble)
{
	estimateMemReq_t emr;

	emr        = xmalloc(sizeof(struct estimateMemReq_struct));
	emr->dim1D = dim1D > 0 ? dim1D : 512;
	if (emr->dim1D > (1L << 17) - 2) {
		fprintf(stderr, "dim1D = %zu is too large, adjusting to %zu.\n",
		        emr->dim1D, (1L << 17) - 2);
		emr->dim1D = (1L << 17) - 2;
	}
	emr->bytesPerCell = isDouble ? 8 : 4;

	return emr;
}

extern void
estimateMemReq_del(estimateMemReq_t *emr)
{
	assert(emr != NULL);
	assert(*emr != NULL);

	xfree(*emr);
	*emr = NULL;
}

extern void
estimateMemReq_run(estimateMemReq_t emr,
                   int              npTot,
                   int              npY,
                   int              npZ,
                   size_t           memPerProcesInBytes)
{
	int pGrid[3] = { 1, npY, npZ };

	assert(emr != NULL);

	local_getProcessGrid(npTot, pGrid);
	local_printProcessGrid(pGrid);
}

#if 0
size_t memTotalMin, memTotalMax;
size_t memLocalMin, memLocalMax;
size_t numCellsTotal;
size_t dimPerProcMin[3], dimPerProcMax[3];

assert(emr != NULL);

printf("Processes:  %i x %i x %i  =  %i\n\n",
       emr->npX, emr->npY, emr->npZ, emr->npTot);


dimPerProcMin[0]  = emr->dims[0] / emr->npX;
dimPerProcMin[1]  = emr->dims[1] / emr->npY;
dimPerProcMin[2]  = emr->dims[2] / emr->npZ;
dimPerProcMax[0]  = dimPerProcMin[0];
dimPerProcMax[1]  = dimPerProcMin[1];
dimPerProcMax[2]  = dimPerProcMin[2];
dimPerProcMax[0] += (emr->dim1D % emr->npX == 0 ? 0 : 1);
dimPerProcMax[1] += (emr->dim1D % emr->npY == 0 ? 0 : 1);
dimPerProcMax[2] += (emr->dim1D % emr->npZ == 0 ? 0 : 1);

printf("Grid 1D         : %i\n", emr->dim1D);
printf("Grid required   : %zu x %zu x %zu\n",
       emr->dims[0], emr->dims[1], emr->dims[2]);
printf("Grid per process:  min: %zu x %zu x %zu\n",
       dimPerProcMin[0], dimPerProcMin[1], dimPerProcMin[2]);
printf("                   max: %zu x %zu x %zu\n\n",
       dimPerProcMax[0], dimPerProcMax[1], dimPerProcMax[2]);

memLocalMin    = dimPerProcMin[0] * dimPerProcMin[1]
                 * dimPerProcMin[2];
memLocalMin   *= emr->bytesPerCell;
memLocalMax    = dimPerProcMax[0] * dimPerProcMax[1]
                 * dimPerProcMax[2];
memLocalMax   *= emr->bytesPerCell;

numCellsTotal  = (size_t)emr->dim1D;
numCellsTotal *= (size_t)emr->dim1D;
numCellsTotal *= (size_t)emr->dim1D;
memTotalMin    = numCellsTotal * emr->bytesPerCell;
memTotalMax    = memLocalMax * emr->npTot;


printf("Minimal memory per task:  grid: ");
local_printMem(memLocalMin);
printf("  grid+buffer: ");
local_printMem(memLocalMin * 2);
printf("\n");
printf("Maximal memory per task:  grid: ");
local_printMem(memLocalMax);
printf("  grid+buffer: ");
local_printMem(memLocalMax * 2);
printf("\n");

printf("\nMinimal memory total   :  grid: ");
local_printMem(memTotalMin);
printf("  grid+buffer: ");
local_printMem(memTotalMin * 2);
printf("\n");
printf("Maximal memory total   :  grid: ");
local_printMem(memTotalMax);
printf("  grid+buffer: ");
local_printMem(memTotalMax * 2);
printf("\n");
} /* estimateMemReq_run */
#endif

/*--- Implementations of local functions --------------------------------*/
static void
local_getProcessGrid(int *npTot, int pGrid[3])
{
	local_getProcessNumbers(*npTot, pGrid + 1, pGrid + 2);

	pGrid[0] = 1;

	if (*npTot > 0) {
		if ((pGrid[1] > 0) && (pGrid[2] <= 0)) {
			pGrid[1] = local_getClosestFactor(*npTot, pGrid[1]);
			pGrid[2] = *npTot / pGrid[1];
		} else if ((pGrid[2] > 0) && (pGrid[1] <= 0)) {
			pGrid[2] = local_getClosestFactor(*npTot, pGrid[2]);
			pGrid[1] = *npTot / pGrid[2];
		} else if ((pGrid[1] <= 0) && (pGrid[2] <= 0)) {
			local_getFactors(*npTot, nGrid + 1, nGrid + 2);
		} else {
			assert(*npTot == pGrid[0] * pGrid[1] * pGrid[2]);
		}
	} else
		*npTot = pGrid[0] * pGrid[1] * pGrid[2];
}

static void
local_printProcessGrid(int pGrid[3])
{
	printf("Process grid:  %i x %i x %i  (%i total)\n",
	       pGrid[0], pGrid[1], pGrid[2], pGrid[0] * pGrid[1] * pGrid[2]);
}

static void
local_checkProcessNumbers(const int npTot, const int npY, const int npZ)
{
	long tmp = (long)npY * (long)npZ;
	if ((npTot > 1 << 30) || (npY > 1 << 30) || (npZ > 1 << 30)
	    || (tmp > 1L << 30)) {
		fprintf(stderr,
		        "Sorry, supporting only a maximum of %i processes.\n",
		        1 << 30);
		exit(EXIT_FAILURE);
	}
	if (!(((npTot <= 0) && (npY > 0) && (npZ > 0)) || (npTot > 0))) {
		fprintf(stderr,
		        "Sorry, you need to give either\n"
		        "  a) npTot\n"
		        "  b) npY and npZ\n"
		        "  c) npTot and npY\n"
		        "  d) npTot and npZ\n"
		        "  e) npTot, npY and npZ\n");
		exit(EXIT_FAILURE);
	}
	if ((npY > 0) && (npZ > 0) && (npTot > 0) && (npY * npZ != npTot)) {
		fprintf(stderr, "npY * npZ != npTot (%i * %i = %i)\n",
		        npY, npZ, npY * npZ);
		exit(EXIT_FAILURE);
	}
}

static int
local_getClosestFactor(const int npTot, const int np)
{
	int rest = npTot % np;
	int factor;

	factor = (np > npTot) ? npTot : np;
	if (rest > np / 2)
		factor += rest;
	else
		factor -= rest;

	return factor;
}

static void
local_getFactors(const int npTot, int *npY, int *npZ)
{
	int npTotSqrt = (int)round(sqrt((double)(*npTot)));

	*npY = local_getClosestFactor(npTotSqrt, *npY);
	*npZ = npTot / *npY;
}

static void
local_printMem(size_t bytes)
{
	if (bytes < 1024L)
		printf("%8.3i b  ", (int)bytes);
	else if (bytes < 1048576L)
		printf("%8.3f Kib", bytes / 1024.);
	else if (bytes < 1073741824L)
		printf("%8.3f Mib", bytes / 1048576.);
	else if (bytes < 1099511627776L)
		printf("%8.3f Gib", bytes / 1073741824.);
	else if (bytes < 1125899906842624L)
		printf("%8.3f Tib", bytes / 1099511627776.);
	else if (bytes < 1152921504606846976L)
		printf("%8.3f Pib", bytes / 1125899906842624.);
	else
		printf("%8.3f Eib", bytes / 1152921504606846976.);
}
