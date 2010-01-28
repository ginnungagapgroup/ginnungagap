// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include "ginnungagap.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"
#include "../libutil/cmdline.h"


/*--- Local variables ---------------------------------------------------*/
char *localIniFname = NULL;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);

static void
local_cleanEnvironment();

static cmdline_t
local_cmdlineSetup(void);

static void
local_checkForPrematureTermination(cmdline_t cmdline);

static void
local_finalMessage(void);


/*--- M A I N -----------------------------------------------------------*/
int
main(int argc, char **argv)
{
	ginnungagap_t ginnungagap;

	local_initEnvironment(&argc, &argv);

	ginnungagap = ginnungagap_new(localIniFname, 0);
	ginnungagap_run(ginnungagap);
	ginnungagap_del(&ginnungagap);

	local_cleanEnvironment();

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
	cmdline_getArgValueByNum(cmdline, 0, &localIniFname);
	cmdline_del(&cmdline);

	atexit(&local_finalMessage);
}

static void
local_cleanEnvironment()
{
	xfree(localIniFname);
#ifdef WITH_MPI
	MPI_Finalize();
#endif
}

static cmdline_t
local_cmdlineSetup(void)
{
	cmdline_t cmdline;
	cmdline       = cmdline_new(1, 2, PACKAGE_NAME);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "An ini file containing the configuration.",
	                     CMDLINE_TYPE_STRING);
	return cmdline;
}

static void
local_checkForPrematureTermination(cmdline_t cmdline)
{
	// This relies on the knowledge of which number is which option!
	// Not nice style, but the respective calls are directly above.
	if (cmdline_checkOptSetByNum(cmdline, 0)) {
		PRINT_VERSION_INFO(stdout);
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

static void
local_finalMessage(void)
{
#ifdef XMEM_TRACK_MEM
	printf("\n");
	xmem_info(stdout);
	printf("\n");
#endif
	printf("Vertu sæl/sæll...\n");
}
