// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "grafic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "endian.h"
#include "xmem.h"
#include "xstring.h"
#include "xfile.h"
#include "diediedie.h"
#include "byteswap.h"


/*--- Implemention of main structure ------------------------------------*/
#include "grafic_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static bool
local_checkIfFileIsWhiteNoise(FILE *f);

static void
local_readGrafic(grafic_t grafic, FILE *f);

static void
local_swapHeaderValues(grafic_t grafic);

static void
local_readPlane(float *data, size_t numInPlane, FILE *f, bool doByteswap);

static void *
local_readPlaneBuffered(void           *data,
                        graficFormat_t dataFormat,
                        int            numComponents,
                        float          *buffer,
                        size_t         numInPlane,
                        FILE           *f,
                        bool           doByteswap);

static void
local_readWindowedActualRead(grafic_t       grafic,
                             void           *data,
                             graficFormat_t dataFormat,
                             int            numComponents,
                             uint32_t       *idxLo,
                             uint32_t       *dims,
                             bool           doByteswap);

static void
local_writeWindowedActualRead(grafic_t       grafic,
                              void           *data,
                              graficFormat_t dataFormat,
                              int            numComponents,
                              uint32_t       *idxLo,
                              uint32_t       *dims,
                              bool           doByteswap);

static void
local_cpBufferToData(float          *buffer,
                     uint32_t       num,
                     void           *data,
                     graficFormat_t format,
                     int            numComponents,
                     size_t         dataOffset,
                     bool           doByteswap);

static void
local_cpDataToBuffer(float          *buffer,
                     uint32_t       num,
                     void           *data,
                     graficFormat_t format,
                     int            numComponents,
                     size_t         dataOffset,
                     bool           doByteswap);

static void
local_skipPlane(FILE *f, long numInPlane);

static void
local_skipRow(FILE *f, long numInRow);

static void
local_writeHeader(grafic_t grafic, FILE *f);

static void
local_writePlane(FILE           *f,
                 void           *data,
                 graficFormat_t format,
                 int            numComponents,
                 size_t         dataOffset,
                 int            numPlane);


/*--- Implementations of exported functios ------------------------------*/
extern grafic_t
grafic_new(bool isWhiteNoise)
{
	grafic_t grafic;

	grafic                   = xmalloc(sizeof(struct grafic_struct));
	grafic->graficFileName   = NULL;
	grafic->machineEndianess = endian_getSystemEndianess();
	grafic->fileEndianess    = grafic->machineEndianess;
	grafic->isWhiteNoise     = isWhiteNoise;
	grafic->headerSkip       = isWhiteNoise ? 16 : 44;
	grafic->np1              = 0;
	grafic->np2              = 0;
	grafic->np3              = 0;
	grafic->dx               = 0.0f;
	grafic->x1o              = 0.0f;
	grafic->x2o              = 0.0f;
	grafic->x3o              = 0.0f;
	grafic->astart           = 0.0f;
	grafic->omegam           = 0.0f;
	grafic->omegav           = 0.0f;
	grafic->h0               = 0.0f;
	grafic->iseed            = 0;

	return grafic;
}

extern grafic_t
grafic_newFromFile(const char *fileName)
{
	grafic_t grafic;
	FILE     *f;
	bool     isWhiteNoise;

	assert(fileName != NULL);

	f                      = xfopen(fileName, "r");
	isWhiteNoise           = local_checkIfFileIsWhiteNoise(f);
	grafic                 = grafic_new(isWhiteNoise);
	grafic->graficFileName = xstrdup(fileName);
	grafic->fileEndianess  = endian_getFileEndianessByBlock(fileName);
	local_readGrafic(grafic, f);

	xfclose(&f);

	return grafic;
}

extern void
grafic_del(grafic_t *grafic)
{
	assert(grafic != NULL && *grafic != NULL);

	if ((*grafic)->graficFileName != NULL)
		xfree((*grafic)->graficFileName);
	xfree(*grafic);

	*grafic = NULL;
}

extern const char *
grafic_getFileName(grafic_t grafic)
{
	assert(grafic != NULL);

	return grafic->graficFileName;
}

extern void
grafic_getSize(grafic_t grafic, uint32_t *np)
{
	assert(grafic != NULL);
	assert(np != NULL);

	np[0] = grafic->np1;
	np[1] = grafic->np2;
	np[2] = grafic->np3;
}

extern float
grafic_getDx(grafic_t grafic)
{
	assert(grafic != NULL);
	assert(!grafic->isWhiteNoise);

	return grafic->dx;
}

extern void
grafic_getXoff(grafic_t grafic, float *xoff)
{
	assert(grafic != NULL);
	assert(!grafic->isWhiteNoise);
	assert(xoff != NULL);

	xoff[0] = grafic->x1o;
	xoff[1] = grafic->x2o;
	xoff[2] = grafic->x3o;
}

extern float
grafic_getAstart(grafic_t grafic)
{
	assert(grafic != NULL);
	assert(!grafic->isWhiteNoise);

	return grafic->astart;
}

extern float
grafic_getOmegam(grafic_t grafic)
{
	assert(grafic != NULL);
	assert(!grafic->isWhiteNoise);

	return grafic->omegam;
}

extern float
grafic_getOmegav(grafic_t grafic)
{
	assert(grafic != NULL);
	assert(!grafic->isWhiteNoise);

	return grafic->omegav;
}

extern float
grafic_getH0(grafic_t grafic)
{
	assert(grafic != NULL);
	assert(!grafic->isWhiteNoise);

	return grafic->h0;
}

extern int
grafic_getIseed(grafic_t grafic)
{
	assert(grafic != NULL);
	assert(grafic->isWhiteNoise);

	return grafic->iseed;
}

extern void
grafic_setFileName(grafic_t grafic, const char *fileName)
{
	assert(grafic != NULL);
	assert(fileName != NULL);

	if (grafic->graficFileName != NULL)
		xfree(grafic->graficFileName);

	grafic->graficFileName = xstrdup(fileName);
}

extern void
grafic_setSize(grafic_t grafic, uint32_t *np)
{
	assert(grafic != NULL);
	assert(np != NULL);
	assert(np[0] > 0);
	assert(np[1] > 0);
	assert(np[2] > 0);

	grafic->np1 = np[0];
	grafic->np2 = np[1];
	grafic->np3 = np[2];
}

extern void
grafic_setDx(grafic_t grafic, float dx)
{
	assert(grafic != NULL);
	assert(isgreater(dx, 0.0));
	assert(!grafic->isWhiteNoise);

	grafic->dx = dx;
}

extern void
grafic_setXoff(grafic_t grafic, float *xoff)
{
	assert(grafic != NULL);
	assert(xoff != NULL);
	assert(!grafic->isWhiteNoise);

	grafic->x1o = xoff[0];
	grafic->x2o = xoff[1];
	grafic->x3o = xoff[2];
}

extern void
grafic_setAstart(grafic_t grafic, float astart)
{
	assert(grafic != NULL);
	assert(isgreater(astart, 0.0));
	assert(isless(astart, 10.0));
	assert(!grafic->isWhiteNoise);

	grafic->astart = astart;
}

extern void
grafic_setOmegam(grafic_t grafic, float omegam)
{
	assert(grafic != NULL);
	assert(isgreaterequal(omegam, 0.0));
	assert(isless(omegam, 1.5));
	assert(!grafic->isWhiteNoise);

	grafic->omegam = omegam;
}

extern void
grafic_setOmegav(grafic_t grafic, float omegav)
{
	assert(grafic != NULL);
	assert(isgreaterequal(omegav, 0.0));
	assert(isless(omegav, 1.5));
	assert(!grafic->isWhiteNoise);

	grafic->omegav = omegav;
}

extern void
grafic_setH0(grafic_t grafic, float h0)
{
	assert(grafic != NULL);
	assert(isgreater(h0, 0.0));
	assert(isless(h0, 1000.0));
	assert(!grafic->isWhiteNoise);

	grafic->h0 = h0;
}

extern void
grafic_setIseed(grafic_t grafic, int iseed)
{
	assert(grafic != NULL);
	assert(grafic->isWhiteNoise);

	grafic->iseed = iseed;
}

extern bool
grafic_isWhiteNoise(grafic_t grafic)
{
	assert(grafic != NULL);
	return grafic->isWhiteNoise;
}

extern void
grafic_makeEmptyFile(grafic_t grafic)
{
	FILE   *f;
	size_t fileSize = 0;
	long   numInPlane;
	int    b;

	assert(grafic != NULL);
	assert(grafic->graficFileName != NULL);
	assert(grafic->np1 > 0);
	assert(grafic->np2 > 0);
	assert(grafic->np3 > 0);

	numInPlane = grafic->np1 * grafic->np2;
	fileSize   = (numInPlane + 8) * grafic->np3;
	fileSize  += grafic->headerSkip + 8;
	xfile_createFileWithSize(grafic->graficFileName, fileSize);

	f = xfopen(grafic->graficFileName, "w+b");
	local_writeHeader(grafic, f);
	b = (int)(numInPlane * sizeof(float));
	if (grafic->fileEndianess != grafic->machineEndianess)
		byteswap(&b, sizeof(int));
	for (int i = 0; i < grafic->np3; i++) {
		xfwrite(&b, sizeof(int), 1, f);
		xfseek(f, numInPlane * sizeof(float), SEEK_CUR);
		xfwrite(&b, sizeof(int), 1, f);
	}
	xfclose(&f);
}

extern void
grafic_read(grafic_t       grafic,
            void           *data,
            graficFormat_t dataFormat,
            int            numComponents)
{
	FILE   *f;
	size_t numInPlane;
	bool   doByteswap;

	assert(grafic != NULL);
	assert(data != NULL);
	assert(numComponents > 0);

	numInPlane = grafic->np1 * grafic->np2;
	doByteswap = grafic->machineEndianess != grafic->fileEndianess;
	f          = xfopen(grafic->graficFileName, "rb");
	xfseek(f, grafic->headerSkip + 8L, SEEK_SET);
	if ((dataFormat == GRAFIC_FORMAT_FLOAT)
	    && (numComponents == 1)) {
		for (uint32_t i = 0; i < grafic->np3; i++) {
			local_readPlane(((float *)data) + i * numInPlane, numInPlane,
			                f, doByteswap);
		}
	} else {
		float *buffer = xmalloc(sizeof(float) * numInPlane);
		for (uint32_t i = 0; i < grafic->np3; i++) {
			data = local_readPlaneBuffered(data, dataFormat,
			                               numComponents, buffer,
			                               numInPlane, f, doByteswap);
		}
		xfree(buffer);
	}
	xfclose(&f);
}

extern void
grafic_write(grafic_t       grafic,
             void           *data,
             graficFormat_t dataFormat,
             int            numComponents)
{
	FILE   *f;
	int    numPlane;
	size_t dataOffset = 0;

	assert(grafic != NULL);
	assert(data != NULL);
	assert(numComponents > 0);
	assert(grafic->graficFileName != NULL);

	numPlane = grafic->np1 * grafic->np2;

	f        = xfopen(grafic->graficFileName, "wb");

	local_writeHeader(grafic, f);

	for (uint32_t k = 0; k < grafic->np3; k++) {
		int b = numPlane * sizeof(float);
		xfwrite(&b, sizeof(int), 1, f);
		local_writePlane(f, data, dataFormat, numComponents, dataOffset,
		                 numPlane);
		dataOffset += numPlane;
		xfwrite(&b, sizeof(int), 1, f);
	}

	xfclose(&f);
}

extern void
grafic_readWindowed(grafic_t       grafic,
                    void           *data,
                    graficFormat_t dataFormat,
                    int            numComponents,
                    uint32_t       *idxLo,
                    uint32_t       *dims)
{
	bool doByteswap;

	assert(grafic != NULL);
	assert(data != NULL);
	assert(numComponents > 0);
	assert(idxLo != NULL);
	assert(dims != NULL);
	assert(dims[0] > 0 && dims[1] > 0 && dims[2] > 0);

	if ((idxLo[0] + dims[0] > grafic->np1)
	    || (idxLo[1] + dims[1] > grafic->np2)
	    || (idxLo[2] + dims[2] > grafic->np3)) {
		fprintf(stderr, "Window too large for data in file :(\n");
		diediedie(EXIT_FAILURE);
	}

	doByteswap = grafic->machineEndianess != grafic->fileEndianess;

	local_readWindowedActualRead(grafic, data, dataFormat, numComponents,
	                             idxLo, dims, doByteswap);
}

extern void
grafic_writeWindowed(grafic_t       grafic,
                     void           *data,
                     graficFormat_t dataFormat,
                     int            numComponents,
                     uint32_t       *idxLo,
                     uint32_t       *dims)
{
	bool doByteswap;

	assert(grafic != NULL);
	assert(data != NULL);
	assert(numComponents > 0);
	assert(idxLo != NULL);
	assert(dims != NULL);
	assert(dims[0] > 0 && dims[1] > 0 && dims[2] > 0);

	if ((idxLo[0] + dims[0] > grafic->np1)
	    || (idxLo[1] + dims[1] > grafic->np2)
	    || (idxLo[2] + dims[2] > grafic->np3)) {
		fprintf(stderr, "Window too large for file :(\n");
		diediedie(EXIT_FAILURE);
	}

	doByteswap = grafic->machineEndianess != grafic->fileEndianess;

	local_writeWindowedActualRead(grafic, data, dataFormat, numComponents,
	                              idxLo, dims, doByteswap);
}

extern void
grafic_readSlab(grafic_t       grafic,
                void           *data,
                graficFormat_t dataFormat,
                int            numComponents,
                int            slabNum)
{
	FILE   *f;
	size_t numInPlane;
	bool   doByteswap;

	assert(grafic != NULL);
	assert(data != NULL);
	assert(numComponents > 0);
	assert(slabNum >= 0 && slabNum < grafic->np3);

	numInPlane = grafic->np1 * grafic->np2;
	doByteswap = grafic->machineEndianess != grafic->fileEndianess;
	f          = xfopen(grafic->graficFileName, "rb");
	xfseek(f, grafic->headerSkip + 8L, SEEK_SET);
	xfseek(f, (numInPlane * sizeof(float) + 8L) * slabNum, SEEK_CUR);
	if ((dataFormat == GRAFIC_FORMAT_FLOAT)
	    && (numComponents == 1)) {
		local_readPlane(((float *)data), numInPlane, f, doByteswap);
	} else {
		float *buffer = xmalloc(sizeof(float) * numInPlane);
		data = local_readPlaneBuffered(data, dataFormat,
		                               numComponents, buffer,
		                               numInPlane, f, doByteswap);
		xfree(buffer);
	}
	xfclose(&f);
}

/*--- Implementations of local functions --------------------------------*/
static bool
local_checkIfFileIsWhiteNoise(FILE *f)
{
	int  b1;
	int  b1Swapped;
	long oldPosition;
	bool isWhiteNoise;

	oldPosition = ftell(f);

	rewind(f);
	xfread(&b1, sizeof(int), 1, f);
	fseek(f, oldPosition, SEEK_SET);

	b1Swapped = b1;
	byteswap(&b1Swapped, sizeof(int));

	if ((b1 == 16) || (b1Swapped == 16))
		isWhiteNoise = true;
	else
		isWhiteNoise = false;

	return isWhiteNoise ? true : false;
}

static void
local_readGrafic(grafic_t grafic, FILE *f)
{
	int b1, b2;

	xfread(&b1, sizeof(int), 1, f);

	xfread(&(grafic->np1), sizeof(uint32_t), 1, f);
	xfread(&(grafic->np2), sizeof(uint32_t), 1, f);
	xfread(&(grafic->np3), sizeof(uint32_t), 1, f);
	if (!grafic->isWhiteNoise) {
		xfread(&(grafic->dx), sizeof(float), 1, f);
		xfread(&(grafic->x1o), sizeof(float), 1, f);
		xfread(&(grafic->x2o), sizeof(float), 1, f);
		xfread(&(grafic->x3o), sizeof(float), 1, f);
		xfread(&(grafic->astart), sizeof(float), 1, f);
		xfread(&(grafic->omegam), sizeof(float), 1, f);
		xfread(&(grafic->omegav), sizeof(float), 1, f);
		xfread(&(grafic->h0), sizeof(float), 1, f);
	} else
		xfread(&(grafic->iseed), sizeof(int), 1, f);

	xfread(&b2, sizeof(int), 1, f);
	if (b1 != b2)
		diediedie(EXIT_FAILURE);

	if (grafic->fileEndianess != grafic->machineEndianess)
		local_swapHeaderValues(grafic);
}

static void
local_swapHeaderValues(grafic_t grafic)
{
	byteswap(&(grafic->np1), sizeof(uint32_t));
	byteswap(&(grafic->np2), sizeof(uint32_t));
	byteswap(&(grafic->np3), sizeof(uint32_t));
	if (!grafic->isWhiteNoise) {
		byteswap(&(grafic->dx), sizeof(float));
		byteswap(&(grafic->x1o), sizeof(float));
		byteswap(&(grafic->x2o), sizeof(float));
		byteswap(&(grafic->x3o), sizeof(float));
		byteswap(&(grafic->astart), sizeof(float));
		byteswap(&(grafic->omegam), sizeof(float));
		byteswap(&(grafic->omegav), sizeof(float));
		byteswap(&(grafic->h0), sizeof(float));
	} else
		byteswap(&(grafic->iseed), sizeof(int));
}

static void
local_readPlane(float *data, size_t numInPlane, FILE *f, bool doByteswap)
{
	int b1, b2;

	xfread(&b1, sizeof(int), 1, f);

	xfread(data, sizeof(float), numInPlane, f);

	xfread(&b2, sizeof(int), 1, f);
	if (b1 != b2)
		diediedie(EXIT_FAILURE);

	if (doByteswap)
		for (size_t i = 0; i < numInPlane; i++)
			byteswap(data + i, sizeof(float));
}

static void *
local_readPlaneBuffered(void           *data,
                        graficFormat_t dataFormat,
                        int            numComponents,
                        float          *buffer,
                        size_t         numInPlane,
                        FILE           *f,
                        bool           doByteswap)
{
	void   *rtn;
	size_t i;

	local_readPlane(buffer, numInPlane, f, doByteswap);

	if (dataFormat == GRAFIC_FORMAT_FLOAT) {
		for (i = 0; i < numInPlane; i++)
			((float *)data)[i * numComponents] = buffer[i];
		rtn = ((float *)data) + i * numComponents;
	} else {
		for (i = 0; i < numInPlane; i++)
			((double *)data)[i * numComponents] = (double)(buffer[i]);
		rtn = ((double *)data) + i * numComponents;
	}

	return rtn;
}

static void
local_readWindowedActualRead(grafic_t       grafic,
                             void           *data,
                             graficFormat_t dataFormat,
                             int            numComponents,
                             uint32_t       *idxLo,
                             uint32_t       *dims,
                             bool           doByteswap)
{
	FILE   *f;
	float  *buffer    = xmalloc(sizeof(float) * dims[0]);
	size_t dataOffset = 0;

	f = xfopen(grafic->graficFileName, "rb");
	xfseek(f, grafic->headerSkip + 8L, SEEK_SET);
	for (uint32_t k = 0; k < idxLo[2]; k++)
		local_skipPlane(f, grafic->np1 * grafic->np2);
	for (uint32_t k = 0; k < dims[2]; k++) {
		int b1, b2;
		xfread(&b1, sizeof(int), 1, f);
		for (uint32_t j = 0; j < idxLo[1]; j++)
			local_skipRow(f, grafic->np1);
		for (uint32_t j = 0; j < dims[1]; j++) {
			if (idxLo[0] > 0)
				xfseek(f, sizeof(float) * idxLo[0], SEEK_CUR);
			xfread(buffer, sizeof(float), dims[0], f);
			if (grafic->np1 - dims[0] - idxLo[0] > 0)
				xfseek(f, sizeof(float) * (grafic->np1 - dims[0] - idxLo[0]),
				       SEEK_CUR);
			local_cpBufferToData(buffer, dims[0], data, dataFormat,
			                     numComponents, dataOffset, doByteswap);
			dataOffset += dims[0];
		}
		for (uint32_t j = 0; j < grafic->np2 - dims[1] - idxLo[1]; j++)
			local_skipRow(f, grafic->np1);
		xfread(&b2, sizeof(int), 1, f);
		if (b1 != b2)
			diediedie(EXIT_FAILURE);
	}

	xfclose(&f);
	xfree(buffer);
} /* local_readWindowedActualRead */

static void
local_writeWindowedActualRead(grafic_t       grafic,
                              void           *data,
                              graficFormat_t dataFormat,
                              int            numComponents,
                              uint32_t       *idxLo,
                              uint32_t       *dims,
                              bool           doByteswap)
{
	FILE   *f;
	float  *buffer    = xmalloc(sizeof(float) * dims[0]);
	size_t dataOffset = 0;

	f = xfopen(grafic->graficFileName, "r+b");
	xfseek(f, grafic->headerSkip + 8L, SEEK_SET);
	for (uint32_t k = 0; k < idxLo[2]; k++)
		local_skipPlane(f, grafic->np1 * grafic->np2);
	for (uint32_t k = 0; k < dims[2]; k++) {
		int b1, b2;
		xfread(&b1, sizeof(int), 1, f);
		for (uint32_t j = 0; j < idxLo[1]; j++)
			xfseek(f, grafic->np1 * sizeof(float), SEEK_CUR);
		for (uint32_t j = 0; j < dims[1]; j++) {
			if (idxLo[0] > 0)
				xfseek(f, sizeof(float) * idxLo[0], SEEK_CUR);
			local_cpDataToBuffer(buffer, dims[0], data, dataFormat,
			                     numComponents, dataOffset, doByteswap);
			dataOffset += dims[0];
			xfwrite(buffer, sizeof(float), dims[0], f);
			if (grafic->np1 - dims[0] - idxLo[0] > 0)
				xfseek(f, sizeof(float) * (grafic->np1 - dims[0] - idxLo[0]),
				       SEEK_CUR);
		}
		for (uint32_t j = 0; j < grafic->np2 - dims[1] - idxLo[1]; j++)
			xfseek(f, grafic->np1 * sizeof(float), SEEK_CUR);
		xfread(&b2, sizeof(int), 1, f);
		if (b1 != b2)
			diediedie(EXIT_FAILURE);
	}

	xfclose(&f);
	xfree(buffer);
} /* local_writeWindowedActualRead */

static void
local_cpBufferToData(float          *buffer,
                     uint32_t       num,
                     void           *data,
                     graficFormat_t format,
                     int            numComponents,
                     size_t         dataOffset,
                     bool           doByteswap)
{
	if (doByteswap) {
		for (uint32_t i = 0; i < num; i++)
			byteswap(buffer + i, sizeof(float));
	}

	if (format == GRAFIC_FORMAT_FLOAT) {
		for (uint32_t i = 0; i < num; i++) {
			size_t idx = (dataOffset + i) * numComponents;
			((float *)data)[idx] = buffer[i];
		}
	} else {
		for (uint32_t i = 0; i < num; i++) {
			size_t idx = (dataOffset + i) * numComponents;
			((double *)data)[idx] = (double)(buffer[i]);
		}
	}
}

static void
local_cpDataToBuffer(float          *buffer,
                     uint32_t       num,
                     void           *data,
                     graficFormat_t format,
                     int            numComponents,
                     size_t         dataOffset,
                     bool           doByteswap)
{
	if (format == GRAFIC_FORMAT_FLOAT) {
		for (uint32_t i = 0; i < num; i++) {
			size_t idx = (dataOffset + i) * numComponents;
			buffer[i] = ((float *)data)[idx];
		}
	} else {
		for (uint32_t i = 0; i < num; i++) {
			size_t idx = (dataOffset + i) * numComponents;
			buffer[i] = (float)(((double *)data)[idx]);
		}
	}

	if (doByteswap) {
		for (uint32_t i = 0; i < num; i++)
			byteswap(buffer + i, sizeof(float));
	}
}

static void
local_skipRow(FILE *f, long numInRow)
{
	xfseek(f, numInRow * sizeof(float), SEEK_CUR);
}

static void
local_skipPlane(FILE *f, long numInPlane)
{
	int b1, b2;

	xfread(&b1, sizeof(int), 1, f);

	xfseek(f, numInPlane * sizeof(float), SEEK_CUR);

	xfread(&b2, sizeof(int), 1, f);
	if (b1 != b2)
		diediedie(EXIT_FAILURE);
}

static void
local_writeHeader(grafic_t grafic, FILE *f)
{
	if (grafic->machineEndianess != grafic->fileEndianess) {
		local_swapHeaderValues(grafic); // swap to file encoding
		byteswap(&(grafic->headerSkip), sizeof(int));
	}

	xfwrite(&(grafic->headerSkip), sizeof(int), 1, f);
	xfwrite(&(grafic->np1), sizeof(int), 1, f);
	xfwrite(&(grafic->np2), sizeof(int), 1, f);
	xfwrite(&(grafic->np3), sizeof(int), 1, f);
	if (!grafic->isWhiteNoise) {
		xfwrite(&(grafic->dx), sizeof(float), 1, f);
		xfwrite(&(grafic->x1o), sizeof(float), 1, f);
		xfwrite(&(grafic->x2o), sizeof(float), 1, f);
		xfwrite(&(grafic->x3o), sizeof(float), 1, f);
		xfwrite(&(grafic->astart), sizeof(float), 1, f);
		xfwrite(&(grafic->omegam), sizeof(float), 1, f);
		xfwrite(&(grafic->omegav), sizeof(float), 1, f);
		xfwrite(&(grafic->h0), sizeof(float), 1, f);
	} else
		xfwrite(&(grafic->iseed), sizeof(int), 1, f);
	xfwrite(&(grafic->headerSkip), sizeof(int), 1, f);

	if (grafic->machineEndianess != grafic->fileEndianess) {
		local_swapHeaderValues(grafic); // swap back to system
		byteswap(&(grafic->headerSkip), sizeof(int));
	}
}

static void
local_writePlane(FILE           *f,
                 void           *data,
                 graficFormat_t format,
                 int            numComponents,
                 size_t         dataOffset,
                 int            numPlane)
{
	if ((numComponents == 1) && (format == GRAFIC_FORMAT_FLOAT))
		xfwrite(((float *)data) + dataOffset, sizeof(float), numPlane, f);
	else if (format == GRAFIC_FORMAT_FLOAT) {
		for (int i = 0; i < numPlane; i++)
			xfwrite(((float *)data) + (dataOffset + i) * numComponents,
			        sizeof(float), 1, f);
	} else {
		float tmp;
		for (int i = 0; i < numPlane; i++) {
			tmp = (float)((double *)data)[(dataOffset + i) * numComponents];
			xfwrite(&tmp, sizeof(float), 1, f);
		}
	}
}
