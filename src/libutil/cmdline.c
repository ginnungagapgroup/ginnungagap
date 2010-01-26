// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#include "cmdline.h"
#include "xmem.h"
#include "xstring.h"
#include <string.h>
#include <ctype.h>


/**
 *  This is the actual implementation of the cmdline ADT structure.
 */
struct cmdline_struct {
	/** This will hold the program name. */
	char *progName;

	/**
	 * The number of arguments and the allocated length of the argument
	 * lists.
	 */
	int  numArgs;
	/** The descriptions of the arguments. */
	char **argDescription;
	/** The data types of the arguments. */
	int  *argType;
	/** The values of the arguments. */
	void **argValue;
	/** A flag indicating whether this argument has been parsed. */
	bool *argParsed;

	/**
	 * This gives the number of arguments actually used in the ADT,
	 * this can be less than numArgs.
	 */
	int numArgsUsed;

	/**
	 * The number of options and the allocated length of the option
	 * lists.
	 */
	int  numOpts;
	/** The name of the option. */
	char **optName;
	/** The description of this option. */
	char **optDescription;
	/** The data types of the arguments. */
	int  *optType;

	/**
	 * A flag indicating whether this option requires a value or
	 * whether it is optional.
	 */
	bool *optValueRequired;
	/** The values of the options. */
	void **optValue;
	/** A flag indicating whether this optiom has been parsed. */
	bool *optParsed;

	/**
	 * This gives the number of options actually used in the ADT,
	 * this can be less than numArgs.
	 */
	int numOptsUsed;
};
typedef struct cmdline_struct cmdline_struct_t;


extern cmdline_t
cmdline_new(int numArgs, int numOpts, const char *progName)
{
	cmdline_t dummy;

	/* Sanity check */
	if (numOpts < 0)
		return NULL;

	/* Get memory */
	dummy = xmalloc(sizeof(cmdline_struct_t));
	if (numArgs > 0) {
		dummy->argDescription = xmalloc(sizeof(char *) * numArgs);
		dummy->argType        = xmalloc(sizeof(int) * numOpts);
		dummy->argValue       = xmalloc(sizeof(void *) * numOpts);
		dummy->argParsed      = xmalloc(sizeof(bool) * numOpts);
	} else {
		dummy->argDescription = NULL;
		dummy->argType        = NULL;
		dummy->argValue       = NULL;
		dummy->argParsed      = NULL;
	}
	if (numOpts > 0) {
		dummy->optName          = xmalloc(sizeof(char *) * numOpts * 2);
		dummy->optDescription   = dummy->optName + numOpts;
		dummy->optValueRequired = xmalloc(sizeof(bool) * numOpts * 2);
		dummy->optType          = xmalloc(sizeof(int) * numOpts);
		dummy->optValue         = xmalloc(sizeof(void *) * numOpts);
		dummy->optParsed        = dummy->optValueRequired + numOpts;
	} else {
		dummy->optName          = NULL;
		dummy->optDescription   = NULL;
		dummy->optValueRequired = NULL;
		dummy->optType          = NULL;
		dummy->optValue         = NULL;
		dummy->optParsed        = NULL;
	}

	/* Set values */
	dummy->progName    = xstrdup(progName);
	dummy->numArgs     = numArgs;
	dummy->numArgsUsed = 0;
	dummy->numOpts     = numOpts;
	dummy->numOptsUsed = 0;
	for (int i = 0; i < numArgs; i++) {
		dummy->argDescription[i] = NULL;
		dummy->argType[i]        = CMDLINE_TYPE_INT;
		dummy->argValue[i]       = NULL;
		dummy->argParsed[i]      = false;
	}
	for (int i = 0; i < numOpts; i++) {
		dummy->optName[i]          = NULL;
		dummy->optDescription[i]   = NULL;
		dummy->optValueRequired[i] = false;
		dummy->optType[i]          = CMDLINE_TYPE_NONE;
		dummy->optValue[i]         = NULL;
		dummy->optParsed[i]        = false;
	}

	/* Done! */
	return dummy;
} /* cmdline_new */

extern void
cmdline_del(cmdline_t *cmdline)
{
	int i;

	/* Sanity check */
	if ((cmdline == NULL) || (*cmdline == NULL))
		return;

	/* Clean up */
	for (i = 0; i < (*cmdline)->numArgsUsed; i++) {
		xfree((*cmdline)->argDescription[i]);
		if (((*cmdline)->argType[i] == CMDLINE_TYPE_STRING)
		    && (*cmdline)->argParsed[i]) {
			xfree(*((char **)((*cmdline)->argValue[i])));
		}
		xfree((*cmdline)->argValue[i]);
	}
	xfree((*cmdline)->argDescription);
	xfree((*cmdline)->argType);
	xfree((*cmdline)->argValue);
	xfree((*cmdline)->argParsed);
	for (int i = 0; i < (*cmdline)->numOptsUsed; i++) {
		xfree((*cmdline)->optName[i]);
		xfree((*cmdline)->optDescription[i]);
		if (((*cmdline)->optType[i] == CMDLINE_TYPE_STRING)
		    && (*cmdline)->optParsed[i]) {
			xfree(*((char **)((*cmdline)->optValue[i])));
		}
		xfree((*cmdline)->optValue[i]);
	}
	xfree((*cmdline)->optName);
	xfree((*cmdline)->optValueRequired);
	xfree((*cmdline)->optType);
	xfree((*cmdline)->optValue);

	xfree((*cmdline)->progName);

	xfree((*cmdline));
	*cmdline = NULL;

	/* Done */
	return;
} /* cmdline_del */

extern int
cmdline_addOpt(cmdline_t  cmdline,
               const char *optName,
               const char *optDescription,
               bool       optValueRequired,
               int        optType)
{
	int optNum = -1;

	/* Sanity check */
	if ((cmdline == NULL) || (optName == NULL)
	    || (optDescription == NULL)) {
#ifdef DEBUG
		fprintf(stderr,
		        "ERROR: Parameter NULL in %s:%s:%i\n",
		        __FILE__, __func__, __LINE__);
#endif
		return -1;
	}
	switch (optType) {
	case CMDLINE_TYPE_NONE:
	case CMDLINE_TYPE_INT:
	case CMDLINE_TYPE_DOUBLE:
	case CMDLINE_TYPE_STRING:
		break;
	default:
#ifdef DEBUG
		fprintf(stderr,
		        "ERROR: Wrong option type in %s:%s:%i\n",
		        __FILE__, __func__, __LINE__);
#endif
		return -1;
	}
	if (cmdline->numOptsUsed >= cmdline->numOpts) {
#ifdef DEBUG
		fprintf(stderr,
		        "ERROR: All options already in use in %s:%s:%i\n",
		        __FILE__, __func__, __LINE__);
#endif
		return -1;
	}

	/* Get the position of this option */
	optNum = cmdline->numOptsUsed;
	cmdline->numOptsUsed++;

	/* Add the option parameters */
	cmdline->optName[optNum]          = xstrdup(optName);
	cmdline->optDescription[optNum]   = xstrdup(optDescription);
	cmdline->optValueRequired[optNum] = optValueRequired;
	cmdline->optType[optNum]          = optType;
	switch (optType) {
	case CMDLINE_TYPE_INT:
		cmdline->optValue[optNum]             = xmalloc(sizeof(int));
		*((int *)(cmdline->optValue[optNum])) = -666;
		break;
	case CMDLINE_TYPE_DOUBLE:
		cmdline->optValue[optNum]                = xmalloc(sizeof(double));
		*((double *)(cmdline->optValue[optNum])) = -1e20;
		break;
	case CMDLINE_TYPE_STRING:
		cmdline->optValue[optNum]               = xmalloc(sizeof(char *));
		*((char **)(cmdline->optValue[optNum])) = NULL;
		break;
	case CMDLINE_TYPE_NONE:
	default:
		break;
	} /* switch */
	cmdline->optParsed[optNum] = false;

	/* Done */
	return optNum;
} /* cmdline_addOpt */

extern int
cmdline_addArg(cmdline_t  cmdline,
               const char *argDescription,
               int        argType)
{
	int argNum = -1;

	/* Sanity check */
	if ((cmdline == NULL) || (argDescription == NULL)) {
#ifdef DEBUG
		fprintf(stderr,
		        "ERROR: Parameter NULL in %s:%s:%i\n",
		        __FILE__, __func__, __LINE__);
#endif
		return -1;
	}
	switch (argType) {
	case CMDLINE_TYPE_INT:
	case CMDLINE_TYPE_DOUBLE:
	case CMDLINE_TYPE_STRING:
		break;
	case CMDLINE_TYPE_NONE:
	default:
#ifdef DEBUG
		fprintf(stderr,
		        "ERROR: Wrong option type in %s:%s:%i\n",
		        __FILE__, __func__, __LINE__);
#endif
		return -1;
	}
	if (cmdline->numArgsUsed >= cmdline->numArgs) {
#ifdef DEBUG
		fprintf(stderr,
		        "ERROR: All arguments already in use in %s:%s:%i\n",
		        __FILE__, __func__, __LINE__);
#endif
		return -1;
	}

	/* Get the position of this option */
	argNum = cmdline->numArgsUsed;
	cmdline->numArgsUsed++;

	/* Add the argument parameters */
	cmdline->argDescription[argNum] = xstrdup(argDescription);
	cmdline->argType[argNum]        = argType;
	switch (argType) {
	case CMDLINE_TYPE_INT:
		cmdline->argValue[argNum]             = xmalloc(sizeof(int));
		*((int *)(cmdline->argValue[argNum])) = -666;
		break;
	case CMDLINE_TYPE_DOUBLE:
		cmdline->argValue[argNum]                = xmalloc(sizeof(double));
		*((double *)(cmdline->argValue[argNum])) = -1e20;
		break;
	case CMDLINE_TYPE_STRING:
		cmdline->argValue[argNum]               = xmalloc(sizeof(char *));
		*((char **)(cmdline->argValue[argNum])) = NULL;
		break;
	case CMDLINE_TYPE_NONE:
	default:
		break;
	} /* switch */
	cmdline->argParsed[argNum] = false;

	/* Done */
	return argNum;
} /* cmdline_addArg */

extern void
cmdline_printHelp(cmdline_t cmdline,
                  FILE      *fout)
{
	int i;

	/* Sanity check */
	if (cmdline == NULL) {
		return;
	}

	/* Print the usage info */
	fprintf(fout, "Usage: %s", cmdline->progName);
	for (i = 0; i < cmdline->numOptsUsed; i++) {
		fprintf(fout, " --%s", cmdline->optName[i]);
		if (cmdline->optType[i] != CMDLINE_TYPE_NONE) {
			if (!cmdline->optValueRequired[i])
				fprintf(fout, " [<value>]");
			else
				fprintf(fout, " <value>");
		}
	}
	for (i = 0; i < cmdline->numArgsUsed; i++) {
		fprintf(fout, " arg%i", i);
	}
	fprintf(fout, "\n\n");

	/* Describe the arguments */
	if (cmdline->numArgsUsed > 0) {
		fprintf(fout, "Meaning of the arguments:\n");
		for (i = 0; i < cmdline->numArgsUsed; i++) {
			fprintf(fout, "  arg%i", i);
			switch (cmdline->argType[i]) {
			case CMDLINE_TYPE_INT:
				fprintf(fout, "  (integer)\n");
				break;
			case CMDLINE_TYPE_DOUBLE:
				fprintf(fout, "  (double)\n");
				break;
			case CMDLINE_TYPE_STRING:
				fprintf(fout, "  (string)\n");
				break;
			}
			fprintf(fout, "    %s\n", cmdline->argDescription[i]);
		}
		fprintf(fout, "\n");
	}

	/* Describe the options */
	if (cmdline->numOptsUsed > 0) {
		fprintf(fout, "Meaning of the options:\n");
		for (i = 0; i < cmdline->numOptsUsed; i++) {
			fprintf(fout, "  --%s", cmdline->optName[i]);
			if (cmdline->optType[i] != CMDLINE_TYPE_NONE) {
				if (!cmdline->optValueRequired[i])
					fprintf(fout, " [");
				else
					fprintf(fout, " ");
				switch (cmdline->optType[i]) {
				case CMDLINE_TYPE_INT:
					fprintf(fout, "<value (integer)>");
					break;
				case CMDLINE_TYPE_DOUBLE:
					fprintf(fout, "<value (double)>");
					break;
				case CMDLINE_TYPE_STRING:
					fprintf(fout, "<value (string)>");
					break;
				}
				if (!cmdline->optValueRequired[i])
					fprintf(fout, "]");
				fprintf(fout, "\n");
			} else {
				fprintf(fout, "\n");
			}

			fprintf(fout, "    %s\n", cmdline->optDescription[i]);
		}
		fprintf(fout, "\n");
	}


	return;
} /* cmdline_printUsage */

extern void
cmdline_parse(cmdline_t cmdline,
              int       argc,
              char      **argv)
{
	int i, j;

	/* Sanity check */
	if (cmdline == NULL)
		return;

	/* Loop over all arguments */
	i = 1;
	while (i < argc) {
		if ((argv[i][0] == '-') && (argv[i][1] == '-')) {
			/* It is an option */
			j = 0;
			while (j < cmdline->numOptsUsed) {
				if (strstr(argv[i] + 2, cmdline->optName[j]) != NULL)
					break;
				j++;
			}
			if (cmdline->optParsed[j] == true) {
				fprintf(stderr,
				        "ERROR:  --%s appears more than once\n",
				        cmdline->optName[j]);
				exit(EXIT_FAILURE);
			}
			i++;
			if (cmdline->optType[j] != CMDLINE_TYPE_NONE) {
				if (cmdline->optValueRequired[j]
				    && ((i == argc)
				        || ((argv[i][0] == '-') && (argv[i][1] == '-')))) {
					fprintf(stderr,
					        "ERROR:  --%s requires an argument\n",
					        cmdline->optName[j]);
					exit(EXIT_FAILURE);
				}
				switch (cmdline->optType[j]) {
				case CMDLINE_TYPE_INT:
					*(int *)(cmdline->optValue[j]) = atoi(argv[i]);
					break;
				case CMDLINE_TYPE_DOUBLE:
					*(double *)(cmdline->optValue[j]) = atof(argv[i]);
					break;
				case CMDLINE_TYPE_STRING:
					*(char **)(cmdline->optValue[j]) = xstrdup(argv[i]);
					break;
				}
				i++;
			}
			cmdline->optParsed[j] = true;
		} else {
			/* It is an argument */
			j = 0;
			while (j < cmdline->numArgsUsed && cmdline->argParsed[j])
				j++;
			if (j >= cmdline->numArgsUsed) {
				fprintf(stderr, "ERROR:  Too many arguments\n");
				exit(EXIT_FAILURE);
			}
			switch (cmdline->argType[j]) {
			case CMDLINE_TYPE_INT:
				*(int *)(cmdline->argValue[j]) = atoi(argv[i]);
				break;
			case CMDLINE_TYPE_DOUBLE:
				*(double *)(cmdline->argValue[j]) = atof(argv[i]);
				break;
			case CMDLINE_TYPE_STRING:
				*(char **)(cmdline->argValue[j]) = xstrdup(argv[i]);
				break;
			}
			cmdline->argParsed[j] = true;
			i++;
		}
	}

	/* Done */
	return;
} /* cmdline_parse */

extern bool
cmdline_verify(cmdline_t cmdline)
{
	if (cmdline == NULL)
		return false;

	for (int i = 0; i < cmdline->numArgsUsed; i++) {
		if (!cmdline->argParsed[i]) {
			fprintf(stderr, "ERROR:  Too few arguments\n");
			return false;
		}
	}

	return true;
}

extern bool
cmdline_checkOptSetByNum(cmdline_t cmdline, int optNum)
{
	if ((cmdline == NULL) || (optNum < 0)
	    || (optNum >= cmdline->numOptsUsed)) {
		return false;
	}

	return cmdline->optParsed[optNum];
}

extern bool
cmdline_getOptValueByNum(cmdline_t cmdline, int optNum, void *val)
{
	if ((cmdline == NULL) || (optNum < 0)
	    || (optNum >= cmdline->numOptsUsed) || (val == NULL)
	    || (cmdline->optType[optNum] == CMDLINE_TYPE_NONE)
	    || (!cmdline->optParsed[optNum])) {
		return false;
	}

	switch (cmdline->optType[optNum]) {
	case CMDLINE_TYPE_INT:
		*(int *)val = *(int *)(cmdline->optValue[optNum]);
		break;
	case CMDLINE_TYPE_DOUBLE:
		*(double *)val = *(double *)(cmdline->optValue[optNum]);
		break;
	case CMDLINE_TYPE_STRING:
		*(char **)val = xstrdup(*(char **)(cmdline->optValue[optNum]));
		break;
	}

	return true;
} /* cmdline_getOptValueByNum */

extern bool
cmdline_checkArgSetByNum(cmdline_t cmdline, int argNum)
{
	if ((cmdline == NULL) || (argNum < 0)
	    || (argNum >= cmdline->numArgsUsed)) {
		return false;
	}

	return cmdline->argParsed[argNum];
}

extern bool
cmdline_getArgValueByNum(cmdline_t cmdline, int argNum, void *val)
{
	if ((cmdline == NULL) || (argNum < 0)
	    || (argNum >= cmdline->numArgsUsed) || (val == NULL)
	    || (!cmdline->argParsed[argNum])) {
		return false;
	}

	switch (cmdline->argType[argNum]) {
	case CMDLINE_TYPE_INT:
		*(int *)val = *(int *)(cmdline->argValue[argNum]);
		break;
	case CMDLINE_TYPE_DOUBLE:
		*(double *)val = *(double *)(cmdline->argValue[argNum]);
		break;
	case CMDLINE_TYPE_STRING:
		*(char **)val = xstrdup(*(char **)(cmdline->argValue[argNum]));
		break;
	}

	return true;
} /* cmdline_getArgValueByNum */
