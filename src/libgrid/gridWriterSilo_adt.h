// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERSILO_ADT_H
#define GRIDWRITERSILO_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterSilo_adt.h
 * @ingroup  libgridIOOutSilo
 * @brief  Implements the main structure for the Silo writer object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter_adt.h"
#include <silo.h>
#ifdef WITH_MPI
#  include <pmpio.h>
#endif


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure. */
struct gridWriterSilo_struct {
	/** @brief  The base structure. */
	struct gridWriter_struct base;
	/** @brief  The underlying file type. */
	int                      dbType;
	/** @brief  The number of files the data will be spread over. */
	int                      numFiles;
	/** @brief  Holds the file pointer used for IO. */
	DBfile                   *f;
	/** @brief  Stores the current directory name. */
	char                     *dirName;
#ifdef WITH_MPI
	/** @brief  The handle used to negotiate exclusive file access. */
	PMPIO_baton_t *baton;
	/** @brief  The rank of the group this writer is in. */
	int           groupRank;
	/** @brief  The rank of this writer in its group. */
	int           rankInGroup;
	/** @brief  The global rank of the process. */
	int           globalRank;
#endif
};


/*--- Prototypes of protected functions ---------------------------------*/

/**
 * @name  Creating and Deleting (Protected, Final)
 *
 * Those are the functions that are only available from within the basic
 * writer and the ones that inherit from the basic writer (the OO equivalent
 * would be @a protected).
 *
 * @{
 */

/**
 * @brief  Allocates memory for a Silo grid writer.
 *
 * @return  Returns a handle to a new (uninitialized) Silo writer structure.
 */
extern gridWriterSilo_t
gridWriterSilo_alloc(void);


/**
 * @brief  Sets all required fields of the Silo writer structure to safe
 *         initial values.
 *
 * @param[in,out]  writer
 *                    The writer to initialize.  This must be a valid writer
 *                    object.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterSilo_init(gridWriterSilo_t writer);


/**
 * @brief  Frees all members of the Silo writer structure.
 *
 * @param[in,out]  writer
 *                    The writer to work with,  This must be a valid writer,
 *                    passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterSilo_free(gridWriterSilo_t writer);


#endif
