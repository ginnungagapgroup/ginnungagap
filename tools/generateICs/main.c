// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/main.c
 * @ingroup  toolsGICSMain
 * @brief  Implements the main routine for generateICs.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include "generateICsConfig.h"
#include "generateICs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "generateICsFactory.h"
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/cmdline.h"
#include "../../src/libutil/parse_ini.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  The name of the program. */
static const char *local_thisProgramName = "generateICs";

/** @brief  Stores the name of the ini file that shoul be used. */
static char *local_iniFileName = NULL;

/** @brief  Stores the section name from which to parse the setup. */
static char *local_sectionName = NULL;

/** @brief  Stores the position of the various elements in the cmdline. */
struct local_cmdlinePos {
	/** @brief  The position for #local_iniFileName. */
	int iniFileName;
	/** @brief  The position for the help screen. */
	int help;
	/** @brief  The position for the version screen. */
	int version;
	/** @brief  The position for #local_sectionName. */
	int sectionName;
} local_cmdlinePos;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);

static void
local_registerCleanUpFunctions(void);

static generateICs_t
local_getGenerateICs(void);

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
	generateICs_t genics;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	genics = local_getGenerateICs();
	generateICs_run(genics);
	generateICs_del(&genics);

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
	cmdline_getArgValueByNum(cmdline, local_cmdlinePos.iniFileName,
	                         &local_iniFileName);
	if (cmdline_checkOptSetByNum(cmdline, local_cmdlinePos.sectionName))
		cmdline_getOptValueByNum(cmdline, local_cmdlinePos.sectionName,
		                         &local_sectionName);
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
	if (local_iniFileName != NULL)
		xfree(local_iniFileName);
	if (local_sectionName != NULL)
		xfree(local_sectionName);
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

	cmdline = cmdline_new(1, 3, local_thisProgramName);

	local_cmdlinePos.version
	        = cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	local_cmdlinePos.help
	    = cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	local_cmdlinePos.sectionName
	    = cmdline_addOpt(cmdline, "section",
	                     "Gives the section in the ini file to use.",
	                     false, CMDLINE_TYPE_STRING);
	local_cmdlinePos.iniFileName
	    = cmdline_addArg(cmdline,
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
	if (cmdline_checkOptSetByNum(cmdline, local_cmdlinePos.version)) {
		if (rank == 0) {
			PRINT_VERSION_INFO2(stdout, local_thisProgramName);
			PRINT_BUILT_INFO(stdout);
			printf("%s", CONFIG_SUMMARY_STRING);
		}
		cmdline_del(&cmdline);
		exit(EXIT_SUCCESS);
	}
	if (cmdline_checkOptSetByNum(cmdline, local_cmdlinePos.help)) {
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

static generateICs_t
local_getGenerateICs(void)
{
	parse_ini_t            ini;
	generateICs_t genics;

	ini = parse_ini_open(local_iniFileName);
	if (ini == NULL) {
		fprintf(stderr, "FATAL:  Could not open %s for reading.\n",
		        local_iniFileName);
		exit(EXIT_FAILURE);
	}

	genics = generateICsFactory_newFromIni(ini, local_sectionName);

	parse_ini_close(&ini);

	return genics;
}

/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGICSMain Driver routine
 * @ingroup  toolsGICS
 * @brief  Provides the driver for @ref toolsGICS.
 *
 * This page describes how to run @c generateICs from the command
 * line.
 *
 * @section toolsGICSSynopsis Synopsis
 * <code>generateICs [--version] [--help] arg0</code>
 *
 * @section toolsGICSArgument Argument
 *
 * The argument <code>arg0</code> has to be a string giving the name of (and
 * path to) the ini file that contains the configuration of the real space
 * contraints that should be executed.  The structure of that file is
 * described in more detail @link toolsGICSSetupIniFormat here @endlink
 *
 * @section toolsGICSOptions  Options
 *
 * @subsection toolsGICSOptionsVersion --version
 *
 * This will simply print a detailed version information to the screen and
 * then successfully terminate.
 *
 * @subsection toolsGICSOptionsHelp --help
 *
 * This will print a help page to the screen (effectively this page).
 *
 * @section toolsGICSSA  See Also
 *
 * The actual parsing of the command line parameters is done in
 * tools/generateICs/main.c, please check that file for the latest
 * take on the actual synopsis of this program (the documentation might be
 * out of date).
 */
