/*--- Includes ----------------------------------------------------------*/
#include "showFreqs.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/bov.h"
#include "../../src/libutil/xfile.h"


/*--- Prototypes of local functions -------------------------------------*/
static void
local_getComplexDims(int       nDims,
                     const int *dims,
                     int       **dimsComplex,
                     uint64_t  *numTotal);

static void
local_fillFreqs(double *freqs, const int nDims, const int *dimsComplex);

static void
local_dumpFreqs(double     *freqs,
                const int  nDims,
                const int  *dimsComplex,
                const char *outFileName);

static void
local_dumpFreqsBov(const int  nDims,
                   const int  *dims,
                   const char *bovName,
                   const char *datName);


/*--- Implementations of exported functios ------------------------------*/
extern void
showFreqs(int nDims, const int *dims, const char *outFileName)
{
	int      *dimsComplex;
	double   *freqs;
	uint64_t numTotal;

	local_getComplexDims(nDims, dims, &dimsComplex, &numTotal);

	freqs = xmalloc(sizeof(double) * numTotal);
	local_fillFreqs(freqs, nDims, dimsComplex);
	local_dumpFreqs(freqs, nDims, dimsComplex, outFileName);

	xfree(freqs);
	xfree(dimsComplex);
}

/*--- Implementations of local functions --------------------------------*/
static void
local_getComplexDims(int       nDims,
                     const int *dims,
                     int       **dimsComplex,
                     uint64_t  *numTotal)
{
	*dimsComplex      = xmalloc(sizeof(int) * nDims);
	(*dimsComplex)[0] = dims[0] / 2 + 1;
	*numTotal         = (*dimsComplex)[0];
	for (int i = 1; i < nDims; i++) {
		(*dimsComplex)[i] = dims[i];
		*numTotal        *= (*dimsComplex)[i];
	}

	printf("Running with %i dimensions: %i", nDims, dims[0]);
	for (int i = 1; i < nDims; i++)
		printf(" x %i", dims[i]);
	printf("\nComplexified this is:      %i", (*dimsComplex)[0]);
	for (int i = 1; i < nDims; i++)
		printf(" x %i", (*dimsComplex)[i]);
	printf(" (total: %" PRIu64 ")\n\n", *numTotal);
}

static void
local_fillFreqs(double *freqs, const int nDims, const int *dimsComplex)
{
	if (nDims == 1) {
		for (int i = 0; i < dimsComplex[0]; i++)
			freqs[i] = (double)i;
	} else if (nDims == 2) {
		for (int j = 0; j < dimsComplex[1]; j++) {
			int k1 = (j > dimsComplex[1] / 2) ? j - dimsComplex[1] : j;
			for (int i = 0; i < dimsComplex[0]; i++) {
				int k0 = i;
				freqs[i + j * dimsComplex[0]] = sqrt((double)k0 * k0
				                                     + (double)k1 * k1);
			}
		}
	} else if (nDims == 3) {
		for (int k = 0; k < dimsComplex[2]; k++) {
			int k2 = (k > dimsComplex[2] / 2) ? k - dimsComplex[2] : k;
			for (int j = 0; j < dimsComplex[1]; j++) {
				int k1 = (j > dimsComplex[1] / 2) ? j - dimsComplex[1] : j;
				for (int i = 0; i < dimsComplex[0]; i++) {
					int    k0  = i;
					size_t pos = i + (j + k * dimsComplex[1])
					             * dimsComplex[0];
					freqs[pos] = sqrt((double)k0 * k0
					                  + (double)k1 * k1
					                  + (double)k2 * k2);
				}
			}
		}
	} else
		exit(EXIT_FAILURE);
}

static void
local_dumpFreqs(double     *freqs,
                const int  nDims,
                const int  *dimsComplex,
                const char *outFileName)
{
	char   *datName, *bovName;
	FILE   *f;
	size_t totsize;

	totsize = dimsComplex[0];
	for (int i = 1; i < nDims; i++) {
		totsize *= dimsComplex[i];
	}

	datName = xstrmerge(outFileName, ".dat");
	bovName = xstrmerge(outFileName, ".bov");
	f       = xfopen(datName, "wb");
	xfwrite(freqs, sizeof(double), totsize, f);
	xfclose(&f);

	local_dumpFreqsBov(nDims, dimsComplex, bovName, datName);

	xfree(bovName);
	xfree(datName);
}

static void
local_dumpFreqsBov(const int  nDims,
                   const int  *dims,
                   const char *bovName,
                   const char *datName)
{
	bov_t    bov;
	uint32_t dataSize[3];
	double   brickSize[3];

	bov = bov_new();
	bov_setDataFileName(bov, datName);

	for (int i = 0; i < 3; i++) {
		dataSize[i]  = (i < nDims) ? (uint32_t)(dims[i]) : 1;
		brickSize[i] = (double)(dataSize[i]);
	}

	bov_setDataSize(bov, dataSize);
	bov_setDataFormat(bov, BOV_FORMAT_DOUBLE);
	bov_setVarName(bov, "frequency");
	bov_setCentering(bov, BOV_CENTERING_ZONAL);
	bov_setBrickSize(bov, brickSize);

	bov_write(bov, bovName);

	bov_del(&bov);
}
