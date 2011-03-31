// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef XMEM_H
#define XMEM_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/xmem.h
 * @ingroup libutilCore
 * @brief This file provides utility functions dealing with memory.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef XMEM_TRACK_MEM
#  include <stdint.h>
#endif


/*--- Exported global variables -----------------------------------------*/
#ifdef XMEM_TRACK_MEM
extern size_t  global_allocated_bytes;
extern size_t  global_max_allocated_bytes;
extern int64_t global_malloc_vs_free;
#endif


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * \brief  A wrapper function for malloc, performing a check whether it
 *         succeeded or not. Also provides accounting of allocated
 *         memory if activated via -DTRACK_MEM.
 *
 * This function will abort the program, if not enough memory could be
 * allocated.
 *
 * \param  size  The amount of bytes to allocate.
 *
 * \return  A pointer to the allocated memory region.
 */
extern void *
xmalloc(size_t size);


/**
 * \brief  A wrapper function for free, to allow for memory tracking.
 *
 * If the tracking is activated and xfree is called too often (no
 * preceeding xmalloc), the program will be aborted.
 *
 * \param *ptr  The memory area to be freed.
 *
 * \return Nothing.
 */
extern void
xfree(void *ptr);


/**
 * \brief  A wrapper function for realloc, performing checks for correct
 *         allocation.  Also keeps track of memory usage, if activated.
 *
 * \param  *ptr  The object to resize.
 * \param  size  The new size.
 *
 * \return A pointer to a resized objects.
 */
extern void *
xrealloc(void *ptr, size_t size);


#ifdef XMEM_TRACK_MEM

/**
 * \brief  Will output the current memory usage to a given stream.
 *
 * This function is only available if the memory tracking is activated.
 *
 * \param  *f  The stream to write to.
 *
 * \return Nothing.
 */
void
xmem_info(FILE *f);

#endif


#endif
