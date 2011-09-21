// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file makeSiloRoot/main.c
 * @ingroup  toolsMakeSiloRoot
 * @brief  Implements the main routine for makeSiloRoot.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include "makeSiloRoot.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/cmdline.h"


/*--- Local defines -----------------------------------------------------*/
#define THIS_PROGNAME "makeSiloRoot"


/*--- Local variables ---------------------------------------------------*/
static char *localSiloFileStem   = NULL;
static char *localSiloFileSuffix = NULL;
static char *localOutputFileName = NULL;
static bool localForce           = false;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);

static void
local_registerCleanUpFunctions(void);

static makeSiloRoot_t
local_getMsr(void);

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
	makeSiloRoot_t msr;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	msr = local_getMsr();
	makeSiloRoot_run(msr);
	makeSiloRoot_del(&msr);

	return EXIT_SUCCESS;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv)
{
	cmdline_t cmdline;

	cmdline = local_cmdlineSetup();
	cmdline_parse(cmdline, *argc, *argv);
	local_checkForPrematureTermination(cmdline);
	if (cmdline_checkOptSetByNum(cmdline, 2))
		cmdline_getOptValueByNum(cmdline, 2, &localSiloFileSuffix);
	else
		localSiloFileSuffix = xstrdup("");
	localForce = cmdline_checkOptSetByNum(cmdline, 3);
	cmdline_getArgValueByNum(cmdline, 0, &localSiloFileStem);
	cmdline_getArgValueByNum(cmdline, 1, &localOutputFileName);
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
	xfree(localSiloFileStem);
	if (localSiloFileSuffix != NULL)
		xfree(localSiloFileSuffix);
	xfree(localOutputFileName);
#ifdef XMEM_TRACK_MEM
	printf("\n");
	xmem_info(stdout);
	printf("\n");
#endif
	printf("\nVertu sæl/sæll...\n");
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

	cmdline = cmdline_new(2, 4, THIS_PROGNAME);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "suffix",
	                     "The suffix of the files (if any).",
	                     true, CMDLINE_TYPE_STRING);
	(void)cmdline_addOpt(cmdline, "force",
	                     "Use this to overwrite the output file.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "The stem of the files for which to produce"
	                     " a root file.  This will be expanded with "
	                     "numbers to find all matching files.",
	                     CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(cmdline,
	                     "Gives the filename of the resulting root file.",
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

static makeSiloRoot_t
local_getMsr(void)
{
	makeSiloRoot_t msr;

	msr = makeSiloRoot_new(localSiloFileStem,
	                       localSiloFileSuffix,
	                       localOutputFileName,
	                       localForce);

	return msr;
}


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsMakeSiloRootMain Driver routine
 * @ingroup  toolsMakeSiloRoot
 * @brief  Provides the driver for @ref toolsMakeSiloRoot.
 */
