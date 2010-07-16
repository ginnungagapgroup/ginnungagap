// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "bov.h"
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include "endian.h"
#include "xmem.h"
#include "xstring.h"
#include "xfile.h"
#include "diediedie.h"
#include "byteswap.h"


/*--- Implemention of main structure ------------------------------------*/
#include "bov_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_readBov(bov_t bov, FILE *f);

static char *
local_getFieldName(const char *line, bov_t bov);

static void
local_parseLine(const char *fieldName, const char *line, bov_t bov);

static double
local_readDouble(const char *line);

static char *
local_readString(const char *line);

static void
local_readUint323(const char *line, uint32_t *vals);

static void
local_readInt3(const char *line, int *vals);

static bovFormat_t
local_readDataFormat(const char *line);

static endian_t
local_readEndian(const char *line);

static bovCentering_t
local_readCentering(const char *line);

static void
local_readDouble3(const char *line, double *vals);

static int
local_readInt(const char *line);

static bool
local_readBool(const char *line);

static int
local_readDataComponent(const char *line);

static void
local_readBuffered(bov_t       bov,
                   void        *data,
                   bovFormat_t dataFormat,
                   int         numComponents,
                   size_t      numElements,
                   FILE        *f);

static void
local_readWindowedActualRead(bov_t       bov,
                             void        *data,
                             bovFormat_t dataFormat,
                             int         numComponents,
                             uint32_t    *idxLo,
                             uint32_t    *dims);

static size_t
local_getSizeForFormat(bovFormat_t format);

static void
local_readPencil(bov_t  bov,
                 void   *buffer,
                 size_t numElements,
                 FILE   *f);

static void
local_mvBufferToData(bov_t       bov,
                     void        *buffer,
                     size_t      numElements,
                     void        *data,
                     size_t      dataOffset,
                     bovFormat_t dataFormat,
                     int         numComponents);

static void
local_cpBufferToData(bov_t       bov,
                     void        *buffer,
                     size_t      numElements,
                     void        *data,
                     size_t      dataOffset,
                     bovFormat_t dataFormat,
                     int         numComponents);


/*--- Implementations of exported functios ------------------------------*/
extern bov_t
bov_new(void)
{
	bov_t bov;

	bov                   = xmalloc(sizeof(struct bov_struct));
	bov->bovFileName      = NULL;
	bov->bovFilePath      = NULL;
	bov->machineEndianess = endian_getSystemEndianess();
	bov->time             = 0.0;
	bov->data_file        = NULL;
	bov->data_format      = BOV_FORMAT_BYTE;
	bov->variable         = NULL;
	bov->data_endian      = bov->machineEndianess;
	bov->centering        = BOV_CENTERING_ZONAL;
	for (int i = 0; i < 3; i++) {
		bov->data_size[i]      = 0;
		bov->brick_origin[i]   = 0.0;
		bov->brick_size[i]     = 1.0;
		bov->data_bricklets[i] = 0;
	}
	bov->byte_offset     = 0;
	bov->divide_brick    = false;
	bov->data_components = 1;

	return bov;
}

extern bov_t
bov_newFromFile(const char *fileName)
{
	bov_t bov;
	FILE  *f;

	assert(fileName != NULL);

	f                = xfopen(fileName, "r");
	bov              = bov_new();
	bov->bovFileName = xstrdup(fileName);
	bov->bovFilePath = xdirname(fileName);
	local_readBov(bov, f);

	xfclose(&f);

	return bov;
}

extern void
bov_del(bov_t *bov)
{
	assert(bov != NULL && *bov != NULL);

	if ((*bov)->bovFileName != NULL)
		xfree((*bov)->bovFileName);
	if ((*bov)->bovFilePath != NULL)
		xfree((*bov)->bovFilePath);
	if ((*bov)->data_file != NULL)
		xfree((*bov)->data_file);
	if ((*bov)->variable != NULL)
		xfree((*bov)->variable);
	xfree(*bov);

	*bov = NULL;
}

extern double
bov_getTime(const bov_t bov)
{
	assert(bov != NULL);

	return bov->time;
}

extern char *
bov_getDataFileName(const bov_t bov)
{
	char *rtn;

	assert(bov != NULL);

	if (bov->data_file[0] == '/')
		rtn = xstrdup(bov->data_file);
	else {
		char *tmp = xstrmerge(bov->bovFilePath, "/");
		rtn = xstrmerge(tmp, bov->data_file);
		xfree(tmp);
	}

	return rtn;
}

extern void
bov_getDataSize(const bov_t bov, uint32_t *dataSize)
{
	assert(bov != NULL);
	assert(dataSize != NULL);

	dataSize[0] = bov->data_size[0];
	dataSize[1] = bov->data_size[1];
	dataSize[2] = bov->data_size[2];
}

extern bovFormat_t
bov_getDataFormat(const bov_t bov)
{
	assert(bov != NULL);

	return bov->data_format;
}

extern char *
bov_getVarName(const bov_t bov)
{
	assert(bov != NULL);

	return xstrdup(bov->variable);
}

extern endian_t
bov_getDataEndian(const bov_t bov)
{
	assert(bov != NULL);

	return bov->data_endian;
}

extern bovCentering_t
bov_getCentering(const bov_t bov)
{
	assert(bov != NULL);

	return bov->centering;
}

extern void
bov_getBrickOrigin(const bov_t bov, double *brickOrigin)
{
	assert(bov != NULL);
	assert(brickOrigin != NULL);

	brickOrigin[0] = bov->brick_origin[0];
	brickOrigin[1] = bov->brick_origin[1];
	brickOrigin[2] = bov->brick_origin[2];
}

extern void
bov_getBrickSize(const bov_t bov, double *brickSize)
{
	assert(bov != NULL);
	assert(brickSize != NULL);

	brickSize[0] = bov->brick_size[0];
	brickSize[1] = bov->brick_size[1];
	brickSize[2] = bov->brick_size[2];
}

extern int
bov_getDataComponents(const bov_t bov)
{
	assert(bov != NULL);

	return bov->data_components;
}

extern void
bov_setTime(bov_t bov, const double time)
{
	assert(bov != NULL);

	bov->time = time;
}

extern void
bov_setDataFileName(bov_t bov, const char *dataFileName)
{
	assert(bov != NULL);
	assert(dataFileName != NULL);

	if (bov->data_file != NULL)
		xfree(bov->data_file);

	bov->data_file = xstrdup(dataFileName);
}

extern void
bov_setDataSize(bov_t bov, const uint32_t *dataSize)
{
	assert(bov != NULL);
	assert(dataSize != NULL);

	bov->data_size[0] = dataSize[0];
	bov->data_size[1] = dataSize[1];
	bov->data_size[2] = dataSize[2];
}

extern void
bov_setDataFormat(bov_t bov, const bovFormat_t format)
{
	assert(bov != NULL);

	bov->data_format = format;
}

extern void
bov_setVarName(bov_t bov, const char *varName)
{
	assert(bov != NULL);
	assert(varName != NULL);

	if (bov->variable != NULL)
		xfree(bov->variable);

	bov->variable = xstrdup(varName);
}

extern void
bov_setDataEndian(bov_t bov, const endian_t endian)
{
	assert(bov != NULL);

	bov->data_endian = endian;
}

extern void
bov_setCentering(bov_t bov, const bovCentering_t centering)
{
	assert(bov != NULL);

	bov->centering = centering;
}

extern void
bov_setBrickOrigin(bov_t bov, const double *brickOrigin)
{
	assert(bov != NULL);
	assert(brickOrigin != NULL);

	bov->brick_origin[0] = brickOrigin[0];
	bov->brick_origin[1] = brickOrigin[1];
	bov->brick_origin[2] = brickOrigin[2];
}

extern void
bov_setBrickSize(bov_t bov, const double *brickSize)
{
	assert(bov != NULL);
	assert(brickSize != NULL);

	bov->brick_size[0] = brickSize[0];
	bov->brick_size[1] = brickSize[1];
	bov->brick_size[2] = brickSize[2];
}

extern void
bov_setDataComponents(bov_t bov, const int numComponents)
{
	assert(bov != NULL);
	assert(numComponents > 0);

	bov->data_components = numComponents;
}

extern void
bov_read(bov_t       bov,
         void        *data,
         bovFormat_t dataFormat,
         int         numComponents)
{
	FILE   *f;
	char   *fileName;
	size_t numElements;

	assert(bov != NULL);
	assert(data != NULL);
	assert(numComponents > 0);

	numElements = bov->data_size[0] * bov->data_size[1] * bov->data_size[2];
	fileName    = bov_getDataFileName(bov);
	f           = xfopen(fileName, "rb");
	xfseek(f, bov->byte_offset, SEEK_SET);

	if ((bov->data_format == dataFormat)
	    && (bov->data_components == numComponents)) {
		local_readPencil(bov, data, numElements, f);
	} else {
		local_readBuffered(bov, data, dataFormat, numComponents,
		                   numElements, f);
	}

	xfclose(&f);
	xfree(fileName);
}

extern void
bov_readWindowed(bov_t       bov,
                 void        *data,
                 bovFormat_t dataFormat,
                 int         numComponents,
                 uint32_t    *idxLo,
                 uint32_t    *dims)
{
	assert(bov != NULL);
	assert(data != NULL);
	assert(numComponents > 0);
	assert(idxLo != NULL);
	assert(dims != NULL);
	assert(dims[0] > 0 && dims[1] > 0 && dims[2] > 0);

	if ((idxLo[0] + dims[0] > bov->data_size[0])
	    || (idxLo[1] + dims[1] > bov->data_size[1])
	    || (idxLo[2] + dims[2] > bov->data_size[2])) {
		fprintf(stderr, "Window too large for data in bov :(\n");
		diediedie(EXIT_FAILURE);
	}

	local_readWindowedActualRead(bov, data, dataFormat, numComponents,
	                             idxLo, dims);
}

/*--- Implementations of local functions --------------------------------*/
static void
local_readBov(bov_t bov, FILE *f)
{
	char   *line = NULL;
	size_t n     = 0;

	while (xgetline(&line, &n, f) != 0) {
		char *fieldName = NULL;
		if (line[0] == '#')
			continue;
		fieldName = local_getFieldName(line, bov);
		local_parseLine(fieldName, line, bov);
		xfree(fieldName);
	}
	xfree(line);
}

static char *
local_getFieldName(const char *line, bov_t bov)
{
	int  startField = 0;
	int  endField   = 0;
	char *rtn       = NULL;

	while (line[startField] != '\0' && isspace(line[startField]))
		startField++;
	endField = startField;
	while (line[endField] != '\0' && !isspace(line[endField])
	       && line[endField] != ':')
		endField++;

	rtn = xmalloc(sizeof(char)
	              * (endField - startField + 1));
	memcpy(rtn, line + startField, endField - startField);
	rtn[endField - startField] = '\0';

	return rtn;
}

static void
local_parseLine(const char *fieldName, const char *line, bov_t bov)
{
	if (strcmp(fieldName, "TIME") == 0) {
		bov->time = local_readDouble(line);
	} else if (strcmp(fieldName, "DATA_FILE") == 0) {
		bov->data_file = local_readString(line);
	} else if (strcmp(fieldName, "DATA_SIZE") == 0) {
		local_readUint323(line, bov->data_size);
	} else if (strcmp(fieldName, "DATA_FORMAT") == 0) {
		bov->data_format = local_readDataFormat(line);
	} else if (strcmp(fieldName, "VARIABLE") == 0) {
		bov->variable = local_readString(line);
	} else if (strcmp(fieldName, "DATA_ENDIAN") == 0) {
		bov->data_endian = local_readEndian(line);
	} else if (strcmp(fieldName, "CENTERING") == 0) {
		bov->centering = local_readCentering(line);
	} else if (strcmp(fieldName, "BRICK_ORIGIN") == 0) {
		local_readDouble3(line, bov->brick_origin);
	} else if (strcmp(fieldName, "BRICK_SIZE") == 0) {
		local_readDouble3(line, bov->brick_size);
	} else if (strcmp(fieldName, "BYTE_OFFSET") == 0) {
		bov->byte_offset = local_readInt(line);
	} else if (strcmp(fieldName, "DIVIDE_BRICK") == 0) {
		bov->divide_brick = local_readBool(line);
	} else if (strcmp(fieldName, "DATA_BRICKLETS") == 0) {
		local_readInt3(line, bov->data_bricklets);
	} else if (strcmp(fieldName, "DATA_COMPONENTS") == 0) {
		bov->data_components = local_readDataComponent(line);
	} else {
		fprintf(stderr, "Parse error, unknown field %s\n", fieldName);
		diediedie(EXIT_FAILURE);
	}
}

static double
local_readDouble(const char *line)
{
	int    rtn;
	double val;

	rtn = sscanf(line, " %*s %lf", &val);
	if (rtn != 1) {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}

	return val;
}

static char *
local_readString(const char *line)
{
	int  i    = 0;
	char *rtn = NULL;

	while (line[i] != ':')
		i++;
	while (line[i] == ':')
		i++;
	while (isspace(line[i]))
		i++;

	rtn = xstrdup(line + i);

	i   = 0;
	while (!isspace(rtn[i]) && rtn[i] != '\n')
		i++;
	rtn[i] = '\0';

	return rtn;
}

static void
local_readUint323(const char *line, uint32_t *vals)
{
	int rtn;

	rtn = sscanf(line, " %*s %" SCNu32 " %" SCNu32 " %" SCNu32,
	             vals, vals + 1, vals + 2);
	if (rtn != 3) {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}
}

static void
local_readInt3(const char *line, int *vals)
{
	int rtn;

	rtn = sscanf(line, " %*s %i %i %i",
	             vals, vals + 1, vals + 2);
	if (rtn != 3) {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}
}

static bovFormat_t
local_readDataFormat(const char *line)
{
	char        *frmt = local_readString(line);
	bovFormat_t rtn;

	if (strcmp(frmt, "BYTE") == 0)
		rtn = BOV_FORMAT_BYTE;
	else if (strcmp(frmt, "INT") == 0)
		rtn = BOV_FORMAT_INT;
	else if (strcmp(frmt, "FLOAT") == 0)
		rtn = BOV_FORMAT_FLOAT;
	else if (strcmp(frmt, "DOUBLE") == 0)
		rtn = BOV_FORMAT_DOUBLE;
	else {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}
	xfree(frmt);

	return rtn;
}

static endian_t
local_readEndian(const char *line)
{
	char     *endian = local_readString(line);
	endian_t rtn;

	if (strcmp(endian, "LITTLE") == 0)
		rtn = ENDIAN_LITTLE;
	else if (strcmp(endian, "BIG") == 0)
		rtn = ENDIAN_BIG;
	else {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}
	xfree(endian);

	return rtn;
}

static bovCentering_t
local_readCentering(const char *line)
{
	char           *cent = local_readString(line);
	bovCentering_t rtn;

	if (strcmp(cent, "zonal") == 0)
		rtn = BOV_CENTERING_ZONAL;
	else if (strcmp(cent, "nodal") == 0)
		rtn = BOV_CENTERING_NODAL;
	else {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}
	xfree(cent);

	return rtn;
}

static void
local_readDouble3(const char *line, double *vals)
{
	int rtn;

	rtn = sscanf(line, " %*s %lf %lf %lf", vals, vals + 1, vals + 2);
	if (rtn != 3) {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}
}

static int
local_readInt(const char *line)
{
	int rtn;
	int val;

	rtn = sscanf(line, " %*s %i", &val);
	if (rtn != 1) {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}

	return val;
}

static bool
local_readBool(const char *line)
{
	char *boolean = local_readString(line);
	bool rtn;

	if (strcmp(boolean, "true") == 0)
		rtn = true;
	else if (strcmp(boolean, "false") == 0)
		rtn = false;
	else {
		fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
		diediedie(EXIT_FAILURE);
	}
	xfree(boolean);

	return rtn;
}

static int
local_readDataComponent(const char *line)
{
	int rtn;
	int val;

	rtn = sscanf(line, " %*s %i", &val);
	if (rtn != 1) {
		char *dataComp = local_readString(line);

		if (strcmp(dataComp, "COMPLEX") == 0)
			val = 2;
		else {
			fprintf(stderr, "Parse Error in line\n --> '%s'\n", line);
			diediedie(EXIT_FAILURE);
		}
		xfree(dataComp);
	}

	return val;
}

static void
local_readBuffered(bov_t       bov,
                   void        *data,
                   bovFormat_t dataFormat,
                   int         numComponents,
                   size_t      numElements,
                   FILE        *f)
{
	int  sizeBufferEle = local_getSizeForFormat(bov->data_format);
	void *buffer       = xmalloc(sizeBufferEle * bov->data_components
	                             * numElements);

	local_readPencil(bov, buffer, numElements, f);

	if (bov->data_format == dataFormat)
		local_mvBufferToData(bov, buffer, numElements,
		                     data, 0, dataFormat, numComponents);
	else
		local_cpBufferToData(bov, buffer, numElements,
		                     data, 0, dataFormat, numComponents);

	xfree(buffer);
}

static void
local_readWindowedActualRead(bov_t       bov,
                             void        *data,
                             bovFormat_t dataFormat,
                             int         numComponents,
                             uint32_t    *idxLo,
                             uint32_t    *dims)
{
	char   *dataFileName = bov_getDataFileName(bov);
	FILE   *f            = xfopen(dataFileName, "rb");
	size_t sizePerEle    = local_getSizeForFormat(bov->data_format);
	void   *buffer       = xmalloc(sizePerEle * bov->data_components
	                               * dims[0]);

	for (uint32_t k = idxLo[2]; k - idxLo[2] < dims[2]; k++) {
		for (uint32_t j = idxLo[1]; j - idxLo[1] < dims[1]; j++) {
			long   offset;
			size_t dataOffset;
			offset     = idxLo[0]
			             + (j + k * bov->data_size[1]) * bov->data_size[0];
			offset    *= sizePerEle * bov->data_components;
			offset    += bov->byte_offset;
			dataOffset = (j - idxLo[1] + (k - idxLo[2]) * dims[1])
			             * dims[0];
			xfseek(f, offset, SEEK_SET);
			local_readPencil(bov, buffer, (size_t)(dims[0]), f);
			if (dataFormat == bov->data_format)
				local_mvBufferToData(bov, buffer, (size_t)(dims[0]),
				                     data, dataOffset, dataFormat,
				                     numComponents);
			else
				local_cpBufferToData(bov, buffer, (size_t)(dims[0]),
				                     data, dataOffset, dataFormat,
				                     numComponents);
		}
	}

	xfree(buffer);
	xfclose(&f);
	xfree(dataFileName);
} /* local_readWindowedActualRead */

static size_t
local_getSizeForFormat(bovFormat_t format)
{
	size_t size;

	if (format == BOV_FORMAT_DOUBLE)
		size = sizeof(double);
	else if (format == BOV_FORMAT_FLOAT)
		size = sizeof(float);
	else if (format == BOV_FORMAT_INT)
		size = sizeof(int);
	else
		size = sizeof(char);

	return size;
}

static void
local_readPencil(bov_t  bov,
                 void   *buffer,
                 size_t numElements,
                 FILE   *f)
{
	size_t sizePerEle = local_getSizeForFormat(bov->data_format);

	xfread(buffer, sizePerEle * bov->data_components, numElements, f);

	if (bov->machineEndianess != bov->data_endian) {
		for (size_t i = 0; i < bov->data_components * numElements; i++)
			byteswap(((char *)buffer) + (i * sizePerEle), sizePerEle);
	}
}

static void
local_mvBufferToData(bov_t       bov,
                     void        *buffer,
                     size_t      numElements,
                     void        *data,
                     size_t      dataOffset,
                     bovFormat_t dataFormat,
                     int         numComponents)
{
	int sizeBufferEle = local_getSizeForFormat(bov->data_format);
	int sizeDataEle   = local_getSizeForFormat(dataFormat);
	int recData       = sizeDataEle * numComponents;
	int recBuffer     = sizeBufferEle * bov->data_components;

	if (recData == recBuffer)
		memcpy((char *)data + recData * dataOffset, buffer,
		       recData * numElements);
	else {
		for (size_t i = 0; i < numElements; i++)
			memcpy((char *)data + recData * (dataOffset + i),
			       (char *)buffer + recBuffer * i,
			       recData);
	}
}

#define cpy(trgt)                                                        \
	{                                                                    \
		if (dataFormat == BOV_FORMAT_INT) {                              \
			for (int j = 0; j < numComponents; j++)                      \
				dat.i[i * numComponents                                  \
				      + j] = (int)trgt[i * bov->data_components + j];    \
		} else if (dataFormat == BOV_FORMAT_FLOAT) {                     \
			for (int j = 0; j < numComponents; j++)                      \
				dat.f[i * numComponents                                  \
				      + j] = (float)trgt[i * bov->data_components + j];  \
		} else if (dataFormat == BOV_FORMAT_DOUBLE) {                    \
			for (int j = 0; j < numComponents; j++)                      \
				dat.d[i * numComponents                                  \
				      + j] = (double)trgt[i * bov->data_components + j]; \
		} else {                                                         \
			for (int j = 0; j < numComponents; j++)                      \
				dat.b[i * numComponents                                  \
				      + j] = (char)trgt[i * bov->data_components + j];   \
		}                                                                \
	}
#define recsize (local_getSizeForFormat(dataFormat) * numComponents)
static void
local_cpBufferToData(bov_t       bov,
                     void        *buffer,
                     size_t      numElements,
                     void        *data,
                     size_t      dataOffset,
                     bovFormat_t dataFormat,
                     int         numComponents)
{
	union { int    *i;
		    double *d;
		    float  *f;
		    char   *b;
	} dat, buf;

	dat.b = (char *)data + dataOffset * recsize;
	buf.b = (char *)buffer;

	switch (bov->data_format) {
	case BOV_FORMAT_INT:
		for (size_t i = 0; i < numElements; i++)
			cpy(buf.i);
		break;
	case BOV_FORMAT_FLOAT:
		for (size_t i = 0; i < numElements; i++)
			cpy(buf.f);
		break;
	case BOV_FORMAT_DOUBLE:
		for (size_t i = 0; i < numElements; i++)
			cpy(buf.d);
		break;
	case BOV_FORMAT_BYTE:
		for (size_t i = 0; i < numElements; i++)
			cpy(buf.b);
		break;
	default:
		break;
	}
}

#undef cpy
#undef recsize
