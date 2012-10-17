// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is pgadget of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  tools/fileTools/gadget_dumpSelection.c
 * @ingroup  fileToolsGADGET
 * @brief  Provides a tool to dump a subset of an GADGET file to the
 *         stdout.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/cmdline.h"
#include "../../src/libutil/gadget.h"
#include "../../src/libutil/stai.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  The stem of the file/the file name if numFiles = 1. */
char *localFileStem = NULL;

/** @brief  This will give the number of pgadgeticles to skip. */
long localPartsSkip = 0;

/** @brief  This will give the number of pgadgeticles to read. */
long localPartsRead = 0;

/** @brief  The number of files used in the set. */
int localNumFiles = 1;

/** @brief  Holds the switch if the units should be converted. */
bool localDoUnitConversion = false;

/** @brief  Stores the name of the program. */
static const char *localProgramName = "gadget_dumpSelection";


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  This will initialize the environment.
 *
 * @param[in,out]  *argc
 *                    Pointer to the external variable holding the
 *                    number of command line arguments.  This number
 *                    might be changed.
 * @param[in,out]  ***argv
 *                    Pointer to the external variable holding the list
 *                    of command line argument.  This list might be
 *                    reordered.
 *
 * @return  Returns nothing.
 */
static void
local_initEnvironment(int *argc, char ***argv);


/**
 * @brief  Makes sure that a set of functions is executed if exit() is
 *         called from somewhere in the code.
 *
 * @return  Returns nothing.
 */
static void
local_registerCleanUpFunctions(void);


/**
 * @brief  This configures the command line parameters expected by the
 *         code.
 *
 * @return  Returns an object that can be used to parse the command line
 *          parameters.
 */
static cmdline_t
local_cmdlineSetup(void);


/**
 * @brief  Checks to command line for switches that would stop the
 *         execution of the code.
 *
 * @return  Returns nothing.
 */
static void
local_checkForPrematureTermination(cmdline_t cmdline);


/**
 * @brief  Frees the memory of the local variables.
 */
static void
local_freeParameterMemory(void);


/**
 * @brief  Prints the last words to stdout before the code returns to
 *         the system.
 */
static void
local_finalMessage(void);


/**
 * @brief  This will make sure that stdout could be closed.
 *
 * This is a sanity check.  If closing of stdout failed, this might
 * indicate very amusing errors, e.g. if the output of the code is
 * redirected to a file and the device the file is stored on runs out of
 * memory, then output will not be stored.  This functions provides a
 * way to notify the user of this problem.
 */
static void
local_verifyCloseOfStdout(void);


/*--- M A I N -----------------------------------------------------------*/

/**
 * @brief  The main function.
 *
 * The function will set-up the environment, read an GADGET header file and
 * pretty print that on the screen.
 *
 * @param[in]  argc
 *               The number of command line arguments.
 * @param[in]  **argv
 *               Array of command line arguments.
 *
 * @return  The function will return EXIT_SUCCESS if everything went
 *          fine or EXIT_FAILURE in the case of errors (which should be
 *          cleanly announced on stderr).
 */
int
main(int argc, char **argv)
{
	gadget_t gadget;
	double   *pos, *vel;
	uint64_t *ids;
	stai_t   dataDescPos, dataDescVel, dataDescIDs;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	gadget = gadget_newSimple(localFileStem, localNumFiles);
	gadget_initForRead(gadget);

	pos         = xmalloc(sizeof(double) * localPartsRead * 3);
	vel         = xmalloc(sizeof(double) * localPartsRead * 3);
	ids         = xmalloc(sizeof(uint64_t) * localPartsRead);
	dataDescPos = stai_new(pos, 3 * sizeof(double), 3 * sizeof(double));
	dataDescVel = stai_new(vel, 3 * sizeof(double), 3 * sizeof(double));
	dataDescIDs = stai_new(ids, sizeof(uint64_t), sizeof(uint64_t));
	gadget_readBlock(gadget, GADGETBLOCK_POS_,
	                 localPartsSkip, localPartsRead, dataDescPos);
	gadget_readBlock(gadget, GADGETBLOCK_VEL_,
	                 localPartsSkip, localPartsRead, dataDescVel);
	gadget_readBlock(gadget, GADGETBLOCK_ID__,
	                 localPartsSkip, localPartsRead, dataDescIDs);

	if (localDoUnitConversion) {
		fprintf(stderr, "Unit conversion is not implemented yet :(\n");
	}

	fprintf(stdout, "#id         \t"
	                "     x      \t     y      \t     z      \t"
	                "     vx     \t     vy     \t     vz     \n");
	for (long i = 0; i < localPartsRead; i++)
		fprintf(stdout,
		        " %8" PRIu64 "\t%12e\t%12e\t%12e\t%12e\t%12e\t%12e\n",
		        ids[i],
		        pos[i * 3 + 0], pos[i * 3 + 1], pos[i * 3 + 2],
		        vel[i * 3 + 0], vel[i * 3 + 1], vel[i * 3 + 2]);

	stai_del(&(dataDescIDs));
	stai_del(&(dataDescVel));
	stai_del(&(dataDescPos));
	xfree(ids);
	xfree(vel);
	xfree(pos);
	gadget_del(&gadget);

	return EXIT_SUCCESS;
} /* main */

/*--- Implementations of local functions --------------------------------*/
static void
local_initEnvironment(int *argc, char ***argv)
{
	cmdline_t cmdline;

	cmdline = local_cmdlineSetup();
	cmdline_parse(cmdline, *argc, *argv);
	local_checkForPrematureTermination(cmdline);

	cmdline_getArgValueByNum(cmdline, 0, &localFileStem);
	cmdline_getArgValueByNum(cmdline, 1, &localPartsSkip);
	cmdline_getArgValueByNum(cmdline, 2, &localPartsRead);

	if (cmdline_checkOptSetByNum(cmdline, 2))
		cmdline_getOptValueByNum(cmdline, 2, &localNumFiles);
	if (cmdline_checkOptSetByNum(cmdline, 3))
		localDoUnitConversion = true;
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
	if (atexit(&local_freeParameterMemory) != 0) {
		fprintf(stderr, "cannot register `%s' as exit function\n",
		        "&local_freeParameterMemory");
		exit(EXIT_FAILURE);
	}
}

static void
local_freeParameterMemory(void)
{
	if (localFileStem != NULL)
		xfree(localFileStem);
}

static void
local_finalMessage(void)
{
#ifdef ENABLE_XMEM_TRACK_MEM
	printf("\n");
	xmem_info(stdout);
	printf("\n");
#endif
	;
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

	cmdline = cmdline_new(3, 4, localProgramName);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "numFiles",
	                     "The number of files.",
	                     true, CMDLINE_TYPE_INT);
	(void)cmdline_addOpt(cmdline, "properUnits",
	                     "Converts from file units to Mpc/h and km/s.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "The (stem of the) file name.",
	                     CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(cmdline,
	                     "The number of particles to skip.",
	                     CMDLINE_TYPE_LONG);
	(void)cmdline_addArg(cmdline,
	                     "The number of particles to read.",
	                     CMDLINE_TYPE_LONG);

	return cmdline;
}

static void
local_checkForPrematureTermination(cmdline_t cmdline)
{
	// This relies on the knowledge of which number is which option!
	// Not nice style, but the respective calls are directly above.
	if (cmdline_checkOptSetByNum(cmdline, 0)) {
		PRINT_VERSION_INFO2(stdout, localProgramName);
		printf("%s", CONFIG_SUMMARY_STRING);
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
 * @defgroup  fileToolsGADGET  Tools for GADGET files
 * @ingroup fileTools
 * @brief  Provides tools to work with GADGET files.
 */
