// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADER_ADT_H
#define GRIDREADER_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReader_adt.h
 * @ingroup libgridIOInInterface
 * @brief  Implements the main reader structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include "gridIO.h"
#include "gridPatch.h"
#include "../libutil/filename.h"


/*--- Callback types ----------------------------------------------------*/

/** @brief  The signature of the function that implements the delete.  */
typedef void
(*gridReader_delFunc_t)(gridReader_t *reader);

/**
 * @brief  The signature of the function that implements the reading into a
 *         patch.
 */
typedef void
(*gridReader_readIntoPatchFunc_t)(gridReader_t reader, gridPatch_t patch);

/**
 * @brief  The signature of the function that implements the reading into a
 *         patch for only one variable of the patch.
 */
typedef void
(*gridReader_readIntoPatchForVarFunc_t)(gridReader_t reader,
                                        gridPatch_t  patch,
                                        int          idxOfVar);

/**
 * @brief  The signature of the function that reacts on filename changes.
 */
typedef void
(*gridReader_handleFilenameChangeFunc_t)(gridReader_t reader);


/*--- Internal structures -----------------------------------------------*/

/** @brief  Provides the function table. */
struct gridReader_func_struct {
	/** @brief  The function used to delete the reader. */
	gridReader_delFunc_t                 del;
	/** @brief  The function to read into a patch. */
	gridReader_readIntoPatchFunc_t       readIntoPatch;
	/** @brief  The function to read into one variable of the patch. */
	gridReader_readIntoPatchForVarFunc_t readIntoPatchForVar;
};

/** @brief  Provides a short name for the function table. */
typedef struct gridReader_func_struct *gridReader_func_t;


/*--- ADT implementation ------------------------------------------------*/

/**
 * @brief  Implements the main structure for a grid Writer.
 */
struct gridReader_struct {
	/** @brief  Holds the type of the reader. */
	gridIO_type_t                         type;
	/** @brief  Holds the function table of the actual functions. */
	gridReader_func_t                     func;
	/** @brief  The call-back function for file-name changes. */
	gridReader_handleFilenameChangeFunc_t handleFilenameChange;
	/** @brief  Holds the file name object for the file. */
	filename_t                            fileName;
	/** @brief	Toggles region to write patch. */
	bool		 doPatch;
	/** @brief	Gives region to write idxLo. */
	gridPointUint32_t	 rtwLo;
	/** @brief	Gives region to write dims. */
	gridPointUint32_t	 rtwDims;
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
 * @brief  Sets all required fields of the basic reader structure.
 *
 * @param[in,out]  reader
 *                    The reader to initialize.  This must be a valid writer
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
gridReader_init(gridReader_t                          reader,
                gridIO_type_t                         type,
                gridReader_func_t                     func,
                gridReader_handleFilenameChangeFunc_t handleFilenameChange);


/**
 * @brief  This will free the members of the basic structure.
 *
 * @param[in,out]  reader
 *                    The reader to work with.  This must be a valid writer
 *                    object, passing @c NULL is undefined.
 */
extern void
gridReader_free(gridReader_t reader);


/** @} */


#endif
