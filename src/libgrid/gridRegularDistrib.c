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
#ifdef WITH_MPITRACE
#  include <mpitrace_user_events.h>
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegularDistrib_adt.h"


/*--- Local defines -----------------------------------------------------*/
#ifdef WITH_MPITRACE
#  define LOCAL_MPITRACE_EVENT 460000000
#endif


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

static void
local_transposeMPIInit(gridRegularDistrib_t distrib,
                       int                  dimA,
                       int                  dimB,
                       gridPatch_t          *patch,
                       gridPatch_t          *patchT,
                       varArr_t             *sendLayout,
                       varArr_t             *recvLayout);

static void
local_transposeMPIClean(varArr_t sendLayout, varArr_t recvLayout);

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

static void
local_transposeAllVarsAtPatch(gridPatch_t    patch,
                              gridPatch_t    patchT,
                              MPI_Comm       commCart,
                              const varArr_t sendLayout,
                              const varArr_t recvLayout);

static void
local_transposeGetFullSendBuffers(commScheme_t      scheme,
                                  const varArr_t    layout,
                                  const gridPatch_t patch,
                                  const gridVar_t   var,
                                  MPI_Comm          comm);

static void
local_transposeGetRecvBuffers(commScheme_t    scheme,
                              const varArr_t  layout,
                              const gridVar_t var,
                              MPI_Comm        comm);

static void
local_transposeDelSendBuffers(const varArr_t  layout,
                              const gridVar_t var);

static void
local_transposeMoveRecvBuffersToPatch(const varArr_t  layout,
                                      gridPatch_t     patchT,
                                      const int       idxOfVar,
                                      const gridVar_t var);

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
	assert(procCoords != NULL);

	localRank = gridRegularDistrib_getLocalRank(distrib);
	MPI_Cart_coords(distrib->commCart, localRank, NDIM, procCoords);
}

extern MPI_Comm
gridRegularDistrib_getGlobalComm(gridRegularDistrib_t distrib)
{
	assert(distrib != NULL);

	return distrib->commGlobal;
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

extern gridRegular_t
gridRegularDistrib_getGridHandle(const gridRegularDistrib_t distrib)
{
	assert(distrib != NULL);

	return distrib->grid;
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
	assert(nProcs > 0 && nProcs <= nCells);
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

/*
 * The idea here is:
 *   - figure out where to send stuff to and from where to receive stuff
 *   - Explode the data from the patch into send buffers
 *   - Delete the patch data (it is copied to the send buffer)
 *   - Allocate receive buffers
 *   - Perform communication
 *   - Wait for communication to completely finish
 *   - Remove send buffers
 *   - Allocate final patch data
 *   - Move the receive buffers to the final patch (deleting each receive
 *     buffer as soon as its content is copied)
 *
 *  This minimizes the amount of memory that is needed to approximately
 *  a tad more than twice the original patch data (depending on the
 *  difference in patch sizes between the tranposed and the
 *  un-transposed patch).
 */
static void
local_transposeMPI(gridRegularDistrib_t distrib,
                   int                  dimA,
                   int                  dimB)
{
	gridPatch_t patch, patchT;
	varArr_t    sendLayout;
	varArr_t    recvLayout;

	local_transposeMPIInit(distrib, dimA, dimB,
	                       &patch, &patchT, &sendLayout, &recvLayout);

	local_transposeAllVarsAtPatch(patch, patchT, distrib->commCart,
	                              sendLayout, recvLayout);
	assert(gridPatch_getNumVars(patch) == 0);

	gridRegular_replacePatch(distrib->grid, 0, patchT);

	local_transposeMPIClean(sendLayout, recvLayout);
}

static void
local_transposeMPIInit(gridRegularDistrib_t distrib,
                       int                  dimA,
                       int                  dimB,
                       gridPatch_t          *patch,
                       gridPatch_t          *patchT,
                       varArr_t             *sendLayout,
                       varArr_t             *recvLayout)
{
	int               rank;
	gridPointInt_t    pPos;
	gridPointUint32_t dims;

#  ifdef WITH_MPITRACE
	MPItrace_event(LOCAL_MPITRACE_EVENT, 11);
#  endif
	MPI_Comm_rank(distrib->commCart, &rank);
	MPI_Cart_coords(distrib->commCart, rank, NDIM, pPos);
	gridRegular_getDims(distrib->grid, dims);

	*sendLayout = local_transposeGetSendLayout(dims, distrib->nProcs, pPos,
	                                           dimA, dimB);
	*recvLayout = local_transposeGetRecvLayout(dims, distrib->nProcs, pPos,
	                                           dimA, dimB);

	*patch  = gridRegular_getPatchHandle(distrib->grid, 0);
	*patchT = local_transposeGetPatchT(dims, distrib->nProcs,
	                                   pPos, dimA, dimB);
#  ifdef WITH_MPITRACE
	MPItrace_event(LOCAL_MPITRACE_EVENT, 0);
#  endif
}

static void
local_transposeMPIClean(varArr_t sendLayout, varArr_t recvLayout)
{
	while (varArr_getLength(sendLayout) > 0)
		xfree(varArr_remove(sendLayout, 0));
	varArr_del(&sendLayout);
	while (varArr_getLength(recvLayout) > 0)
		xfree(varArr_remove(recvLayout, 0));
	varArr_del(&recvLayout);
}

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
                             int               t0,
                             int               t1)
{
	gridPointUint32_t loMine, hiMine, lo, hi, loS, hiS;
	gridPointInt_t    p;
	varArr_t          layout = varArr_new(nProcs[t0] * nProcs[t1] / 20);

	for (int i = 0; i < NDIM; i++) {
		getIdx(dims[i], nProcs[i], pPos[i], loMine + i, hiMine + i);
		loS[i] = loMine[i];
		hiS[i] = hiMine[i];
		p[i]   = pPos[i];
	}

	for (p[t1] = 0; p[t1] < nProcs[t1]; p[t1]++) {
		getIdx(dims[t0], nProcs[t1], p[t1], lo + t1, hi + t1);
		if (is(loMine[t0], hiMine[t0], lo[t1], hi[t1], loS, hiS)) {
			for (p[t0] = 0; p[t0] < nProcs[t0]; p[t0]++) {
				getIdx(dims[t1], nProcs[t0], p[t0], lo + t0, hi + t0);
				if (is(loMine[t1], hiMine[t1], lo[t0], hi[t0], loS + t1,
				       hiS + t1)) {
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
                             int               t0,
                             int               t1)
{
	gridPointUint32_t loMine, hiMine, lo, hi, loS, hiS;
	gridPointInt_t    p;
	varArr_t          layout = varArr_new(nProcs[t0] * nProcs[t1] / 20);

	for (int i = 0; i < NDIM; i++) {
		getIdx(dims[i], nProcs[i], pPos[i], loMine + i, hiMine + i);
		loS[i] = loMine[i];
		hiS[i] = hiMine[i];
		p[i]   = pPos[i];
	}

	getIdx(dims[t0], nProcs[t1], pPos[t1], loMine + t0, hiMine + t0);
	getIdx(dims[t1], nProcs[t0], pPos[t0], loMine + t1, hiMine + t1);
	for (p[t1] = 0; p[t1] < nProcs[t1]; p[t1]++) {
		getIdx(dims[t1], nProcs[t1], p[t1], lo + t1, hi + t1);
		if (is(loMine[t1], hiMine[t1], lo[t1], hi[t1], loS + t1, hiS
		       + t1)) {
			for (p[t0] = 0; p[t0] < nProcs[t0]; p[t0]++) {
				getIdx(dims[t0], nProcs[t0], p[t0], lo + t0, hi + t0);
				if (is(loMine[t0], hiMine[t0], lo[t0], hi[t0], loS, hiS)) {
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

static void
local_transposeAllVarsAtPatch(gridPatch_t    patch,
                              gridPatch_t    patchT,
                              MPI_Comm       commCart,
                              const varArr_t sendLayout,
                              const varArr_t recvLayout)
{
	int numVars = gridPatch_getNumVars(patch);

	for (int i = 0; i < numVars; i++) {
		int          idxOfVar;
		gridVar_t    varTmp;
		gridVar_t    var    = gridPatch_getVarHandle(patch, 0);
		commScheme_t scheme = commScheme_new(commCart, 4223);

		var = gridVar_getRef(var);

#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 12);
#  endif
		local_transposeGetFullSendBuffers(scheme, sendLayout, patch,
		                                  var, commCart);
#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 0);
#  endif
		varTmp = gridPatch_detachVar(patch, 0);
		gridVar_del(&varTmp);
#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 13);
#  endif
		local_transposeGetRecvBuffers(scheme, recvLayout, var, commCart);
#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 0);
#  endif

#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 14);
#  endif
		commScheme_fire(scheme);
		commScheme_wait(scheme);
#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 0);
#  endif

#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 15);
#  endif
		local_transposeDelSendBuffers(sendLayout, var);
#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 0);
#  endif
		idxOfVar = gridPatch_attachVar(patchT, var);
#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 16);
#  endif
		local_transposeMoveRecvBuffersToPatch(recvLayout, patchT,
		                                      idxOfVar, var);
#  ifdef WITH_MPITRACE
		MPItrace_event(LOCAL_MPITRACE_EVENT, 0);
#  endif

		commScheme_del(&scheme);
		gridVar_del(&var);
	}
} /* local_transposeAllVarsAtPatch */

static void
local_transposeGetFullSendBuffers(commScheme_t      scheme,
                                  const varArr_t    layout,
                                  const gridPatch_t patch,
                                  const gridVar_t   var,
                                  MPI_Comm          comm)
{
	int          len  = varArr_getLength(layout);
	MPI_Datatype type = gridVar_getMPIDatatype(var);

	for (int j = 0; j < len; j++) {
		void                  *dataSend;
		uint64_t              dataSize;
		local_layoutElement_t le = varArr_getElementHandle(layout, j);
		int                   count;
		int                   rankSend;

		// We always work on the 0th variable as the patch is
		// emptied during the course of the main loop.
		dataSend   = gridPatch_getWindowedDataCopy(patch, 0, le->idxLo,
		                                           le->idxHi, &dataSize);
		count      = gridVar_getMPICount(var, dataSize);
		MPI_Cart_rank(comm, le->processCoord, &rankSend);
		le->buffer = commSchemeBuffer_new(dataSend, count, type, rankSend);
		commScheme_addBuffer(scheme, le->buffer, COMMSCHEME_TYPE_SEND);
	}
}

static void
local_transposeGetRecvBuffers(commScheme_t    scheme,
                              const varArr_t  layout,
                              const gridVar_t var,
                              MPI_Comm        comm)
{
	int          len  = varArr_getLength(layout);
	MPI_Datatype type = gridVar_getMPIDatatype(var);

	for (int j = 0; j < len; j++) {
		local_layoutElement_t le = varArr_getElementHandle(layout, j);
		void                  *dataRecv;
		int                   rankRecv;
		int                   count;
		uint64_t              dataSize = 1;

		for (int k = 0; k < NDIM; k++) {
			dataSize *= (le->idxHi[k] - le->idxLo[k] + 1);
		}
		dataRecv   = gridVar_getMemory(var, dataSize);
		count      = gridVar_getMPICount(var, dataSize);
		MPI_Cart_rank(comm, le->processCoord, &rankRecv);
		le->buffer = commSchemeBuffer_new(dataRecv, count, type, rankRecv);
		commScheme_addBuffer(scheme, le->buffer, COMMSCHEME_TYPE_RECV);
	}
}

static void
local_transposeDelSendBuffers(const varArr_t  layout,
                              const gridVar_t var)
{
	for (int j = 0; j < varArr_getLength(layout); j++) {
		local_layoutElement_t le = varArr_getElementHandle(layout, j);
		gridVar_freeMemory(var, commSchemeBuffer_getBuf(le->buffer));
	}
}

static void
local_transposeMoveRecvBuffersToPatch(const varArr_t  layout,
                                      gridPatch_t     patchT,
                                      const int       idxOfVar,
                                      const gridVar_t var)
{
	for (int j = 0; j < varArr_getLength(layout); j++) {
		local_layoutElement_t le = varArr_getElementHandle(layout, j);
		void                  *dataRecv;

		dataRecv = commSchemeBuffer_getBuf(le->buffer);
		gridPatch_putWindowedData(patchT, idxOfVar, le->idxLo,
		                          le->idxHi, dataRecv);
		gridVar_freeMemory(var, dataRecv);
	}
}

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

#endif  // WITH_MPI
