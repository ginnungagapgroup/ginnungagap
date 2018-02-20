// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITER_ADT_H
#define GRIDWRITER_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriter_adt.h
 * @ingroup libgridIOOut
 * @brief  This file implements the main grid writer structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include <stdbool.h>
#include <assert.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "gridIO.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "../libutil/filename.h"


/*--- Callback types ----------------------------------------------------*/

/**
 * @brief  The signature of the function that is implementing the delete
 *         funcion.
 */
typedef void
(*gridWriter_delFunc_t)(gridWriter_t *writer);

/**
 * @brief  The signature of the function that implements the activation of
 *         the writer.
 */
typedef void
(*gridWriter_activateFunc_t)(gridWriter_t writer);

/**
 * @brief  The signature of the function that implements the deactivation
 *         of the writer.
 */
typedef void
(*gridWriter_deactivateFunc_t)(gridWriter_t writer);

/**
 * @brief  The signature of the function that implements the writing of a
 *         patch.
 */
typedef void
(*gridWriter_writeGridPatchFunc_t)(gridWriter_t   writer,
                                   gridPatch_t    patch,
                                   const char     *patchName,
                                   gridPointDbl_t origin,
                                   gridPointDbl_t delta);

/**
 * @brief  The signature of the function that implements the writing of a
 *         grid.
 */
typedef void
(*gridWriter_writeGridRegularFunc_t)(gridWriter_t  writer,
                                     gridRegular_t grid);

#ifdef WITH_MPI

/**
 * @brief  The signature of the function that implements the additionally
 *         required parallel setup.
 */
typedef void
(*gridWriter_initParallelFunc_t)(gridWriter_t writer, MPI_Comm mpiComm);
#endif

/*--- Internal structures -----------------------------------------------*/

/** @brief  Provides the function table. */
struct gridWriter_func_struct {
	/** @brief The function used to delete the writer object. */
	gridWriter_delFunc_t              del;
	/** @brief The function used to actiavte the writer. */
	gridWriter_activateFunc_t         activate;
	/** @brief The function used to deactivate the writer. */
	gridWriter_deactivateFunc_t       deactivate;
	/** @brief The function used to write a patch. */
	gridWriter_writeGridPatchFunc_t   writeGridPatch;
	/** @brief The function used to write a grid. */
	gridWriter_writeGridRegularFunc_t writeGridRegular;
#ifdef WITH_MPI
	/** @brief The function used to initialize the MPI information. */
	gridWriter_initParallelFunc_t initParallel;
#endif
};

/** @brief  Provides a short name for the function table. */
typedef struct gridWriter_func_struct *gridWriter_func_t;


/*--- ADT implementation ------------------------------------------------*/

/**
 * @brief  Implements the main structure for a grid Writer.
 */
struct gridWriter_struct {
	/** @brief  Gives the type this writer is for. */
	gridIO_type_t     type;
	/** @brief  Provides the function table of the actual functions. */
	gridWriter_func_t func;
	/** @brief  Keeps track of whether the writer is active of not. */
	bool              isActive;
	/** @brief  Tracks if #gridWriter_activate() has already been called. */
	bool              hasBeenActivated;
	/** @brief  Decides what to do if the target already exists. */
	bool              overwriteFileIfExists;
	/** @brief  Holds the file name object for the file. */
	filename_t        fileName;
	/** @brief	Gives region to write dims. */
	gridPointUint32_t	 rtwDims;
	/** @brief	Toggles region to write patch. */
	bool		 doPatch;
	/** @brief	Gives region to write idxLo. */
	int32_t	 rtwLo[3];
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
 * @brief  Sets all required fields of the basic writer structure.
 *
 * @param[in,out]  writer
 *                    The writer to initialize.  This must be a valid writer
 *                    object.  Passing @c NULL is undefined.
 * @param[in]      type
 *                    The type of the writer.
 * @param[in]      func
 *                    The function table that holds the implementations of
 *                    the virtual functions.
 *
 *
 * @return  Returns nothing.
 */
extern void
gridWriter_init(gridWriter_t      writer,
                gridIO_type_t     type,
                gridWriter_func_t func);


/**
 * @brief  This will free the members of the basic structure.
 *
 * @param[in,out]  writer
 *                    The writer to work with.  This must be a valid writer
 *                    object, passing @c NULL is undefined.
 */
extern void
gridWriter_free(gridWriter_t writer);


/** @} */

/**
 * @name  Using (Protected)
 *
 * @{
 */

/**
 * @brief  Set the active status to @c true.
 *
 * @param[in,out]  writer
 *                    The writer to set to active, passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
inline static void
gridWriter_setIsActive(gridWriter_t writer)
{
	assert(writer != NULL);

	writer->isActive = true;
}

/**
 * @brief  Set the active status to @c false.
 *
 * @param[in,out]  writer
 *                    The writer to set to active, passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
inline static void
gridWriter_setIsInactive(gridWriter_t writer)
{
	assert(writer != NULL);

	writer->isActive = false;
}

/** @} */


#endif
