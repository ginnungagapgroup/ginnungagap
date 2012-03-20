// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERGRAFIC_ADT_H
#define GRIDWRITERGRAFIC_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterGrafic_adt.h
 * @ingroup  libgridIOOutGrafic
 * @brief  Implements the main structure for the Grafic writer object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter_adt.h"
#include <stdbool.h>
#ifdef WITH_MPI
#  include <mpi.h>
#  include "../libutil/groupi.h"
#endif
#include "../libutil/grafic.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure. */
struct gridWriterGrafic_struct {
	/** @brief  The base structure. */
	struct gridWriter_struct base;
	/** @brief  The low level Grafic interface. */
	grafic_t                 grafic;
#ifdef WITH_MPI
	/** @brief  Provides a Poor-Man Parallel IO interface. */
	groupi_t groupi;
#endif
};

/*--- Prototypes of protected functions ---------------------------------*/

/**
 * @name  Creating and Deleting (Protected)
 *
 * Those are the functions that are only available from within the basic
 * writer and the ones that inherit from the basic writer (the OO equivalent
 * would be @a protected).
 *
 * @{
 */

/**
 * @brief  Allocates memory for a Grafic grid writer.
 *
 * @return  Returns a handle to a new (uninitialized) Grafic writer structure.
 */
extern gridWriterGrafic_t
gridWriterGrafic_alloc();


/**
 * @brief  Sets all required fields of the Grafic writer structure to safe
 *         initial values.
 *
 * @param[in,out]  writer
 *                    The writer to initialize.  This must be a valid writer
 *                    object.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterGrafic_init(gridWriterGrafic_t writer);


/**
 * @brief  Frees all members of the Grafic writer structure.
 *
 * @param[in,out]  writer
 *                    The writer to work with,  This must be a valid writer,
 *                    passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterGrafic_free(gridWriterGrafic_t writer);


/** @} */


#endif
