// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include "partDesc.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "partDesc_adt.h"


/*--- Local defines -----------------------------------------------------*/
// Gives the maximum number of variables that may be attached.  This is
// simply a sanity number, you can safely increase this value if you
// ever feel the need to do so without impacting the code.
#define LOCAL_MAXNUMVARS 256


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern partDesc_t
partDesc_new(const char *partDescName, int partDescID, int hintNumVars)
{
	partDesc_t desc;

	assert(partDescName != NULL);
	assert(hintNumVars < LOCAL_MAXNUMVARS);

	hintNumVars      = hintNumVars < 0 ? 0 : hintNumVars;

	desc             = xmalloc(sizeof(struct partDesc_struct));
	desc->name       = xstrdup(partDescName);
	desc->partDescID = partDescID;
	desc->vars       = varArr_new(hintNumVars);
	desc->isLocked   = false;

	refCounter_init(&(desc->refCounter));

	return partDesc_getRef(desc);
}

extern partDesc_t
partDesc_getRef(partDesc_t desc)
{
	assert(desc != NULL);

	refCounter_ref(&(desc->refCounter));

	return desc;
}

extern void
partDesc_del(partDesc_t *desc)
{
	assert(desc != NULL && *desc != NULL);

	if (refCounter_deref(&((*desc)->refCounter))) {
		int arrLength;
		if ((*desc)->name != NULL)
			xfree((*desc)->name);
		while ((arrLength = varArr_getLength((*desc)->vars)) > 0) {
			gridVar_t var;
			var = varArr_remove((*desc)->vars, arrLength - 1);
			gridVar_del(&var);
		}
		varArr_del(&((*desc)->vars));
		xfree(*desc);
		*desc = NULL;
	}
}

extern int
partDesc_addVar(partDesc_t desc, gridVar_t var)
{
	assert(desc != NULL);
	assert(var != NULL);
	assert(!partDesc_isLocked(desc));
	assert(varArr_getLength(desc->vars) < LOCAL_MAXNUMVARS);

	return varArr_insert(desc->vars, gridVar_getRef(var));
}

extern void
partDesc_lock(partDesc_t desc)
{
	assert(desc != NULL);
	assert(!partDesc_isLocked(desc));

	desc->isLocked = true;
}

extern bool
partDesc_isLocked(const partDesc_t desc)
{
	assert(desc != NULL);

	return desc->isLocked ? true : false;
}

extern char *
partDesc_getNameHandle(const partDesc_t desc)
{
	assert(desc != NULL);

	return desc->name;
}

extern int
partDesc_getPartDescID(const partDesc_t desc)
{
	assert(desc != NULL);

	return desc->partDescID;
}

extern int
partDesc_getNumVars(const partDesc_t desc)
{
	assert(desc != NULL);

	return varArr_getLength(desc->vars);
}

extern gridVar_t
partDesc_getVarHandle(const partDesc_t desc, int varNum)
{
	assert(desc != NULL);
	assert((varNum >= 0) && (varNum < partDesc_getNumVars(desc)));

	return varArr_getElementHandle(desc->vars, varNum);
}

/*--- Implementations of local functions --------------------------------*/
