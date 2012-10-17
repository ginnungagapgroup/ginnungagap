// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  tools/fileTools/cubepm_dumpSelection.c
 * @ingroup  fileToolsCubePM
 * @brief  Provides a tool to dump a subset of an CubePM file to the
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
#include <assert.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/cmdline.h"
#include "../../src/libutil/cubepm.h"
#include "../../src/libutil/stai.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  This will give the path the files. */
char *localPathToFiles = NULL;

/** @brief  This will contain the suffix of the files. */
char *localFileNameStem = NULL;

/** @brief  The number of nodes used to write the set. */
int localNodesDim = 1;

/** @brief  Holds the ngrid value of the file set. */
int localNGrid = 1;

/** @brief  This will give the number of particles to skip. */
long localPartsSkip = 0;

/** @brief  This will give the number of particles to read. */
long localPartsRead = 0;

/** @brief  Holds the boxsize in Mpc/h. */
double localBoxsize = 1.;

/** @brief  Holds Omega0. */
double localOmega0 = 0.3;

/** @brief  Holds OmegaLambda. */
double localLambda0 = 0.7;

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
	cubepm_t cubepm;
	double   *data;
	stai_t   dataDesc[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
	uint64_t actualRead;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	cubepm = cubepm_new(localPathToFiles, localFileNameStem,
	                    localNodesDim, localNGrid);
	cubepm_initHeaderValuesFromFiles(cubepm);
	cubepm_setBoxsizeInMpch(cubepm, localBoxsize);
	cubepm_setOmega0(cubepm, localOmega0);
	cubepm_setLambda0(cubepm, localLambda0);

	data        = xmalloc(sizeof(double) * localPartsRead * 6);
	dataDesc[0] = stai_new(data + 0, sizeof(double), 6 * sizeof(double));
	dataDesc[1] = stai_new(data + 1, sizeof(double), 6 * sizeof(double));
	dataDesc[2] = stai_new(data + 2, sizeof(double), 6 * sizeof(double));
	dataDesc[3] = stai_new(data + 3, sizeof(double), 6 * sizeof(double));
	dataDesc[4] = stai_new(data + 4, sizeof(double), 6 * sizeof(double));
	dataDesc[5] = stai_new(data + 5, sizeof(double), 6 * sizeof(double));
	actualRead  = cubepm_read(cubepm, localPartsSkip, localPartsRead,
	                          dataDesc);
	assert(actualRead == localPartsRead);

	if (localDoUnitConversion) {
		double posFac, velFac;

		posFac = cubepm_calcFactorFilePositionToMpch(cubepm);
		velFac = cubepm_calcFactorFileVelocityToKms(cubepm);
#ifdef WITH_OPENMP
#  pragma omp parallel for
#endif
		for (long i = 0; i < localPartsRead; i++) {
			data[i * 6 + 0]  = data[i * 6 + 0] * posFac;
			data[i * 6 + 1]  = data[i * 6 + 1] * posFac;
			data[i * 6 + 2]  = data[i * 6 + 2] * posFac;
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
	cubepm_del(&cubepm);

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
	if (cmdline_checkOptSetByNum(cmdline, 2))
		cmdline_getOptValueByNum(cmdline, 2, &localBoxsize);
	if (cmdline_checkOptSetByNum(cmdline, 3))
		cmdline_getOptValueByNum(cmdline, 3, &localOmega0);
	if (cmdline_checkOptSetByNum(cmdline, 4))
		cmdline_getOptValueByNum(cmdline, 4, &localLambda0);
	if (cmdline_checkOptSetByNum(cmdline, 5))
		localDoUnitConversion = true;
	cmdline_getArgValueByNum(cmdline, 0, &localPathToFiles);
	cmdline_getArgValueByNum(cmdline, 1, &localFileNameStem);
	cmdline_getArgValueByNum(cmdline, 2, &localNodesDim);
	cmdline_getArgValueByNum(cmdline, 3, &localNGrid);
	cmdline_getArgValueByNum(cmdline, 4, &localPartsSkip);
	cmdline_getArgValueByNum(cmdline, 5, &localPartsRead);
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
	if (localFileNameStem != NULL)
		xfree(localFileNameStem);
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

	cmdline = cmdline_new(6, 6, "cubepm_dumpSelection");
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "boxsize",
	                     "The boxsize in Mpc/h.",
	                     true, CMDLINE_TYPE_DOUBLE);
	(void)cmdline_addOpt(cmdline, "omega0",
	                     "Omega0",
	                     true, CMDLINE_TYPE_DOUBLE);
	(void)cmdline_addOpt(cmdline, "lambda0",
	                     "Lambda0",
	                     true, CMDLINE_TYPE_DOUBLE);
	(void)cmdline_addOpt(cmdline, "properUnits",
	                     "Converts from file units to Mpc/h and km/s.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "The prefix of the filenames.",
	                     CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(cmdline,
	                     "The stem of the filenames",
	                     CMDLINE_TYPE_STRING);
	(void)cmdline_addArg(
	    cmdline,
	    "The number of nodes (1D) used to write the files.",
	    CMDLINE_TYPE_INT);
	(void)cmdline_addArg(cmdline,
	                     "The size of the grid.",
	                     CMDLINE_TYPE_INT);
	(void)cmdline_addArg(cmdline,
	                     "The number of particles to skip.",
	                     CMDLINE_TYPE_LONG);
	(void)cmdline_addArg(cmdline,
	                     "The number of particles to read.",
	                     CMDLINE_TYPE_LONG);

	return cmdline;
} /* local_cmdlineSetup */

static void
local_checkForPrematureTermination(cmdline_t cmdline)
{
	// This relies on the knowledge of which number is which option!
	// Not nice style, but the respective calls are directly above.
	if (cmdline_checkOptSetByNum(cmdline, 0)) {
		PRINT_VERSION_INFO2(stdout, "cubepm_dumpSelection");
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
