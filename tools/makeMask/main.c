// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file makeMask/main.c
 * @ingroup  toolsMakeMaskMain
 * @brief  Implements the main routine for makeMask.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include "makeMaskConfig.h"
#include "makeMask.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/cmdline.h"
#include "../../src/libutil/parse_ini.h"


/*--- Local defines -----------------------------------------------------*/
#define THIS_PROGNAME "makeMask"


/*--- Local variables ---------------------------------------------------*/
static char *localSectionName = NULL;
static char *localIniFileName = NULL;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);

static void
local_registerCleanUpFunctions(void);

static makeMask_t
local_getMama(void);

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
	makeMask_t mama;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	mama = local_getMama();
	makeMask_run(mama);
	makeMask_del(&mama);

	return EXIT_SUCCESS;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv)
{
	cmdline_t cmdline;

#ifdef WITH_MPI
	MPI_Init(argc, argv);
#endif

	cmdline = local_cmdlineSetup();
	cmdline_parse(cmdline, *argc, *argv);
	local_checkForPrematureTermination(cmdline);
	if (cmdline_checkOptSetByNum(cmdline, 2))
		cmdline_getOptValueByNum(cmdline, 2, &localSectionName);
	else
		localSectionName = xstrdup(MAKEMASK_SECTIONNAME_MASK);
	cmdline_getArgValueByNum(cmdline, 0, &localIniFileName);
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
	int rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Finalize();
#endif
	if (localSectionName != NULL)
		xfree(localSectionName);
	if (localIniFileName != NULL)
		xfree(localIniFileName);
	if (rank == 0) {
#ifdef XMEM_TRACK_MEM
		printf("\n");
		xmem_info(stdout);
		printf("\n");
#endif
		printf("\nVertu sæl/sæll...\n");
	}
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

	cmdline = cmdline_new(1, 3, THIS_PROGNAME);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(
	    cmdline,
	    "sectionName",
	    "The name of the section in the configuration file.",
	    true,
	    CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(cmdline,
	                     "Gives the name of the configuration file.",
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

static makeMask_t
local_getMama(void)
{
	parse_ini_t ini;
	makeMask_t  mama;

	ini = parse_ini_open(localIniFileName);
	if (ini == NULL) {
		fprintf(stderr, "FATAL:  Could not open %s for reading.\n",
		        localIniFileName);
		exit(EXIT_FAILURE);
	}

	mama = makeMask_newFromIni(ini, localSectionName);

	parse_ini_close(&ini);

	return mama;
}


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsMakeMaskMain Driver routine
 * @ingroup  toolsMakeMask
 * @brief  Provides the driver for @ref toolsMakeMask.
 */
