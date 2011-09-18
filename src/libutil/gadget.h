// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGET_H
#define GADGET_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadget.h
 * @ingroup libutilFilesGadget
 * @brief  This file provides the interface to Gadget (type 1 and 2)
 *         files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include "gadgetHeader.h"


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  Decides whether the object is for reading or writing. */
typedef enum {
	/// @brief  Write mode.
	GADGET_MODE_WRITE,
	/// @brief  Read mode.
	GADGET_MODE_READ
} gadgetMode_t;

/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a Gadget file object.
 */
typedef struct gadget_struct *gadget_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new Gadget (v2) file object.
 *
 * @param[in]  *fileNameStem
 *                The stem of the file names, or the proper file name if
 *                @c numFiles is 1.  Must not be @c NULL.
 * @param[in]  numFiles
 *                The number of physical files the file is spread over.
 *                Must be larger than 0.
 *
 * @return  Returns a new Gadget v2 file object.
 */
extern gadget_t
gadget_new(const char *fileNameStem, int numFiles);


/**
 * @brief  Deletes a Gadget file object and frees the associated memory.
 *
 * @param[in,out]  *gadget
 *                    A pointer to the external variable that holds the
 *                    Gadget object, will be set to @c NULL after
 *                    completion of the freeing process.  Passing
 *                    @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gadget_del(gadget_t *gadget);


/** @} */

/**
 * @name  Setter
 *
 * @{
 */

/**
 * @brief  Sets the version of the Gadget file.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object.  Must not be @c NULL.
 * @param[in]      version
 *                    The version, must be either 1 or 2.
 *
 * @return  Returns nothing.
 */
extern void
gadget_setFileVersion(gadget_t gadget, int version);


/** @} */

/**
 * @name  Getter
 *
 * @{
 */

/**
 * @brief  Retrieves the file version of the Gadget file.
 *
 * @param[in]  gadget
 *                The Gadget file object.  Must not be @c NULL.
 *
 * @return  Returns the version, will be either 1 or 2.
 */
extern int
gadget_getFileVersion(const gadget_t gadget);


/**
 * @brief  Retrieves the number of files used in for the Gadget file.
 *
 * @param[in]  gadget
 *                The Gadget file object.  Must not be @c NULL.
 *
 * @return  Returns the number of files.
 */
extern int
gadget_getNumFiles(const gadget_t gadget);


/** @} */

/**
 * @name  Using
 *
 * @{
 */

/**
 * @brief  Attaches a header to a given file.
 *
 * @param[in,out]  gadget
 *                    The file object to work with.
 * @param[in]      header
 *                    The header to attach.
 * @param[in]      numFile
 *                    The file number to which to attach the header.
 *
 * @return  Returns nothing.
 */
extern void
gadget_attachHeader(gadget_t gadget, gadgetHeader_t header, int numFile);


/** @} */

/**
 * @name Input/Output
 *
 * @{
 */

/**
 * @brief  Opens a given physical file for reading or writing.
 *
 * @param[in,out]  gadget
 *                    The file object to work with.
 * @param[in]      mode
 *                    The mode in which to open the file.
 * @param[in]      numFile
 *                    The file number to open.
 *
 * @return  Returns nothing.
 */
extern void
gadget_open(gadget_t gadget, gadgetMode_t mode, int numFile);


/**
 * @brief  Closes a given physical file.
 *
 * @param[in,out]  gadget
 *                    The file object to work with.
 * @param[in]      numFile
 *                    The file number to close.
 *
 * @return  Returns nothing.
 */
extern void
gadget_close(gadget_t gadget, int numFile);


/**
 * @brief  Writes a Gadget file.
 *
 * This requires the file to be opened in write mode.
 *
 * @param[in,out]  gadget
 *                    The Gadget object to use.
 * @param[in]      numFile
 *                    The file to write to.
 * @param[in]      *pos
 *                    The position values.  Must be an array of position
 *                    vectors of the proper length (number of particles
 *                    in the file).
 * @param[in]      *vel
 *                    The velocity values, same restrictions as for
 *                    @c pos.
 * @param[in]      *id
 *                    The IDs of the particles, same restrictions as for
 *                    @c pos.
 *
 * @return  Returns nothing.
 */
extern void
gadget_write(gadget_t                 gadget,
             int                      numFile,
             const float *restrict    pos,
             const float *restrict    vel,
             const uint32_t *restrict id);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilFilesGadget Gadget
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to Gadget files.
 *
 */


#endif
