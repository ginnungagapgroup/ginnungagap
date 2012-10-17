// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  tools/fileTools/art_dumpSelection.c
 * @ingroup  fileToolsART
 * @brief  Provides a tool to dump a subset of an ART file to the
 *         stdout.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/cmdline.h"
#include "../../src/libutil/art.h"
#include "../../src/libutil/stai.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  This will give the number of particles to skip. */
long localPartsSkip = 0;

/** @brief  This will give the number of particles to read. */
long localPartsRead = 0;

/** @brief  This will give the path the files. */
char *localPathToFiles = ART_USE_DEFAULT_PATH;

/** @brief  This will contain the suffix of the files. */
char *localFileNameSuffix = ART_USE_DEFAULT_SUFFIX;

/** @brief  The number of files used in the set. */
int localNumFiles = 1;

/** @brief  Holds the switch if the units should be converted. */
bool localDoUnitConversion = false;


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
 * The function will set-up the environment, read an ART header file and
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
	art_t  art;
	double *data;
	stai_t dataDesc[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	art = art_new(localPathToFiles, localFileNameSuffix, localNumFiles);
	art_attachHeaderFromFile(art);

	data        = xmalloc(sizeof(double) * localPartsRead * 6);
	dataDesc[0] = stai_new(data + 0, sizeof(double), 6 * sizeof(double));
	dataDesc[1] = stai_new(data + 1, sizeof(double), 6 * sizeof(double));
	dataDesc[2] = stai_new(data + 2, sizeof(double), 6 * sizeof(double));
	dataDesc[3] = stai_new(data + 3, sizeof(double), 6 * sizeof(double));
	dataDesc[4] = stai_new(data + 4, sizeof(double), 6 * sizeof(double));
	dataDesc[5] = stai_new(data + 5, sizeof(double), 6 * sizeof(double));
	art_read(art, localPartsSkip, localPartsRead, dataDesc);

	if (localDoUnitConversion) {
		artHeader_t header;
		double      posFac, velFac;

		header = art_getHeaderHandle(art);
		posFac = artHeader_getFactorFilePositionToMpch(header);
		velFac = artHeader_getFactorFileVelocityToKms(header);
		for (long i = 0; i < localPartsRead; i++) {
			data[i * 6 + 0]  = (data[i * 6 + 0] - 1.0) * posFac;
			data[i * 6 + 1]  = (data[i * 6 + 1] - 1.0) * posFac;
			data[i * 6 + 2]  = (data[i * 6 + 2] - 1.0) * posFac;
			data[i * 6 + 3] *= velFac;
			data[i * 6 + 4] *= velFac;
			data[i * 6 + 5] *= velFac;
		}
	}

	fprintf(stdout, "#position\t"
	                "     x      \t     y      \t     z      \t"
	                "     vx     \t     vy     \t     vz     \n");
	for (long i = 0; i < localPartsRead; i++)
		fprintf(stdout, " %8li\t%12e\t%12e\t%12e\t%12e\t%12e\t%12e\n",
		        i + localPartsSkip,
		        data[i * 6 + 0], data[i * 6 + 1], data[i * 6 + 2],
		        data[i * 6 + 3], data[i * 6 + 4], data[i * 6 + 5]);

	xfree(data);
	stai_del(&(dataDesc[0]));
	stai_del(&(dataDesc[1]));
	stai_del(&(dataDesc[2]));
	stai_del(&(dataDesc[3]));
	stai_del(&(dataDesc[4]));
	stai_del(&(dataDesc[5]));
	art_del(&art);

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
	cmdline_getArgValueByNum(cmdline, 0, &localPartsSkip);
	cmdline_getArgValueByNum(cmdline, 1, &localPartsRead);
	if (cmdline_checkOptSetByNum(cmdline, 2))
		cmdline_getOptValueByNum(cmdline, 2, &localPathToFiles);
	if (cmdline_checkOptSetByNum(cmdline, 3))
		cmdline_getOptValueByNum(cmdline, 3, &localFileNameSuffix);
	if (cmdline_checkOptSetByNum(cmdline, 4))
		cmdline_getOptValueByNum(cmdline, 4, &localNumFiles);
	if (cmdline_checkOptSetByNum(cmdline, 5))
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
	if (localPathToFiles != NULL)
		xfree(localPathToFiles);
	if (localFileNameSuffix != NULL)
		xfree(localFileNameSuffix);
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

	cmdline = cmdline_new(2, 6, "art_createTestFile");
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "prefix",
	                     "The prefix of the filenames.",
	                     true, CMDLINE_TYPE_STRING);
	(void)cmdline_addOpt(cmdline, "suffix",
	                     "The suffix of the filenames",
	                     true, CMDLINE_TYPE_STRING);
	(void)cmdline_addOpt(cmdline, "numFiles",
	                     "The number of files.",
	                     true, CMDLINE_TYPE_INT);
	(void)cmdline_addOpt(cmdline, "properUnits",
	                     "Converts from file units to Mpc/h and km/s.",
	                     false, CMDLINE_TYPE_NONE);
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
		PRINT_VERSION_INFO2(stdout, "art_peekHeader");
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
 * @defgroup  fileToolsART  Tools for ART files
 * @ingroup fileTools
 * @brief  Provides tools to work with ART files.
 */
