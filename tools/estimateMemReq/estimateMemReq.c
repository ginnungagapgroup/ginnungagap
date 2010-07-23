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
local_printMem(size_t bytes);


/*--- Implementations of exported functios ------------------------------*/
extern estimateMemReq_t
estimateMemReq_new(int    dim1D,
                   int    npTot,
                   int    npY,
                   size_t memPerProcInBytes,
                   bool   isDouble)
{
	estimateMemReq_t emr;

	emr        = xmalloc(sizeof(struct estimateMemReq_struct));
	emr->dim1D = dim1D > 0 ? dim1D : 512;
	if (emr->dim1D > (1L << 17) - 2) {
		fprintf(stderr, "dim1D = %zu is too large, adjusting to %zu.\n",
		        emr->dim1D, (1L << 17) - 2);
		emr->dim1D = (1L << 17) - 2;
	}
	emr->dims[0] = (emr->dim1D/2 + 1 ) * 2;
	emr->dims[1] = emr->dim1D;
	emr->dims[2] = emr->dim1D;
	emr->npTot = npTot > 0 ? npTot : 1;
	emr->npY   = (npY > 0 && npY <= emr->npTot) ?
	             npY : emr->npTot;
	if (emr->npTot % emr->npY != 0) {
		fprintf(stderr,
		        "npy = %i not valid (nptot %% npy != 0), changing to ",
		        emr->npY);
		emr->npY += emr->npTot % emr->npY;
		fprintf(stderr, "%i\n", emr->npY);
	}
	emr->npZ               = emr->npTot / emr->npY;
	emr->npX               = 1;
	emr->memPerProcInBytes = memPerProcInBytes;
	emr->bytesPerCell      = isDouble ? 8 : 4;

	return emr;
}

extern void
estimateMemReq_run(estimateMemReq_t emr)
{
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
	dimPerProcMax[0] += (emr->dim1D % emr->npX == 0 ? 0 : 1 );
	dimPerProcMax[1] += (emr->dim1D % emr->npY == 0 ? 0 : 1 );
	dimPerProcMax[2] += (emr->dim1D % emr->npZ == 0 ? 0 : 1 );

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

extern void
estimateMemReq_del(estimateMemReq_t *emr)
{
	assert(emr != NULL);
	assert(*emr != NULL);

	xfree(*emr);
	*emr = NULL;
}

/*--- Implementations of local functions --------------------------------*/
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
