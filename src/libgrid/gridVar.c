// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVar.h"
#include <assert.h>
#include "../libutil/refCounter.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridVar_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridVar_t
gridVar_new(const char *name, gridVarType_t type, int numComponents)
{
	gridVar_t gridVar;

	assert(name != NULL);
	assert(numComponents > 0);

	gridVar                = xmalloc(sizeof(struct gridVar_struct));
	gridVar->name          = xstrdup(name);
	gridVar->type          = type;
	gridVar->numComponents = numComponents;

	refCounter_init(&(gridVar->refCounter));

	return gridVar_getRef(gridVar);
}

extern void
gridVar_del(gridVar_t *var)
{
	assert(var != NULL && *var != NULL);

	if (refCounter_deref(&((*var)->refCounter))) {
		xfree((*var)->name);
		xfree(*var);

		*var = NULL;
	}
}

extern gridVar_t
gridVar_getRef(gridVar_t var)
{
	assert(var != NULL);

	refCounter_ref(&(var->refCounter));

	return var;
}

extern size_t
gridVar_getSizePerElement(gridVar_t var)
{
	assert(var != NULL);
	return gridVarType_sizeof(var->type) * var->numComponents;
}

extern int
gridVar_getNumComponents(gridVar_t var)
{
	assert(var != NULL);
	return var->numComponents;
}

extern gridVarType_t
gridVar_getType(gridVar_t var)
{
	assert(var != NULL);
	return var->type;
}

extern char *
gridVar_getName(gridVar_t var)
{
	assert(var != NULL);
	return var->name;
}

extern void *
gridVar_getMemory(gridVar_t var, uint64_t numElements)
{
	return xmalloc(gridVar_getSizePerElement(var) * numElements);
}

extern void
gridVar_freeMemory(gridVar_t var, void *data)
{
	assert(var != NULL);

	if (data != NULL) {
		xfree(data);
	}
}

/*--- Implementations of local functions --------------------------------*/
