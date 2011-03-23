// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "grafic2bovConfig.h"
#include "grafic2bov.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/xfile.h"
#include "../../src/libutil/diediedie.h"
#include "../../src/libutil/grafic.h"
#include "../../src/libutil/bov.h"


/*--- Implemention of main structure ------------------------------------*/
#include "grafic2bov_adt.h"


/*--- Prototypes of local functions -------------------------------------*/
static void
local_checkOutputName(const char *outName, bool force);

static float *
local_getDataAndMeta(const char *fname,
                     uint32_t   *np,
                     float      *dx,
                     float      *astart);

static void
local_writeData(const char *fname, const float *data, size_t numElements);

static void
local_writeBov(const char *bovFileName,
               const char *dataFileName,
               const char *varName,
               uint32_t   *np,
               float      astart,
               float      dx);


/*--- Implementations of exported functios ------------------------------*/
extern grafic2bov_t
grafic2bov_new(const char *graficFileName,
               const char *outputFileStem,
               bool       force)
{
	grafic2bov_t g2b;

	assert(graficFileName != NULL);
	assert(outputFileStem != NULL);

	g2b                  = xmalloc(sizeof(struct grafic2bov_struct));
	g2b->graficFileName  = xstrdup(graficFileName);
	g2b->bovFileName     = xstrmerge(outputFileStem, ".bov");
	g2b->bovDataFileName = xstrmerge(outputFileStem, ".dat");

	local_checkOutputName(g2b->bovFileName, force);
	local_checkOutputName(g2b->bovDataFileName, force);

	return g2b;
}

extern void
grafic2bov_del(grafic2bov_t *g2b)
{
	assert(g2b != NULL);
	assert(*g2b != NULL);

	xfree((*g2b)->graficFileName);
	xfree((*g2b)->bovFileName);
	xfree((*g2b)->bovDataFileName);
	xfree(*g2b);
	*g2b = NULL;
}

extern void
grafic2bov_run(grafic2bov_t g2b)
{
	uint32_t np[3];
	float    dx;
	float    astart;
	float    *data;
	char     *varName;

	assert(g2b != NULL);

	varName = xbasename(g2b->graficFileName);
	data    = local_getDataAndMeta(g2b->graficFileName, np, &dx, &astart);

	local_writeData(g2b->bovDataFileName, data, np[0] * np[1] * np[2]);
	local_writeBov(g2b->bovFileName, g2b->bovDataFileName, varName, np,
	               astart, dx);

	xfree(varName);
	xfree(data);
}

/*--- Implementations of local functions --------------------------------*/
static void
local_checkOutputName(const char *outName, bool force)
{
	FILE *f;

	f = fopen(outName, "r");

	if (f != NULL) {
		if (!force) {
			fprintf(stderr,
			        "Output file %s already exists.  "
			        "Use --force to overwrite.\n",
			        outName);
			exit(EXIT_FAILURE);
		}
		fclose(f);
		fprintf(stdout,
		        "Warning:  Output file %s already will be overwritten.\n",
		        outName);
	}
}

static float *
local_getDataAndMeta(const char *fname,
                     uint32_t   *np,
                     float      *dx,
                     float      *astart)
{
	float    *data;
	grafic_t grafic;

	grafic = grafic_newFromFile(fname);
	grafic_getSize(grafic, np);
	if (grafic_isWhiteNoise(grafic)) {
		*dx     = 1.0;
		*astart = 1.0;
	} else {
		*dx     = grafic_getDx(grafic);
		*astart = grafic_getAstart(grafic);
	}

	data = xmalloc(sizeof(float) * np[0] * np[1] * np[2]);
	grafic_read(grafic, data, GRAFIC_FORMAT_FLOAT, 1);
	grafic_del(&grafic);

	return data;
}

static void
local_writeData(const char *fname, const float *data, size_t numElements)
{
	FILE *f;

	f = xfopen(fname, "wb");
	xfwrite(data, sizeof(float), numElements, f);
	xfclose(&f);
}

static void
local_writeBov(const char *bovFileName,
               const char *dataFileName,
               const char *varName,
               uint32_t   *np,
               float      astart,
               float      dx)
{
	bov_t  bov;
	double brickSize[3];

	bov = bov_new();

	bov_setTime(bov, astart);
	bov_setDataFileName(bov, dataFileName);
	bov_setDataSize(bov, np);
	bov_setDataFormat(bov, BOV_FORMAT_FLOAT);
	bov_setVarName(bov, varName);
	for (int i = 0; i < 3; i++)
		brickSize[i] = np[0] * dx;
	bov_setBrickSize(bov, brickSize);

	bov_write(bov, bovFileName);

	bov_del(&bov);
}
