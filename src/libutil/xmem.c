// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/xmem.c
 * @ingroup libutilCore
 * @brief This file implements the utility functions dealing with
 *        memory.
 */


/*--- Includes ----------------------------------------------------------*/
#include "xmem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>


/*--- Implementation of exported variables ------------------------------*/
size_t  global_allocated_bytes     = 0;
size_t  global_max_allocated_bytes = 0;
int64_t global_malloc_vs_free      = 0;


/*--- Implementation of exported functions ------------------------------*/
extern void *
xmalloc(size_t size)
{
	void *dummy;

#ifdef XMEM_TRACK_MEM
	dummy = malloc(size + 8);
#else
	dummy = malloc(size);
#endif
	if (dummy == NULL) {
		fprintf(stderr, "Could not allocate ");
		if (size < 1024) {
			fprintf(stderr, "%i b\n", (int)size);
		} else if (size < 1048576) {
			fprintf(stderr, "%.2f Kib\n", size / 1024.);
		} else if (size < 1073741824) {
			fprintf(stderr, "%.2f Mib\n", size / 1048576.);
		} else {
			fprintf(stderr, "%.2f Gib\n", size / 1073741824.);
		}
#ifdef XMEM_TRACK_MEM
		xmem_info(stderr);
#endif
		fprintf(stderr, "Aborting... :-(\n");
		abort();
	}

#ifdef XMEM_TRACK_MEM
	global_allocated_bytes += size;
	global_malloc_vs_free++;
	if (global_allocated_bytes > global_max_allocated_bytes) {
		global_max_allocated_bytes = global_allocated_bytes;
	}
	*((uint64_t *)dummy) = (uint64_t)size;
	dummy                = (void *)(((uint64_t *)dummy) + 1);
#endif

	return dummy;
} /* xmalloc */

extern void
xfree(void *ptr)
{
#ifdef XMEM_TRACK_MEM
	if (ptr == NULL)
		return;

	if (global_malloc_vs_free <= 0) {
		fprintf(stderr, "Calling free too often.\n");
		xmem_info(stderr);
		abort();
	}
	global_allocated_bytes -= *(((uint64_t *)ptr) - 1);
	global_malloc_vs_free--;
	free(((uint64_t *)ptr) - 1);
#else
	free(ptr);
#endif

	return;
}

extern void *
xrealloc(void *ptr, size_t size)
{
	void     *dummy;
#ifdef XMEM_TRACK_MEM
	uint64_t old_size;
#endif

	if (ptr == NULL) {
		dummy = xmalloc(size);
		return dummy;
	}

#ifdef XMEM_TRACK_MEM
	dummy = realloc(((uint64_t *)ptr) - 1, size + 8);
#else
	dummy = realloc(ptr, size);
#endif
	if (dummy == NULL) {
		fprintf(stderr, "Could not re-allocate ");
		if (size < 1024) {
			fprintf(stderr, "%i b\n", (int)size);
		} else if (size < 1048576) {
			fprintf(stderr, "%f Kib\n", size / 1024.);
		} else if (size < 1073741824) {
			fprintf(stderr, "%f Mib\n", size / 1048576.);
		} else {
			fprintf(stderr, "%.2f Gib\n", size / 1073741824.);
		}
#ifdef XMEM_TRACK_MEM
		xmem_info(stderr);
#endif
		fprintf(stderr, "Aborting... :-(\n");
		abort();
	}

#ifdef XMEM_TRACK_MEM
	old_size                = *((uint64_t *)dummy);
	global_allocated_bytes -= old_size;
	global_allocated_bytes += size;
	if (global_allocated_bytes > global_max_allocated_bytes) {
		global_max_allocated_bytes = global_allocated_bytes;
	}
	*((uint64_t *)dummy) = (uint64_t)size;
	dummy                = (void *)(((uint64_t *)dummy) + 1);
#endif

	return dummy;
} /* xrealloc */

#ifdef XMEM_TRACK_MEM
void
xmem_info(FILE *f)
{
	fprintf(f, "Currently holding: ");
	if (global_allocated_bytes < 1024) {
		fprintf(f, "%i B\n", (int)global_allocated_bytes);
	} else if (global_allocated_bytes < 1048576) {
		fprintf(f, "%.2f KiB\n", global_allocated_bytes / 1024.);
	} else if (global_allocated_bytes < 1073741824) {
		fprintf(f, "%.2f MiB\n", global_allocated_bytes / 1048576.);
	} else {
		fprintf(stderr, "%.2f GiB\n",
		        global_allocated_bytes / 1073741824.);
	}
	fprintf(f, "Peak usage: ");
	if (global_max_allocated_bytes < 1024) {
		fprintf(f, "%i B\n", (int)global_max_allocated_bytes);
	} else if (global_max_allocated_bytes < 1048576) {
		fprintf(f, "%.2f kiB\n", global_max_allocated_bytes / 1024.);
	} else if (global_max_allocated_bytes < 1073741824) {
		fprintf(f, "%.2f MiB\n", global_max_allocated_bytes / 1048576.);
	} else {
		fprintf(stderr, "%.2f GiB\n",
		        global_max_allocated_bytes / 1073741824.);
	}
	fprintf(f, "Malloc vs. free balance: %" PRIi64 "\n",
	        global_malloc_vs_free);

	return;
}

#endif
