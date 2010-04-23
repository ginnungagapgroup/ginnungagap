// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegularDistrib.h"
#include <assert.h>
#ifdef WITH_MPI
#  include "../libutil/commScheme.h"
#  include "../libutil/commSchemeBuffer.h"
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegularDistrib_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_calcProcCoords(gridRegularDistrib_t distrib,
                     int                  rank,
                     gridPointInt_t       procCoords);


#ifdef WITH_MPI
static void
local_transposeVarMPI(gridRegularDistrib_t distrib,
                      int                  idxVar,
                      int                  dimA,
                      int                  dimB);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern gridRegularDistrib_t
gridRegularDistrib_new(gridRegular_t grid, gridPointInt_t nProcs)
{
	gridRegularDistrib_t distrib;

	assert(grid != NULL);

	distrib           = xmalloc(sizeof(struct gridRegularDistrib_struct));
	distrib->numProcs = 1;
	for (int i = 0; i < NDIM; i++) {
		distrib->nProcs[i] = (nProcs != NULL) ? nProcs[i] : 1;
		if (distrib->nProcs[i] < 1)
			distrib->nProcs[i] = 1;
		distrib->numProcs *= distrib->nProcs[i];
	}
	distrib->grid       = gridRegular_getRef(grid);
#ifdef WITH_MPI
	distrib->commGlobal = MPI_COMM_NULL;
	distrib->commCart   = MPI_COMM_NULL;
#endif

	refCounter_init(&(distrib->refCounter));

	return gridRegularDistrib_getRef(distrib);
}

extern void
gridRegularDistrib_del(gridRegularDistrib_t *distrib)
{
	assert(distrib != NULL && *distrib != NULL);

	if (refCounter_deref(&((*distrib)->refCounter))) {
		gridRegular_del(&((*distrib)->grid));
#ifdef WITH_MPI
		if ((*distrib)->commGlobal != MPI_COMM_NULL)
			MPI_Comm_free(&((*distrib)->commGlobal));
		if ((*distrib)->commCart != MPI_COMM_NULL)
			MPI_Comm_free(&((*distrib)->commCart));
#endif
		xfree(*distrib);

		*distrib = NULL;
	}
}

extern gridRegularDistrib_t
gridRegularDistrib_getRef(gridRegularDistrib_t distrib)
{
	assert(distrib != NULL);

	refCounter_ref(&(distrib->refCounter));

	return distrib;
}

#ifdef WITH_MPI
extern void
gridRegularDistrib_initMPI(gridRegularDistrib_t distrib,
                           gridPointInt_t       nProcs,
                           MPI_Comm             comm)
{
	gridPointInt_t periodicity;
	int            size;

	assert(distrib != NULL);
	assert(comm != MPI_COMM_NULL);

	if (nProcs != NULL) {
		for (int i = 0; i < NDIM; i++)
			distrib->nProcs[i] = nProcs[i];
	}
	MPI_Comm_size(comm, &size);
	MPI_Dims_create(size, NDIM, distrib->nProcs);
	distrib->numProcs = 1;
	for (int i = 0; i < NDIM; i++) {
		periodicity[i]     = 0; // means false
		distrib->numProcs *= distrib->nProcs[i];
	}
	assert(distrib->numProcs == size);

	MPI_Comm_dup(comm, &(distrib->commGlobal));
	MPI_Cart_create(comm, NDIM, distrib->nProcs,
	                periodicity, 1, &(distrib->commCart));
}

extern int
gridRegularDistrib_getLocalRank(gridRegularDistrib_t distrib)
{
	int rank;

	assert(distrib != NULL);

	MPI_Comm_rank(distrib->commCart, &rank);

	return rank;
}

extern void
gridRegularDistrib_getProcCoords(gridRegularDistrib_t distrib,
                                 gridPointInt_t       procCoords)
{
	int localRank;
	assert(distrib != NULL);

	localRank = gridRegularDistrib_getLocalRank(distrib);
	MPI_Cart_coords(distrib->commCart, localRank, NDIM, procCoords);
}

#endif
extern gridPatch_t
gridRegularDistrib_getPatchForRank(gridRegularDistrib_t distrib, int rank)
{
	gridPointInt_t    procCoords;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	gridPointUint32_t gridDims;

	assert(distrib != NULL);
	assert(rank >= 0 && rank < distrib->numProcs);

	local_calcProcCoords(distrib, rank, procCoords);
	gridRegular_getDims(distrib->grid, gridDims);

	for (int i = 0; i < NDIM; i++) {
		gridRegularDistrib_calcIdxsForRank1D(gridDims[i],
		                                     distrib->nProcs[i],
		                                     procCoords[i],
		                                     idxLo + i,
		                                     idxHi + i);
	}

	return gridPatch_new(idxLo, idxHi);
}

extern void
gridRegularDistrib_getNProcs(gridRegularDistrib_t distrib,
                             gridPointInt_t       nProcs)
{
	assert(distrib != NULL);
	assert(nProcs != NULL);

	for (int i = 0; i < NDIM; i++)
		nProcs[i] = distrib->nProcs[i];
}

extern void
gridRegularDistrib_calcIdxsForRank1D(uint32_t nCells,
                                     int      nProcs,
                                     int      rank,
                                     uint32_t *idxLo,
                                     uint32_t *idxHi)
{
	int cellsPerProc;
	int cellsUnbalance;

	assert(nCells > 0);
	assert(nProcs > 0 && nProcs < nCells);
	assert(rank >= 0 && rank < nProcs);
	assert(idxLo != NULL);
	assert(idxHi != NULL);

	cellsPerProc   = nCells / nProcs;
	cellsUnbalance = nCells % nProcs;

	*idxLo         = rank * cellsPerProc;
	if (rank < cellsUnbalance)
		*idxLo += rank;
	else
		*idxLo += cellsUnbalance;
	*idxHi = *idxLo + cellsPerProc - 1;
	if (rank < cellsUnbalance)
		(*idxHi)++;
}

extern void
gridRegularDistrib_transposeVar(gridRegularDistrib_t distrib,
                                int                  idxVar,
                                int                  dimA,
                                int                  dimB)
{
	gridPatch_t patch;

	assert(distrib != NULL);
	assert((idxVar >= 0)
	       && (idxVar < gridRegular_getNumVars(distrib->grid)));
	assert(dimA >= 0 && dimA < NDIM);
	assert(dimB >= 0 && dimB < NDIM);

	patch = gridRegular_getPatchHandle(distrib->grid, 0);

#ifdef WITH_MPI
	local_transposeVarMPI(distrib, patch, idxVar, dimA, dimB);
#endif

	gridPatch_transposeVar(patch, idxVar, dimA, dimB);
}

/*--- Implementations of local functions --------------------------------*/
static void
local_calcProcCoords(gridRegularDistrib_t distrib,
                     int                  rank,
                     gridPointInt_t       procCoords)
{
#ifdef WITH_MPI
	MPI_Cart_coords(distrib->commCart, rank, NDIM, procCoords);
#else
	for (int i = 0; i < NDIM; i++) {
		procCoords[i] = rank % distrib->nProcs[i];
		rank         /= distrib->nProcs[i];
	}
#endif
}

#ifdef WITH_MPI

extern void
local_transposeVarMPI(gridRegularDistrib_t distrib,
                      gridPatch_t          patch,
                      int                  idxVar,
                      int                  dimA,
                      int                  dimB)
{
	commScheme_t      scheme;
	gridPointUint32_t dims;
	int               rank;
	gridPointInt_t    pPos;

	gridRegular_getDims(distrib->grid, dims);
	MPI_Comm_rank(distrib->commCart);
	MPI_Cart_coords(distrib->commCart, rank, NDIM, pPos);
	scheme = local_transposeGetCommScheme(distrib->commCart, patch, idxVar,
	                                      dims, distrib->nProcs, pPos);

	commScheme_fire(scheme);
	commScheme_wait(scheme);
	commScheme_del(&scheme);
}

static commScheme_t
local_transposeGetCommScheme(MPI_Comm          comm,
                             gridPatch_t       patch,
                             int               idxVar,
                             gridPointUint32_t dims,
                             gridPointInt_t    nProcs,
                             gridPointInt_t    pPos)
{
	commScheme_t scheme;
	varArr_t     sendLayout;
	varArr_t     recvLayout;

	scheme     = commScheme_new(comm, 4223);

	sendLayout = local_transposeGetSendLayout(dims, nProcs, pPos);
	recvLayout = local_transposeGetRecvLayout(dims, nProcs, pPos);

	local_getSendBuffers(comm, patch, idxVar, sendLayout);

	while (varArr_length(sendLayout > 0))
		xfree(varArr_remove(sendLayout, 0));
	varArr_del(&sendLayout);
	while (varArr_length(recvLayout > 0))
		xfree(varArr_remove(recvLayout, 0));
	varArr_del(&recvLayout);

	return scheme;
}

static void
local_getSendBuffers(MPI_Comm comm;
                     gridPatch_t patch,
                     int         idxVar,
                     varArr_t    send)
{
	gridVar_t var   = patch_getVarHandle(patch, idxVar);
	void      *data = patch_getVarDataHandle(patch, idxVar);

	for (int i = 0; i < varArr_length(send); i++) {
		void                  *dataSend;
		uint64_t              dataSize;
		local_layoutElement_t le = varArr_getElementHandle(send, i);
		MPI_Datatype          type;
		int                   count;
		int                   rank;

		dataSend = gridPatch_getWindowedDataCopy(patch, idxVar,
		                                         le->idxLo, le->idxHi,
		                                         &dataSize);
		type = gridVar_getMPIDataype(var);
		count = gridVar_getMPICount(var, dataSize);
		MPI_Cart_rank(comm, le->processCoord, &rank);
		le->buffer = commSchemeBuffer_new(dataSend, count, type, rank);

		// XXX
	}
}

#  define getIdx gridRegularDistrib_calcIdxsForRank1D
#  define is     gridUtil_intersection
#  define vAi    varArr_insert
static varArr_t
local_transposeGetSendLayout(gridPointUint32_t dims,
                             gridPointInt_t    nProcs,
                             gridPointInt_t    pPos)
{
	gridPointUint32_t loMine, hiMine, lo, hi, loS, hiS;
	gridPointInt_t    p;
	varArr_t          layout = varArr_new(nProcs[0] * nProcs[1] / 20);

	getIdx(dims[1], nProcs[1], pPos[1], loMine + 1, hiMine + 1);
	getIdx(dims[0], nProcs[0], pPos[0], loMine + 0, hiMine + 0);
	for (p[1] = 0; p[1] < nProcs[1]; p[1]++) {
		getIdx(dims[0], nProcs[1], p[1], lo + 1, hi + 1);
		if (is(loMine[1], hiMine[1], lo[1], hi[1], loS + 1, hiS + 1)) {
			for (int p[0] = 0; p[0] < nProcs[0]; p[0]++) {
				getIdx(dims[1], nProcs[0], p[0], lo + 0, hi + 0);
				if (is(loMine[0], hiMine[0], lo[0], hi[0], loS, hiS)) {
					// i, j is the process we need to send loS,hiS to
					vAi(layout, local_layoutElement_new(loS, hiS, p));
				}
			}
		}
	}
	return layout;
}

static varArr_t
local_transposeGetRecvLayout(gridPointUint32_t dims,
                             gridPointInt_t    nProcs,
                             gridPointInt_t    pPos)
{
	gridPointUint32_t loMine, hiMine, lo, hi, loS, hiS;
	gridPointInt_t    p;
	varArr_t          layout = varArr_new(nProcs[0] * nProcs[1] / 20);

	getIdx(dims[0], nProcs[1], pPos[1], loMine + 1, hiMine + 1);
	getIdx(dims[1], nProcs[0], pPos[0], loMine + 0, hiMine + 0);
	for (p[1] = 0; p[1] < nProcs[1]; p[1]++) {
		getIdx(dims[1], nProcs[1], p[1], lo + 1, hi + 1);
		if (is(loMine[1], hiMine[1], lo[1], hi[1], loS + 1, hiS + 1)) {
			for (p[0] = 0; p[0] < nProcs[0]; p[0]++) {
				getIdx(dims[0], nProcs[0], p[0], lo + 0, hi + 0);
				if (is(loMine[0], hiMine[0], lo[0], hi[0], loS, hiS)) {
					// i, j is the process we will receive loS,hiS from
					vAi(layout, local_layoutElement_new(loS, hiS, p));
				}
			}
		}
	}
	return layout;
}

#  undef vAi
#  undef intersect
#  undef getIdx

static local_layoutElement_t
local_layoutElement_new(gridPointUint32_t idxLo,
                        gridPointUint32_t idxHi,
                        gridPointInt_t    processCoord)
{
	local_transposeLayout_t element;

	element = xmalloc(sizeof(struct local_transposeLayout_struct));
	for (int i = 0; i < NDIM; i++) {
		element->idxLo[i]        = loS[i];
		element->idxHi[i]        = hiS[i];
		element->processCoord[i] = processCoord[i];
		element->buffer          = NULL;
	}

	return element;
}

#endif
