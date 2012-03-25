// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  tools/fileTools/gadget_describeFile.c
 * @ingroup  fileToolsGadget
 * @brief  Provides a tool to describe a file.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../../version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xfile.h"
#include "../../src/libutil/cmdline.h"
#include "../../src/libutil/gadgetTOC.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  This will hold the file name. */
char *localFileName = NULL;

/** @brief  Stores the name of the program. */
static const char *localProgramName = "gadget_describeFile";


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
 * The function will set-up the environment, read the TOC of a Gadget file
 * and pretty print that on the screen.
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
	gadgetTOC_t toc;
	FILE *f;

	local_registerCleanUpFunctions();
	local_initEnvironment(&argc, &argv);

	f = xfopen(localFileName, "rb");
	toc = gadgetTOC_newFromFile(f);
	xfclose(&f);

	if (!gadgetTOC_isValid(toc)) {
		fprintf(
		    stderr,
		    "The table of content of the file is invalid. Corrupt file?\n");
	} else {
		printf("%s:\n  File Version: %s\n",
		       localFileName,
		       gadgetVersion_getNameFromType(gadgetTOC_getFileVersion(toc)));
		gadgetTOC_prettyPrint(toc, stdout, NULL);
	}

	gadgetTOC_del(&toc);

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
	cmdline_getArgValueByNum(cmdline, 0, &localFileName);
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
	if (localFileName != NULL)
		xfree(localFileName);
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

	cmdline = cmdline_new(1, 2, localProgramName);
	(void)cmdline_addOpt(cmdline, "version",
	                     "This will output a version information.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addOpt(cmdline, "help",
	                     "This will print this help text.",
	                     false, CMDLINE_TYPE_NONE);
	(void)cmdline_addArg(cmdline,
	                     "The filename.",
	                     CMDLINE_TYPE_STRING);

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
 * @defgroup  fileToolsCubePM  Tools for CubePM files
 * @ingroup fileTools
 * @brief  Provides tools to work with CubePM files.
 */
