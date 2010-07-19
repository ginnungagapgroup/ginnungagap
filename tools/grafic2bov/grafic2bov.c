// Copyright (C) 2010, Steffen Knollmann
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


/*--- Implemention of main structure ------------------------------------*/
#include "grafic2bov_adt.h"


/*--- Prototypes of local functions -------------------------------------*/
static void
local_checkOutputName(const char *outName, bool force);

static void
local_doHeader(FILE *f, int np[3], float *dx, float *astart);


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
	FILE  *f;
	int   np[3];
	float dx;
	float astart;
	float *data;

	assert(g2b != NULL);

	f = xfopen(g2b->graficFileName, "rb");

	local_doHeader(f, np, &dx, &astart);
	data = xmalloc(sizeof(float) * np[0]*np[1]*np[2]);
	for (int i=0; i<np[2]; i++) {
		int   bSize1, bSize2;
		xfread(&bSize1, sizeof(int), 1, f);
		xfread(data+i*np[0]*np[1], sizeof(float), np[0]*np[1], f);
		xfread(&bSize2, sizeof(int), 1, f);
		if (bSize1 != bSize2)
			diediedie(EXIT_FAILURE);
	}
	xfclose(&f);

	f = xfopen(g2b->bovDataFileName, "wb");
	xfwrite(data, sizeof(float), np[0]*np[1]*np[2], f);
	xfclose(&f);

	xfree(data);

	f = xfopen(g2b->bovFileName, "w");
	fprintf(f, "TIME: %f\n", 1./astart - 1.);
	char *tmp = xbasename(g2b->graficFileName);
	fprintf(f, "VARIABLE: %s\n", tmp);
	xfree(tmp);
	fprintf(f, "BRICK_ORIGIN: 0. 0. 0.\n");
	fprintf(f, "BRICK_SIZE: %f %f %f\n", np[0]*dx, np[1]*dx, np[2]*dx);
	fprintf(f, "DATA_FILE: %s\n", g2b->bovDataFileName);
	fprintf(f, "DATA_SIZE: %i %i %i\n", np[0], np[1], np[2]);
	fprintf(f, "DATA_ENDIAN: LITTLE\n");
	fprintf(f, "DATA_FORMAT: FLOAT\n");
	fprintf(f, "DATA_COMPONENTS: 1\n");
	fprintf(f, "CENTERING: zonal\n");
	xfclose(&f);
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

static void
local_doHeader(FILE *f, int np[3], float *dx, float *astart)
{
	int   bSize1, bSize2;
	float xoff[3];
	float omegam;
	float omegav;
	float h0;

	xfread(&bSize1, sizeof(int), 1, f);
	xfread(np, sizeof(int), 3, f);
	if (bSize1 > 16) {
		xfread(dx, sizeof(float), 1, f);
		xfread(xoff, sizeof(float), 3, f);
		xfread(astart, sizeof(float), 1, f);
		xfread(&omegam, sizeof(float), 1, f);
		xfread(&omegav, sizeof(float), 1, f);
		xfread(&h0, sizeof(float), 1, f);
	} else {
		int iseed;
		xfread(&iseed, sizeof(int), 1, f);
		*dx = 1.0;
		h0 = 1.0;
		*astart = 1.0;
	}
	xfread(&bSize2, sizeof(int), 1, f);
	if (bSize1 != bSize2)
		diediedie(EXIT_FAILURE);

	*dx *= .01f * h0;

	fprintf(stdout,
	        "np     =  (%i, %i, %i)\n"
	        "dx     =  %f\nxoff   =  (%f, %f, %f)\n"
	        "astart =  %f\nomegam =  %f\n"
	        "omegav =  %f\nh0     =  %f\n",
	        np[0], np[1], np[2], *dx, xoff[0], xoff[1], xoff[2],
	        *astart, omegam, omegav, h0);
}
