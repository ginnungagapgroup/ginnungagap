// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICs_impl.c
 * @ingroup  toolsGICSApp
 * @brief  Provides implementation of getter/setter functions.
 *
 * This is in a separate file to not clutter the main generateICs code too
 * much.  The file will be included in generateICs.c and that should be the
 * only place it is ever included.
 */


/*--- Exported function: Setter -----------------------------------------*/
extern void
generateICs_setMode(generateICs_t genics, generateICsMode_t mode)
{
	assert(genics != NULL);

	if (genics->mode != NULL)
		generateICsMode_del( &(genics->mode) );

	genics->mode = mode;
}

extern void
generateICs_setData(generateICs_t genics, generateICsData_t data)
{
	assert(genics != NULL);

	if (genics->data != NULL)
		generateICsData_del( &(genics->data) );

	genics->data = data;
}

extern void
generateICs_setIn(generateICs_t genics, generateICsIn_t in)
{
	assert(genics != NULL);

	if (genics->in != NULL)
		generateICsIn_del( &(genics->in) );

	genics->in = in;
}

extern void
generateICs_setOut(generateICs_t genics, generateICsOut_t out)
{
	assert(genics != NULL);

	if (genics->out != NULL)
		generateICsOut_del( &(genics->out) );

	genics->out = out;
}

extern void
generateICs_setHierarchy(generateICs_t genics, g9pHierarchy_t hierarchy)
{
	assert(genics != NULL);

	if (genics->hierarchy != NULL) {
		fprintf(stderr, "ERROR: The hierarchy can only be set once.\n");
		diediedie(EXIT_FAILURE);
	}

	genics->hierarchy = hierarchy;
}

extern void
generateICs_setDataStore(generateICs_t genics, g9pDataStore_t datastore)
{
	assert(genics != NULL);

	if (genics->datastore != NULL) {
		fprintf(stderr, "ERROR: The datastore can only be set once.\n");
		diediedie(EXIT_FAILURE);
	}

	genics->datastore = datastore;
}

extern void
generateICs_setMask(generateICs_t genics, g9pMask_t mask)
{
	assert(genics != NULL);

	if (genics->mask != NULL) {
		fprintf(stderr, "ERROR: The mask can only be set once.\n");
		diediedie(EXIT_FAILURE);
	}

	genics->mask = mask;
}

extern void
generateICs_setTypes(generateICs_t genics, int32_t *types, int s)
{
	assert(genics != NULL);

	if (genics->typeForLevel != NULL) {
		fprintf(stderr, "ERROR: The types-for-levels can only be set once.\n");
		diediedie(EXIT_FAILURE);
	}

	genics->typeForLevel = xmalloc(sizeof(int32_t)*s);
	for(int i=0; i<s; i++) {
		genics->typeForLevel[i] = types[i];
	}
}

extern void
generateICs_setShift(generateICs_t genics, double* shift)
{
	assert(genics != NULL);
	for (int i=0; i<NDIM; i++) {
		genics->shift[i] = shift[i];
	}
}

/*--- Exported function: Getter -----------------------------------------*/
extern g9pHierarchy_t
generateICs_getHierarchy(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->hierarchy;
}

extern g9pDataStore_t
generateICs_getDataStore(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->datastore;
}

extern g9pMask_t
generateICs_getMask(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->mask;
}

extern void
generateICs_setZoomLevel(generateICs_t genics, int32_t z)
{
	assert(genics != NULL);
	genics->zoomlevel = z;
}

extern uint32_t
generateICs_getMinLevel(generateICs_t genics)
{
	return g9pMask_getMinLevel(genics->mask);
}
extern uint32_t
generateICs_getMaxLevel(generateICs_t genics)
{
	return g9pMask_getMaxLevel(genics->mask);
}

