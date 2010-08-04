// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include "grafic2gadget.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/cmdline.h"


/*--- Local defines -----------------------------------------------------*/
#define THIS_PROGNAME "grafic2gadget"


/*--- Local variables ---------------------------------------------------*/
static char *localGraficFileNameVx = NULL;
static char *localGraficFileNameVy = NULL;
static char *localGraficFileNameVz = NULL;
static char *localOutputFileStem   = NULL;
static int  localNumGadgetFiles    = 1;
static bool localForce             = false;


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv);

static void
local_registerCleanUpFunctions(void);

static grafic2gadget_t
local_getG2g(void);

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
	grafic2gadget_t g2g;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	g2g = local_getG2g();
	grafic2gadget_run(g2g);
	grafic2gadget_del(&g2g);

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
	localForce = cmdline_checkOptSetByNum(cmdline, 2);
	cmdline_getArgValueByNum(cmdline, 0, &localGraficFileNameVx);
	cmdline_getArgValueByNum(cmdline, 1, &localGraficFileNameVy);
	cmdline_getArgValueByNum(cmdline, 2, &localGraficFileNameVz);
	cmdline_getArgValueByNum(cmdline, 3, &localOutputFileStem);
	if (cmdline_checkOptSetByNum(cmdline, 3))
		cmdline_getOptValueByNum(cmdline, 3, &localNumGadgetFiles);
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
	xfree(localGraficFileNameVx);
	xfree(localGraficFileNameVy);
	xfree(localGraficFileNameVz);
	xfree(localOutputFileStem);
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

	cmdline = cmdline_new(4, 5, THIS_PROGNAME);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "force",
	                     "Use this to overwrite the output file.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "numFiles",
	                     "Number of output files (defaults to 1).",
	                     true, CMDLINE_TYPE_INT);
	(void)cmdline_addArg(cmdline, "The grafic file containing vx.",
	                     CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(cmdline, "The grafic file containing vy.",
	                     CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(cmdline, "The grafic file containing vz.",
	                     CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(cmdline,
	                     "The stem for the resulting files (.gadget "
	                     "will be appended).",
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

static grafic2gadget_t
local_getG2g(void)
{
	grafic2gadget_t g2g;

	g2g = grafic2gadget_new(localGraficFileNameVx,
	                        localGraficFileNameVy,
	                        localGraficFileNameVz,
	                        localOutputFileStem,
	                        localNumGadgetFiles,
	                        localForce);

	return g2g;
}
