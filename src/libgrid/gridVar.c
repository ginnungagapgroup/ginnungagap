// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVar.h"
#include <assert.h>
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

	return gridVar;
}

extern void
gridVar_del(gridVar_t *gridVar)
{
	assert(gridVar != NULL && *gridVar != NULL);

	xfree((*gridVar)->name);
	xfree(*gridVar);

	*gridVar = NULL;
}

extern size_t
gridVar_getSizePerElement(gridVar_t var)
{
	assert(var != NULL);
	return gridVarType_sizeof(var->type) * var->numComponents;
}

/*--- Implementations of local functions --------------------------------*/
