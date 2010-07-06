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
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/xfile.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "bov_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static bovEndian_t
local_getMachineEndianess(void);

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

static bovEndian_t
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


/*--- Implementations of exported functios ------------------------------*/
extern bov_t
bov_new(void)
{
	bov_t bov;

	bov                   = xmalloc(sizeof(struct bov_struct));
	bov->bovFileName      = NULL;
	bov->bovFilePath      = NULL;
	bov->machineEndianess = local_getMachineEndianess();
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

/*--- Implementations of local functions --------------------------------*/
static bovEndian_t
local_getMachineEndianess(void)
{
	const static int temp = 1;
	unsigned char    *p   = (unsigned char *)&temp;

	// Otherwise the magic might not work as intended
	assert(4 * sizeof(unsigned char) == sizeof(int));

	return *p == 0 ? BOV_ENDIAN_BIG : BOV_ENDIAN_LITTLE;
}

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

static bovEndian_t
local_readEndian(const char *line)
{
	char        *endian = local_readString(line);
	bovEndian_t rtn;

	if (strcmp(endian, "LITTLE") == 0)
		rtn = BOV_ENDIAN_LITTLE;
	else if (strcmp(endian, "BIG") == 0)
		rtn = BOV_ENDIAN_BIG;
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
