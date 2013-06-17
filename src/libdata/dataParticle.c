// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataParticle.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "dataParticle_adt.h"


/*--- Local defines -----------------------------------------------------*/
// Gives the maximum number of variables that may be attached.  This is
// simply a sanity number, you can safely increase this value if you
// ever feel the need to do so without impacting the code.
#define LOCAL_MAXNUMVARS 256


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern dataParticle_t
dataParticle_new(const char *dataParticleName,
                 int        dataParticleID,
                 int        hintNumVars)
{
	dataParticle_t desc;

	assert(dataParticleName != NULL);
	assert(hintNumVars < LOCAL_MAXNUMVARS);

	hintNumVars          = hintNumVars < 0 ? 0 : hintNumVars;

	desc                 = xmalloc(sizeof(struct dataParticle_struct));
	desc->name           = xstrdup(dataParticleName);
	desc->dataParticleID = dataParticleID;
	desc->vars           = varArr_new(hintNumVars);
	desc->isLocked       = false;

	refCounter_init(&(desc->refCounter));

	return dataParticle_getRef(desc);
}

extern dataParticle_t
dataParticle_getRef(dataParticle_t desc)
{
	assert(desc != NULL);

	refCounter_ref(&(desc->refCounter));

	return desc;
}

extern void
dataParticle_del(dataParticle_t *desc)
{
	assert(desc != NULL && *desc != NULL);

	if (refCounter_deref(&((*desc)->refCounter))) {
		int arrLength;
		if ((*desc)->name != NULL)
			xfree((*desc)->name);
		while ((arrLength = varArr_getLength((*desc)->vars)) > 0) {
			dataVar_t var;
			var = varArr_remove((*desc)->vars, arrLength - 1);
			dataVar_del(&var);
		}
		varArr_del(&((*desc)->vars));
		xfree(*desc);
	}
	*desc = NULL;
}

extern int
dataParticle_addVar(dataParticle_t desc, dataVar_t var)
{
	assert(desc != NULL);
	assert(var != NULL);
	assert(!dataParticle_isLocked(desc));
	assert(varArr_getLength(desc->vars) < LOCAL_MAXNUMVARS);

	return varArr_insert(desc->vars, var);
}

extern void
dataParticle_lock(dataParticle_t desc)
{
	assert(desc != NULL);
	assert(!dataParticle_isLocked(desc));

	desc->isLocked = true;
}

extern bool
dataParticle_isLocked(const dataParticle_t desc)
{
	assert(desc != NULL);

	return desc->isLocked ? true : false;
}

extern char *
dataParticle_getNameHandle(const dataParticle_t desc)
{
	assert(desc != NULL);

	return desc->name;
}

extern int
dataParticle_getPartDescID(const dataParticle_t desc)
{
	assert(desc != NULL);

	return desc->dataParticleID;
}

extern int
dataParticle_getNumVars(const dataParticle_t desc)
{
	assert(desc != NULL);

	return varArr_getLength(desc->vars);
}

extern dataVar_t
dataParticle_getVarHandle(const dataParticle_t desc, int varNum)
{
	assert(desc != NULL);
	assert((varNum >= 0) && (varNum < dataParticle_getNumVars(desc)));

	return varArr_getElementHandle(desc->vars, varNum);
}

/*--- Implementations of local functions --------------------------------*/
