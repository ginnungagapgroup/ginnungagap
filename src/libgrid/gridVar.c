// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVar.h"
#include "gridVarTypes.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include <assert.h>


/*--- Implemention of main structure ------------------------------------*/
#include "gridVar_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridVar_t
gridVar_new(const char *varName, gridVarTypes_t type)
{
	gridVar_t var;

	assert(varName != NULL);
	var                 = xmalloc(sizeof(struct gridVar_struct));
	var->name           = xstrdup(varName);
	var->type           = type;
	var->numElements    = 0;
	var->sizePerElement = gridVarTypes_getSizePerElement(type);
	var->data           = NULL;

	return var;
}

extern void
gridVar_del(gridVar_t *var)
{
	assert(var != NULL && *var != NULL);

	xfree((*var)->name);
	if ((*var)->data != NULL)
		xfree((*var)->data);
	xfree(*var);

	*var = NULL;
}

extern void
gridVar_realloc(gridVar_t var, size_t numElements)
{
	assert(var != NULL);
	assert(numElements > 0);

	var->data = xrealloc(var->data, var->sizePerElement * numElements);
	var->numElements = numElements;
}

extern void
gridVar_free(gridVar_t var)
{
	assert(var != NULL);
	assert(var->data != NULL);

	xfree(var->data);
	var->data = NULL;
	var->numElements = 0;
}


/*--- Implementations of local functions --------------------------------*/
