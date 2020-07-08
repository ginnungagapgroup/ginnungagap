// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file checkZero/main.c
 * @ingroup  toolscheckZeroMain
 * @brief  Implements the main routine for checkZero.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include "checkZeroConfig.h"
#include "checkZero.h"
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
#define THIS_PROGNAME "checkZero"


/*--- Local variables ---------------------------------------------------*/
static char *localIniFileName = NULL;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);

static void
local_registerCleanUpFunctions(void);

static checkZero_t
local_getTest(void);

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
	checkZero_t te;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	te = local_getTest();
	checkZero_run(te);
	checkZero_del(&te);

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

	if (localIniFileName != NULL)
		xfree(localIniFileName);
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//MPI_Finalize();
#endif
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

	cmdline = cmdline_new(1, 2, THIS_PROGNAME);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "Gives the name of the configuration file.",
	                     CMDLINE_TYPE_STRING);

	return cmdline;
}

static void
local_checkForPrematureTermination(cmdline_t cmdline)
{
	int rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	// This relies on the knowledge of which number is which option!
	// Not nice style, but the respective calls are directly above.
	if (cmdline_checkOptSetByNum(cmdline, 0)) {
		if (rank == 0) {
			PRINT_VERSION_INFO2(stdout, THIS_PROGNAME);
			PRINT_BUILT_INFO(stdout);
			printf("%s", CONFIG_SUMMARY_STRING);
		}
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

static checkZero_t
local_getTest(void)
{
	parse_ini_t            ini;
	checkZero_t te;

	ini = parse_ini_open(localIniFileName);
	if (ini == NULL) {
		fprintf(stderr, "FATAL:  Could not open %s for reading.\n",
		        localIniFileName);
		exit(EXIT_FAILURE);
	}

	te = checkZero_newFromIni(ini);

	parse_ini_close(&ini);

	return te;
}

/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolscheckZeroMain Driver routine
 * @ingroup  toolscheckZero
 * @brief  Provides the driver for @ref toolscheckZero.
 *
 * This page describes how to run checkZero from the command
 * line.
 *
 * @section toolsRSCSynopsis Synopsis
 * <code>checkZero [--version] [--help] arg0</code>
 *
 * @section toolsRSCArgument Argument
 *
 * The argument <code>arg0</code> has to be a string giving the name of (and
 * path to) the ini file that contains the configuration of the real space
 * contraints that should be executed.  The structure of that file is
 * described in more detail @link toolsRSCSetupIniFormat here @endlink
 *
 * @section toolsRSCOptions  Options
 *
 * @subsection toolsRSCOptionsVersion --version
 *
 * This will simply print a detailed version information to the screen and
 * then successfully terminate.
 *
 * @subsection toolsRSCOptionsHelp --help
 *
 * This will print a help page to the screen (effectively this page).
 *
 * @section toolsRSCSA  See Also
 *
 * The actual parsing of the command line parameters is done in
 * tools/checkZero/main.c, please check that file for the latest
 * take on the actual synopsis of this program (the documentation might be
 * out of date).
 */
