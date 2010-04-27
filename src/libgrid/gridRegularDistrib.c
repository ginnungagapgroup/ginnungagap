// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegularDistrib.h"
#include <assert.h>
#ifdef WITH_MPI
#  include "gridUtil.h"
#  include "../libutil/varArr.h"
#  include "../libutil/commScheme.h"
#  include "../libutil/commSchemeBuffer.h"
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegularDistrib_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local structures --------------------------------------------------*/
#ifdef WITH_MPI
typedef struct local_transposeLayout_struct *local_layoutElement_t;

struct local_transposeLayout_struct {
	gridPointUint32_t  idxLo;
	gridPointUint32_t  idxHi;
	gridPointInt_t     processCoord;
	commSchemeBuffer_t buffer;
};
#endif

/*--- Prototypes of local functions -------------------------------------*/
static void
local_calcProcCoords(gridRegularDistrib_t distrib,
                     int                  rank,
                     gridPointInt_t       procCoords);


#ifdef WITH_MPI
static void
local_transposeMPI(gridRegularDistrib_t distrib,
                   int                  dimA,
                   int                  dimB);

static gridPatch_t
local_transposeGetPatchT(gridPointUint32_t dimsT,
                         gridPointInt_t    nProcs,
                         gridPointInt_t    pPos,
                         int               dimA,
                         int               dimB);

static varArr_t
local_transposeGetSendLayout(gridPointUint32_t dims,
                             gridPointInt_t    nProcs,
                             gridPointInt_t    pPos,
                             int               dimA,
                             int               dimB);

static varArr_t
local_transposeGetRecvLayout(gridPointUint32_t dims,
                             gridPointInt_t    nProcs,
                             gridPointInt_t    pPos,
                             int               dimA,
                             int               dimB);

static local_layoutElement_t
local_layoutElement_new(gridPointUint32_t idxLo,
                        gridPointUint32_t idxHi,
                        gridPointInt_t    processCoord);

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
gridRegularDistrib_transpose(gridRegularDistrib_t distrib,
                             int                  dimA,
                             int                  dimB)
{
	assert(distrib != NULL);
	assert(dimA >= 0 && dimA < NDIM);
	assert(dimB >= 0 && dimB < NDIM);

#ifdef WITH_MPI
	local_transposeMPI(distrib, dimA, dimB);
#endif
	gridRegular_transpose(distrib->grid, dimA, dimB);
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

static void
local_transposeMPI(gridRegularDistrib_t distrib,
                   int                  dimA,
                   int                  dimB)
{
	int               rank;
	gridPointInt_t    pPos;
	gridPointUint32_t dims;
	varArr_t          sendLayout;
	varArr_t          recvLayout;

	MPI_Comm_rank(distrib->commCart, &rank);
	MPI_Cart_coords(distrib->commCart, rank, NDIM, pPos);
	gridRegular_getDims(distrib->grid, dims);

	sendLayout = local_transposeGetSendLayout(dims, distrib->nProcs, pPos,
	                                          dimA, dimB);
	recvLayout = local_transposeGetRecvLayout(dims, distrib->nProcs, pPos,
	                                          dimA, dimB);

	gridPatch_t patch   = gridRegular_getPatchHandle(distrib->grid, 0);
	int         numVars = gridPatch_getNumVars(patch);
	gridPatch_t patchT  = local_transposeGetPatchT(dims,
	                                               distrib->nProcs,
	                                               pPos, dimA, dimB);

	for (int i = 0; i < numVars; i++) {
		void         *data;
		int          idxOfVar;
		gridVar_t    var
		    = gridVar_getRef(gridPatch_getVarHandle(patch, 0));
		MPI_Datatype type   = gridVar_getMPIDatatype(var);
		commScheme_t scheme = commScheme_new(distrib->commCart, 4223);

#  ifdef DEBUG2
		printf("\nAm %i, sending to %i tasks\n", rank,
		       varArr_getLength(sendLayout));
#  endif
		for (int j = 0; j < varArr_getLength(sendLayout); j++) {
			void                  *dataSend;
			uint64_t              dataSize;
			local_layoutElement_t le = varArr_getElementHandle(sendLayout,
			                                                   j);
			int                   count;
			int                   rankSend;

			// We always work on the 0th variable as the patch is
			// emptied during the course of the main loop.
			dataSend = gridPatch_getWindowedDataCopy(patch, 0,
			                                         le->idxLo, le->idxHi,
			                                         &dataSize);
			count      = gridVar_getMPICount(var, dataSize);
			MPI_Cart_rank(distrib->commCart, le->processCoord, &rankSend);
			le->buffer = commSchemeBuffer_new(dataSend,
			                                  count,
			                                  type,
			                                  rankSend);
#  ifdef DEBUG2
			printf("\nAm %i, send to %i: %u %u - %u %u\n",
			       rank, rankSend, le->idxLo[0], le->idxLo[1],
			       le->idxHi[0], le->idxHi[1]);
#  endif
			commScheme_addBuffer(scheme, le->buffer, COMMSCHEME_TYPE_SEND);
		}

		gridVar_freeMemory(var, gridPatch_detachVarData(patch, 0));
		idxOfVar = gridPatch_attachVarData(patchT, var);
		data     = gridPatch_getVarDataHandle(patchT, idxOfVar);

		uint64_t offset = 0;
#  ifdef DEBUG2
		printf("\nAm %i, receiving from %i tasks\n", rank,
		       varArr_getLength(recvLayout));
#  endif
		for (int j = 0; j < varArr_getLength(recvLayout); j++) {
			local_layoutElement_t le = varArr_getElementHandle(recvLayout,
			                                                   j);
			int                   count;
			int                   rankRecv;
			void                  *dataRecv;
			uint64_t              numCells = 1;


			dataRecv = gridVar_getPointerByOffset(var, data, offset);
			for (int k = 0; k < NDIM; k++)
				numCells *= (le->idxHi[k] - le->idxLo[k] + 1);
			offset    += numCells;
			count      = gridVar_getMPICount(var, numCells);
			MPI_Cart_rank(distrib->commCart, le->processCoord, &rankRecv);
			le->buffer = commSchemeBuffer_new(dataRecv,
			                                  count,
			                                  type,
			                                  rankRecv);
#  ifdef DEBUG2
			printf("\nAm %i, receive from %i: %u %u - %u %u\n",
			       rank, rankRecv, le->idxLo[0], le->idxLo[1],
			       le->idxHi[0], le->idxHi[1]);
#  endif
			commScheme_addBuffer(scheme, le->buffer, COMMSCHEME_TYPE_RECV);
		}


		commScheme_fire(scheme);
		commScheme_wait(scheme);
		for (int j = 0; j < varArr_getLength(sendLayout); j++) {
			local_layoutElement_t le = varArr_getElementHandle(sendLayout,
			                                                   j);
			gridVar_freeMemory(var, commSchemeBuffer_getBuf(le->buffer));
		}
		commScheme_del(&scheme);
		gridVar_del(&var);
	}

	assert(gridPatch_getNumVars(patch) == 0);
	gridRegular_replacePatch(distrib->grid, 0, patchT);

	while (varArr_getLength(sendLayout) > 0)
		xfree(varArr_remove(sendLayout, 0));
	varArr_del(&sendLayout);
	while (varArr_getLength(recvLayout) > 0)
		xfree(varArr_remove(recvLayout, 0));
	varArr_del(&recvLayout);
} /* local_transposeMPI */

static gridPatch_t
local_transposeGetPatchT(gridPointUint32_t dims,
                         gridPointInt_t    nProcs,
                         gridPointInt_t    pPos,
                         int               dimA,
                         int               dimB)
{
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	uint32_t          tmp;

	tmp        = dims[dimA];
	dims[dimA] = dims[dimB];
	dims[dimB] = tmp;

	for (int i = 0; i < NDIM; i++)
		gridRegularDistrib_calcIdxsForRank1D(dims[i], nProcs[i], pPos[i],
		                                     idxLo + i, idxHi + i);

	tmp         = idxLo[dimA];
	idxLo[dimA] = idxLo[dimB];
	idxLo[dimB] = tmp;

	tmp         = idxHi[dimA];
	idxHi[dimA] = idxHi[dimB];
	idxHi[dimB] = tmp;

	return gridPatch_new(idxLo, idxHi);
}

#  define getIdx gridRegularDistrib_calcIdxsForRank1D
#  define is     gridUtil_intersection1D
#  define vAi    varArr_insert
static varArr_t
local_transposeGetSendLayout(gridPointUint32_t dims,
                             gridPointInt_t    nProcs,
                             gridPointInt_t    pPos,
                             int               dimA,
                             int               dimB)
{
	gridPointUint32_t loMine, hiMine, lo, hi, loS, hiS;
	gridPointInt_t    p;
	varArr_t          layout = varArr_new(nProcs[0] * nProcs[1] / 20);

	getIdx(dims[1], nProcs[1], pPos[1], loMine + 1, hiMine + 1);
	getIdx(dims[0], nProcs[0], pPos[0], loMine + 0, hiMine + 0);
	for (p[1] = 0; p[1] < nProcs[1]; p[1]++) {
		getIdx(dims[0], nProcs[1], p[1], lo + 1, hi + 1);
		if (is(loMine[0], hiMine[0], lo[1], hi[1], loS, hiS)) {
			for (p[0] = 0; p[0] < nProcs[0]; p[0]++) {
				getIdx(dims[1], nProcs[0], p[0], lo + 0, hi + 0);
				if (is(loMine[1], hiMine[1], lo[0], hi[0], loS + 1, hiS
				       + 1)) {
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
                             gridPointInt_t    pPos,
                             int               dimA,
                             int               dimB)
{
	gridPointUint32_t loMine, hiMine, lo, hi, loS, hiS;
	gridPointInt_t    p;
	varArr_t          layout = varArr_new(nProcs[0] * nProcs[1] / 20);

	getIdx(dims[0], nProcs[1], pPos[1], loMine + 0, hiMine + 0);
	getIdx(dims[1], nProcs[0], pPos[0], loMine + 1, hiMine + 1);
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
	local_layoutElement_t element;

	element = xmalloc(sizeof(struct local_transposeLayout_struct));
	for (int i = 0; i < NDIM; i++) {
		element->idxLo[i]        = idxLo[i];
		element->idxHi[i]        = idxHi[i];
		element->processCoord[i] = processCoord[i];
		element->buffer          = NULL;
	}

	return element;
}

#endif
