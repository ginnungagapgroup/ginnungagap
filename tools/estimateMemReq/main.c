// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include "estimateMemReq.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "../../src/libutil/cmdline.h"
#ifdef ENABLE_DEBUG
#  include "../../src/libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/
#define THIS_PROGNAME "estimateMemReq"


/*--- Local variables ---------------------------------------------------*/
static int    localDim1D             = 0;
static int    localNpTot             = 0;
static int    localNpY               = 0;
static int    localNpZ               = 0;
static size_t localMemPerProcInBytes = 0;
static int    localProcessesPerNode  = 0;
static bool   localIsDouble          = false;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);

static void
local_registerCleanUpFunctions(void);

static estimateMemReq_t
local_getEmr(void);

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
	estimateMemReq_t emr;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	emr = local_getEmr();
	estimateMemReq_run(emr, localNpTot, localNpY, localNpZ,
	                   localMemPerProcInBytes, localProcessesPerNode);
	estimateMemReq_del(&emr);

	return EXIT_SUCCESS;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv)
{
	cmdline_t cmdline;
	double    tmp;

	cmdline = local_cmdlineSetup();
	cmdline_parse(cmdline, *argc, *argv);
	local_checkForPrematureTermination(cmdline);

	if (cmdline_checkOptSetByNum(cmdline, 2))
		cmdline_getOptValueByNum(cmdline, 2, &localNpTot);
	if (cmdline_checkOptSetByNum(cmdline, 3))
		cmdline_getOptValueByNum(cmdline, 3, &localNpY);
	if (cmdline_checkOptSetByNum(cmdline, 4))
		cmdline_getOptValueByNum(cmdline, 4, &localNpZ);
	if (cmdline_checkOptSetByNum(cmdline, 5)) {
		cmdline_getOptValueByNum(cmdline, 5, &tmp);
		localMemPerProcInBytes = (size_t)(tmp * 1024 * 1024);
	}
	if (cmdline_checkOptSetByNum(cmdline, 6))
		cmdline_getOptValueByNum(cmdline, 6, &localProcessesPerNode);
	localIsDouble = cmdline_checkOptSetByNum(cmdline, 7);
	cmdline_getArgValueByNum(cmdline, 0, &localDim1D);
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

	cmdline = cmdline_new(1, 8, THIS_PROGNAME);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "nptot",
	                     "The total number of MPI processes.",
	                     true, CMDLINE_TYPE_INT);
	(void)cmdline_addOpt(cmdline, "npy",
	                     "Number of MPI processes in the y-direction.",
	                     true, CMDLINE_TYPE_INT);
	(void)cmdline_addOpt(cmdline, "npz",
	                     "Number of MPI processes in the z-direction.",
	                     true, CMDLINE_TYPE_INT);
	(void)cmdline_addOpt(cmdline, "mem",
	                     "Amount of memory available per (MPI) process "
	                     "in MiB.",
	                     true, CMDLINE_TYPE_DOUBLE);
	(void)cmdline_addOpt(cmdline, "ppn",
	                     "Number of processors per node.",
	                     true, CMDLINE_TYPE_INT);
	(void)cmdline_addOpt(cmdline, "double",
	                     "Use if you want to use double instead of float "
	                     "for the grid.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "The dimensions of the grid.",
	                     CMDLINE_TYPE_INT);

	return cmdline;
} /* local_cmdlineSetup */

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

static estimateMemReq_t
local_getEmr(void)
{
	estimateMemReq_t emr;

	emr = estimateMemReq_new(localDim1D, localIsDouble);

	return emr;
}
