// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#include "parseCmdline.h"
#include "globalVariables.h"
#include "../libutil/cmdline.h"
#include "../../version.h"
#include <stdio.h>
#include <stdlib.h>


int numVersionOpt;
int numHelpOpt;
int numIniFnameArg;


static cmdline_t
setupCmdline(void);

static void
checkForPrematureTermination(cmdline_t cmdline);


extern void
parseCmdline(int argc, char **argv)
{
	cmdline_t cmdline;

	cmdline = setupCmdline();

	cmdline_parse(cmdline, argc, argv);

	checkForPrematureTermination(cmdline);

	cmdline_getArgValueByNum(cmdline, 0, &globalIniFname);

	cmdline_del(&cmdline);
}

static cmdline_t
setupCmdline(void)
{
	cmdline_t cmdline;

	cmdline       = cmdline_new(1, 2, PACKAGE_NAME);
	numVersionOpt = cmdline_addOpt(cmdline, "version",
	                               "This will output a version "
	                               "information.",
	                               false, CMDLINE_TYPE_NONE);
	numHelpOpt = cmdline_addOpt(cmdline, "help",
	                            "This will print this help text.",
	                            false, CMDLINE_TYPE_NONE);
	numIniFnameArg = cmdline_addArg(cmdline,
	                                "An ini file containing the "
	                                "configuration.",
	                                CMDLINE_TYPE_STRING);

	return cmdline;
}

static void
checkForPrematureTermination(cmdline_t cmdline)
{
	if (cmdline_checkOptSetByNum(cmdline, numVersionOpt)) {
		PRINT_VERSION_INFO(stdout);
		cmdline_del(&cmdline);
		exit(EXIT_SUCCESS);
	}

	if (cmdline_checkOptSetByNum(cmdline, numHelpOpt)) {
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
