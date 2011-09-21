// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file showFreqs/main.c
 * @ingroup  toolsShowFreqsMain
 * @brief  Implements the main routine for showFreqs.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include "showFreqs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "../../src/libutil/cmdline.h"
#include "../../src/libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/
#define THIS_PROGNAME "showFreqs"


/*--- Local variables ---------------------------------------------------*/
static int  localNumDims = 1;
static int  localDims[3] = { 0, 0, 0 };
static char *localOutFileName = NULL;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);

static void
local_registerCleanUpFunctions(void);

static cmdline_t
local_cmdlineSetup(void);

static void
local_checkForPrematureTermination(cmdline_t cmdline);

static void
local_finalMessage(void);

static void
local_verifyCloseOfStdout(void);


/*--- M A I N -----------------------------------------------------------*/
int
main(int argc, char **argv)
{
	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	showFreqs(localNumDims, localDims, localOutFileName);

	return EXIT_SUCCESS;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv)
{
	cmdline_t cmdline;
	char      *tmp;

	cmdline = local_cmdlineSetup();
	cmdline_parse(cmdline, *argc, *argv);
	local_checkForPrematureTermination(cmdline);

	cmdline_getArgValueByNum(cmdline, 0, &localNumDims);
	if (localNumDims <= 0 || localNumDims > 3) {
		exit(EXIT_FAILURE);
	}

	cmdline_getArgValueByNum(cmdline, 1, &tmp);
	if (localNumDims == 1)
		sscanf(tmp, "%i", localDims);
	else if (localNumDims == 2)
		sscanf(tmp, "%i,%i", localDims, localDims + 1);
	else 
		sscanf(tmp, "%i,%i,%i", localDims, localDims + 1, localDims + 2);
	xfree(tmp);

	cmdline_getArgValueByNum(cmdline, 2, &localOutFileName);
	
	cmdline_del(&cmdline);
}

static void
local_registerCleanUpFunctions(void)
{
	if (atexit(&local_verifyCloseOfStdout) != 0) {
		fprintf(stderr, "cannot register `%s' as exit function\n",
		        "local_verifyCloseOfStdout");
		exit(EXIT_FAILURE);
	}
	if (atexit(&local_finalMessage) != 0) {
		fprintf(stderr, "cannot register `%s' as exit function\n",
		        "local_finalMessage");
		exit(EXIT_FAILURE);
	}
}

static void
local_finalMessage(void)
{
	xfree(localOutFileName);
#ifdef XMEM_TRACK_MEM
	printf("\n");
	xmem_info(stdout);
	printf("\n");
#endif
	printf("\n");
}

static void
local_verifyCloseOfStdout(void)
{
	if (fclose(stdout) != 0) {
		int errnum = errno;
		fprintf(stderr, "%s", strerror(errnum));
		_Exit(EXIT_FAILURE);
	}
}

static cmdline_t
local_cmdlineSetup(void)
{
	cmdline_t cmdline;

	cmdline = cmdline_new(3, 2, THIS_PROGNAME);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "The number of dimenions (must be 1, 2 or 3).",
	                     CMDLINE_TYPE_INT);
	(void)cmdline_addArg(cmdline,
	                     "The dimensions of the grid (i.e. 5,42,3).",
	                     CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(cmdline,
	                     "The file to which the output will be written "
	                     "(.dat and .bov will be appended automatically).",
	                     CMDLINE_TYPE_STRING);

	return cmdline;
}

static void
local_checkForPrematureTermination(cmdline_t cmdline)
{
	// This relies on the knowledge of which number is which option!
	// Not nice style, but the respective calls are directly above.
	if (cmdline_checkOptSetByNum(cmdline, 0)) {
		PRINT_VERSION_INFO2(stdout, THIS_PROGNAME);
		cmdline_del(&cmdline);
		exit(EXIT_SUCCESS);
	}
	if (cmdline_checkOptSetByNum(cmdline, 1)) {
		cmdline_printHelp(cmdline, stdout);
		cmdline_del(&cmdline);
		exit(EXIT_SUCCESS);
	}
	if (!cmdline_verify(cmdline)) {
		cmdline_printHelp(cmdline, stderr);
		cmdline_del(&cmdline);
		exit(EXIT_FAILURE);
	}
}


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsShowFreqsMain Driver routine
 * @ingroup  toolsShowFreqs
 * @brief  Provides the driver for @ref toolsShowFreqs.
 */
