// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file ginnungagap/main.c
 * @ingroup  ginnungagapMain
 * @brief  Implements the main routine for ginnungagap.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "../../config.h"
#include "../../version.h"
#include "ginnungagap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#if (defined _OPENMP && WITH_FFT_FFTW3)
#  include <omp.h>
#  include <fftw3.h>
#endif
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/xfile.h"
#include "../libutil/cmdline.h"
#include "../libutil/parse_ini.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the name of the ini file used for the run.  */
char *localIniFname = NULL;

/**
 * @brief  Switches between full run mode (standard) and verify mode.
 *
 * Verify mode will stop after setting up the run.
 */
bool localVerify = false;

/**
 * @brief  Switches between full run mode (standard) and only init.
 *
 * If this is @c true, the code will stop after initialization.
 */
bool localInitOnly = false;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);


#if (defined _OPENMP && WITH_FFT_FFTW3)
static void
local_setThreadedFFTW(void);

#endif

static void
local_registerCleanUpFunctions(void);

static ginnungagap_t
local_getGinnungagap(void);

static cmdline_t
local_cmdlineSetup(void);

static void
local_checkForPrematureTermination(cmdline_t cmdline);

static void
local_finalMessage(void);

static void
local_verifyCloseOfStdout(void);


/**
 * @brief  Dumps the ini file that was read to a file.
 *
 * @param[in]  ini
 *                The ini file to dump.
 *
 * @return  Returns nothing.
 */
static void
local_dumpIniFile(parse_ini_t ini);


/*--- M A I N -----------------------------------------------------------*/
int
main(int argc, char **argv)
{
	ginnungagap_t g9p;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	g9p = local_getGinnungagap();
	if (localVerify)
		return EXIT_SUCCESS;

	ginnungagap_init(g9p);
	if (localInitOnly)
		return EXIT_SUCCESS;

	ginnungagap_run(g9p);
	ginnungagap_del(&g9p);

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
#if (defined _OPENMP && WITH_FFT_FFTW3)
	local_setThreadedFFTW();
#endif

	cmdline = local_cmdlineSetup();
	cmdline_parse(cmdline, *argc, *argv);
	local_checkForPrematureTermination(cmdline);
	cmdline_getArgValueByNum(cmdline, 0, &localIniFname);
	localVerify   = cmdline_checkOptSetByNum(cmdline, 2);
	localInitOnly = cmdline_checkOptSetByNum(cmdline, 3);
	cmdline_del(&cmdline);
}

#if (defined _OPENMP && WITH_FFT_FFTW3)
static void
local_setThreadedFFTW(void)
{
	int rank = 0;

#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	fftw_init_threads();
	fftw_plan_with_nthreads(omp_get_max_threads());

	if (rank == 0)
		printf("Using %i threads\n", omp_get_max_threads());
}

#endif

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
#if (defined _OPENMP && WITH_FFT_FFTW3)
	fftw_cleanup_threads();
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//MPI_Finalize();
#endif
	xfree(localIniFname);
	if (rank == 0) {
#ifdef XMEM_TRACK_MEM
		printf("\n");
		xmem_info(stdout);
		printf("\n");
#endif
		printf("\nAll done.\nThank you for using `%s'!\n"
		       "Vertu sæl/sæll...\n",
		       PACKAGE_NAME);
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

	cmdline = cmdline_new(1, 4, PACKAGE_NAME);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "verify",
	                     "This will stop after reading the ini file.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "initOnly",
	                     "This will stop after initialisation.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "An ini file containing the configuration.",
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
			PRINT_VERSION_INFO(stdout);
			PRINT_BUILT_INFO(stdout);
			printf("%s", CONFIG_SUMMARY_STRING);
		}
		cmdline_del(&cmdline);
		exit(EXIT_SUCCESS);
	}
	if (cmdline_checkOptSetByNum(cmdline, 1)) {
		if (rank == 0)
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

static ginnungagap_t
local_getGinnungagap(void)
{
	ginnungagap_t g9p;
	parse_ini_t   ini;
	int           rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	ini = parse_ini_open(localIniFname);
	if (ini == NULL) {
		fprintf(stderr, "FATAL:  Could not open %s for reading.\n",
		        localIniFname);
		exit(EXIT_FAILURE);
	}

	g9p = ginnungagap_new(ini);

	if (rank == 0)
		local_dumpIniFile(ini);

	parse_ini_close(&ini);

	return g9p;
}

static void
local_dumpIniFile(parse_ini_t ini)
{
	char *iniDump;
	FILE *f;

	iniDump = xstrmerge(localIniFname, ".dump");
	f       = xfopen(iniDump, "w");
	parse_ini_dump(ini, f);
	xfclose(&f);
	xfree(iniDump);
}

/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagapMain Driver Routine
 * @ingroup ginnungagap
 * @brief Provides the driver routine for ginnungagap.
 *
 * This page describes how to run Ginnungagap from the command line.
 *
 * @section g9pMainSynopsis Synopsis
 * <code>ginnungagap [--version] [--help] [--verify] [--initOnly]
 * arg0</code>
 *
 * @section g9pMainSynopsisArgument Argument
 *
 * The argument <code>arg0</code> has to be a string giving the name of
 * (and path to) the ini file that should be used for the run.  The
 * structure of that file is described @link pageInputfile here
 * @endlink and @link ginnungagapSetup here @endlink.
 *
 * @section g9pMainSynopsisOptions Options
 *
 * Ginnungagap supports the following options:
 *
 * @subsection g9pMainSynopsisOptionsVersion --version
 *
 * This will print a detailed version information to the screen
 * (including the exact configuration settings that were used to compile
 * the code) and then immediately exists.
 *
 * @subsection g9pMainSynopsisOptionsHelp --help
 *
 * This will print a help screen summarizing the usage of the binary and
 * then terminate the program.
 *
 * @subsection g9pMainSynopsisOptionsVerify --verify
 *
 * This will start the program and run through the setup routine, ie.
 * read the ini file.  After this has been done, the program exists.
 * This is meant as a quick check to see if the provided ini file parses
 * correctly and the code can set itself up.  It is annoying if the job
 * is sitting in the queue for a long time and when it finally executes
 * it immediately terminates because something is missing from the ini
 * file.  The code uses minimal memory during the setup, so it can be
 * safely executed on the head node and does not need the full MPI
 * environment.
 *
 * @subsection g9pMainSynopsisOptionsInitOnly --initOnly
 *
 * This is somewhat similar to the previous options: the code will now
 * continue to the initialisation phase (which mainly includes the
 * normalisation of the power spectrum and the calculation of a few
 * interesting quantities).  Then the code terminates.
 *
 * No large scale memory allocation is done during the initialisation
 * phase, thus the code can safely be executed on the head node.  This
 * is of interest to play around with different settings of the starting
 * redshift, or normalisation methods.
 *
 * @section g9pMainSynopsisSeeAlso See Also
 *
 * The actual parsing of the command line parameters is done in
 * src/ginnungagap/main.c, please look at the documentation of this
 * file for the latest information on the details of the invocation.
 */
