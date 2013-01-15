// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file lilbg9p/g9pDataStore.c
 * @ingroup  libg9pDatastore
 * @brief  Provides the implementation of the data store.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pDataStore.h"
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"


/*--- Implementation of main structure ----------------------------------*/
#include "g9pDataStore_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static g9pDataStore_t
local_allocate(void);


/*--- Implementations of exported functions -----------------------------*/
extern g9pDataStore_t
g9pDataStore_new(g9pHierarchy_t h, const char *name, const char *path)
{
	assert(h != NULL);
	assert(name != NULL);
	assert(name[0] != '\0');

	g9pDataStore_t ds = local_allocate();

	ds->hierarchy = h;
	ds->name      = xstrdup(name);
	ds->path      = path == NULL ? xstrdup("") : xstrdup(path);
	if ((ds->path[0] != '\0') && (ds->path[strlen(ds->path) - 1] != '/')) {
		char *tmp = xstrmerge(ds->path, "/");
		xfree(ds->path);
		ds->path = tmp;
	}

	return g9pDataStore_ref(ds);
}

extern g9pDataStore_t
g9pDataStore_ref(g9pDataStore_t ds)
{
	refCounter_ref(&(ds->refCounter));

	return ds;
}

extern void
g9pDataStore_del(g9pDataStore_t *ds)
{
	assert(ds != NULL && *ds != NULL);

	if (refCounter_deref(&((*ds)->refCounter))) {
		g9pHierarchy_del(&((*ds)->hierarchy));
		xfree((*ds)->name);
		xfree((*ds)->path);
		xfree(*ds);
	}

	*ds = NULL;
}

extern char *
g9pDataStore_getDir(const g9pDataStore_t ds, uint8_t level)
{
	char buf[32];
	snprintf(buf, 32, "%02" PRIu8 "_%" PRIu32 "/",
	         level, g9pHierarchy_getDim1DAtLevel(ds->hierarchy, level));

	return xstrdup(buf);
}

extern filename_t
g9pDataStore_getFileName(const g9pDataStore_t ds,
                         uint8_t              level,
                         g9pFieldID_t         fid)
{
	assert(ds != NULL);
	assert(level < g9pHierarchy_getNumLevels(ds->hierarchy));

	char       *dir  = g9pDataStore_getDir(ds, level);
	char       *path = xstrmerge(ds->path, dir);

	filename_t fn;

	char *tmp = xstrmerge("_", g9pFieldID_getStr(fid));
	fn = filename_newFull(path, ds->name, tmp, NULL);

	xfree(tmp);
	xfree(path);
	xfree(dir);

	return fn;
}

/*--- Implementations of local functions --------------------------------*/
static g9pDataStore_t
local_allocate(void)
{
	g9pDataStore_t ds = xmalloc(sizeof(struct g9pDataStore_struct));

	refCounter_init(&(ds->refCounter));
	ds->hierarchy = NULL;
	ds->path      = NULL;

	return ds;
}
