// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
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
#include "gadgetBlock.h"
#include "gadgetTOC.h"
#include "gadgetVersion.h"
#include "../libutil/endian.h"
#include "../libutil/stai.h"


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  Decides whether the object is for reading or writing. */
typedef enum {
	/// @brief  Write mode, creating a new file, deleting old if existing.
	GADGET_MODE_WRITE_CREATE,
	/// @brief  Write mode, using an existing file, creating if not existing.
	GADGET_MODE_WRITE_CONT,
	/// @brief  Read mode.
	GADGET_MODE_READ
} gadget_mode_t;


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
 * @brief  Creates a new (largely uninitialized) Gadget (v2) file object.
 *
 * @return  Returns a new Gadget file object.
 */
extern gadget_t
gadget_new(void);


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
gadget_newSimple(const char *fileNameStem, int numFiles);


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
 * @name  Initialisation
 *
 * @{
 */

/**
 * @brief  Initializes the Gadget file object for reading.
 *
 * This requires the (stem of the) file name(s) and the number of files to
 * be set before calling this function.  It will then find out the version
 * of the Gadget file and the endianess of the files, and will read all
 * headers and TOCs.
 *
 * @param[in]  gadget
 *                The Gadget file object.  This must not be @c NULL.
 *
 * @return  Returns nothing.
 */
extern void
gadget_initForRead(gadget_t gadget);


/** @} */

/**
 * @name  Setter
 *
 * @{
 */

/**
 * @brief  Sets the number of files in the Gadget file set.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object.  Must not be @c NULL.
 * @param[in]      numFiles
 *                    The new number of files.
 *
 * @return  Returns nothing.
 */
extern void
gadget_setNumFiles(gadget_t gadget, int numFiles);


/**
 * @brief  Sets the version of the Gadget file.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object.  Must not be @c NULL.
 * @param[in]      version
 *                    The file version.
 *
 * @return  Returns nothing.
 */
extern void
gadget_setFileVersion(gadget_t gadget, gadgetVersion_t version);


/**
 * @brief  Sets the name of the given file.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object.  Must not be @c NULL.
 * @param[in]      fileNumber
 *                    The number of the file for which to set the file
 *                    name.  This must be a valid file number.
 * @param[in]      *fileName
 *                    The new file name.  The function will use exactly
 *                    this reference and will not perform a deep-copy.
 */
extern void
gadget_setFileNameOfFile(gadget_t gadget, int fileNumber, char *fileName);


/**
 * @brief  Sets all file names from a provided stem by attaching
 *         appropriate numbers.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object.  Must not be @c NULL.
 * @param[in]      stem
 *                    The stem of the file names.  The number of each
 *                    file will be appended, unless the Gadget file
 *                    consists of only one physical file.  In the latter
 *                    case, the file name will be set equal to the stem.
 *                    The function will internally create copies of the
 *                    provided string, the calling function is
 *                    still responsible for freeing @c stem.
 */
extern void
gadget_setFileNamesFromStem(gadget_t gadget, const char *stem);


/**
 * @brief  Sets the endianess of the file.
 *
 * The default value of the endianess is either automatic detection when
 * reading, or the system endianess when writing.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object.  Must not be @c NULL.
 * @param[in]      endianess
 *                    The endianess of the file.
 *
 * @return  Returns nothing.
 */
extern void
gadget_setFileEndianess(gadget_t gadget, endian_t endianess);


/**
 * @brief  Sets the header for a given file.
 *
 * If a header for the file already existed, it will be freed.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object.  Must not be @c NULL.
 * @param[in]      fileNumber
 *                    The number of the file for which to set the file
 *                    name.  This must be a valid file number.
 * @param[in]      header
 *                    The header to set.  The function will use exactly
 *                    this reference and does not perform a deep-copy.
 *                    It may be @c NULL.
 */
extern void
gadget_setHeaderOfFile(gadget_t       gadget,
                       int            fileNumber,
                       gadgetHeader_t header);


/**
 * @brief  Sets the table of content for a given file.
 *
 * If there is already a table of content for the requested file, then it
 * will be deleted before the new one is set.  When using this function,
 * file version stored in the TOC will be set to the file version used in
 * the Gadget file object, i.e. the offsets stored in the TOC might be
 * recalculated.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object.  Must not be @c NULL.
 * @param[in]      fileNumber
 *                    The number of the file for which to set the table of
 *                    content.  This must be a valid file number.
 * @param[in]      toc
 *                    The table of content.  The function will use exactly
 *                    this reference and dos not perform a deep-copy.  It
 *                    may be @c NULL.
 *
 * @return  Returns nothing.
 */
extern void
gadget_setTOCOfFile(gadget_t    gadget,
                    int         fileNumber,
                    gadgetTOC_t toc);


/** @} */

/**
 * @name  Getter
 *
 * @{
 */

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


/**
 * @brief  Retrieves the file version of the Gadget file.
 *
 * @param[in]  gadget
 *                The Gadget file object.  Must not be @c NULL.
 *
 * @return  Returns the version, will be either 1 or 2.
 */
extern gadgetVersion_t
gadget_getFileVersion(const gadget_t gadget);


/**
 * @brief  Retrieves the name of a given file.
 *
 * @param[in]  gadget
 *                The Gadget file object.  Must not be @c NULL.
 * @param[in]  fileNumber
 *                The number of the file to get the name for.  Must be
 *                valid file number.
 *
 * @return  Returns a pointer to the internal string holding the name of
 *          the file.
 */
extern const char *
gadget_getFileNameOfFile(const gadget_t gadget, int fileNumber);


/**
 * @brief  Retrieves the endianess of the file.
 *
 * @param[in]  gadget
 *                The Gadget file object.  Must not be @c NULL.
 *
 * @return  Returns the endianess of the file.
 */
extern endian_t
gadget_getFileEndianess(const gadget_t gadget);


/**
 * @brief  Retrieves the header for a given file.
 *
 * @param[in]  gadget
 *                The Gadget file object.  Must not be @c NULL.
 * @param[in]  fileNumber
 *                The number of the file to get the header for.  Must be
 *                valid file number.
 *
 * @return  Returns the header associated with the requested file.  This
 *          may be @c NULL, if no header is associated.
 */
extern gadgetHeader_t
gadget_getHeaderOfFile(const gadget_t gadget, int fileNumber);


/**
 * @brief  Retrieves the table of content of a given file.
 *
 * @param[in]  gadget
 *                The Gadget file object.  Must not be @c NULL.
 * @param[in]  fileNumber
 *                The number of the file to get the TOC for.  Must be
 *                valid file number.
 *
 * @return  Returns the TOC associated with the requested file.  This
 *          may be @c NULL, if no TOC is associated.
 */
extern gadgetTOC_t
gadget_getTOCOfFile(const gadget_t gadget, int fileNumber);


/** @} */

/**
 * @name IO: Basic
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
 * @param[in]      fileNumber
 *                    The file number to open.
 *
 * @return  Returns nothing.
 */
extern void
gadget_open(gadget_t gadget, gadget_mode_t mode, int fileNumber);


/**
 * @brief  Closes the currently opened file.
 *
 * @param[in,out]  gadget
 *                    The file object to work with.
 *
 * @return  Returns nothing.
 */
extern void
gadget_close(gadget_t gadget);


/**
 * @brief  This open a file for writing and ensure that it has the correct
 *         size.
 *
 * This requires the TOC of the file to be setup correctly so that the file
 * size can be determined.  After this function has been called, all write
 * calls are guaranteed to succeed provided that neither the TOC nor the
 * header changes.
 *
 * @param[in,out]  gadget
 *                    The file object to work with.  Must not be @c NULL.
 * @param[in]      fileNumber
 *                    The file number to open.  Must be a valid number.
 *
 * @return  Returns nothing.
 */
extern void
gadget_createEmptyFile(gadget_t gadget, int fileNumber);


/** @} */

/**
 * @name IO: Writing (Fileset)
 *
 * @{
 */

/**
 * @brief  Writes a selection of particle data to a file-set, automatically
 *         spanning the relevant files.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object to use.
 * @param[in]      block
 *                    The block to write.
 * @param[in]      pSkip
 *                    The particles to skip before starting to write.
 * @param[in]      pWrite
 *                    The number of particles to write.
 * @param[in]      stai
 *                    The description of the external data array.
 *
 * @return  Returns the number of particles written.
 */
extern uint64_t
gadget_writeBlock(gadget_t      gadget,
                  gadgetBlock_t block,
                  uint64_t      pSkip,
                  uint64_t      pWrite,
                  const stai_t  stai);


/** @} */

/**
 * @name IO: Writing (Current File)
 *
 * @{
 */

/**
 * @brief  Will write the header of the currently opened file to disk.
 *
 * @param[in,out]  gadget
 *                    The file object to work with.  The proper file must be
 *                    opened for writing and a headers for the file must
 *                    exits.  Also, the TOC of the file must be properly
 *                    set.
 *
 * @return  Returns nothing.
 */
extern void
gadget_writeHeaderToCurrentFile(gadget_t gadget);


/**
 * @brief  Will write a (subset of a) block to the currently opened file.
 *
 * @param[in,out]  gadget
 *                    The file object to work with.  The proper file must be
 *                    opened for writing and a headers for the file must
 *                    exits.  Also, the TOC of the file must be properly
 *                    set.
 * @param[in]      block
 *                    The block that should be written.
 * @param[in]      pSkipFile
 *                    The number of particles to skip before starting to
 *                    write.  Must be less than or equal to the number of
 *                    particles in the block.
 * @param[in]      pWriteFile
 *                    The number of particles to write.  Must be less than
 *                    or equal to the number of particles in the block less
 *                    the number of particles that were skipped.
 * @param[in]      stai
 *                    The abstract description of the data that should be
 *                    written.
 *
 * @return  Returns the number of particles that have been written.
 */
extern uint64_t
gadget_writeBlockToCurrentFile(gadget_t      gadget,
                               gadgetBlock_t block,
                               uint32_t      pSkipFile,
                               uint32_t      pWriteFile,
                               const stai_t  stai);


/** @} */

/**
 * @name IO: Reading (Fileset)
 *
 * @{
 */

/**
 * @brief  Reads a selection of particle from to a file-set, automatically
 *         spanning the relevant files.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object to use.
 * @param[in]      block
 *                    The block to write.
 * @param[in]      pSkip
 *                    The particles to skip before starting to read.
 * @param[in]      pRead
 *                    The number of particles to read.
 * @param[in]      stai
 *                    The description of the external data array.
 *
 * @return  Returns the number of particles written.
 */
extern uint64_t
gadget_readBlock(gadget_t      gadget,
                 gadgetBlock_t block,
                 uint64_t      pSkip,
                 uint64_t      pRead,
                 stai_t        stai);


/** @} */

/**
 * @name IO: Reading (Current File)
 *
 * @{
 */

/**
 * @brief  Reads the TOC from the currently opened file.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object to work with.  The proper file
 *                    must be opened for reading.  If a TOC for this file
 *                    already exists, it will be overwritten.
 *
 * @return  Returns nothing.
 */
extern void
gadget_readTOCFromCurrentFile(gadget_t gadget);


/**
 * @brief  Reads the header values from the currently opened file.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object to work with.  The proper file
 *                    must be opened for reading.  If a header for this file
 *                    already exists, it will be overwritten.
 *
 * @return  Returns nothing.
 */
extern void
gadget_readHeaderFromCurrentFile(gadget_t gadget);


/**
 * @brief  Autodetects the endianess of the file.
 *
 * This function is intended to be called just after gadget_open(), to set
 * the swapping of the file that is being read accordingly.  It is not
 * required to use this function, the endianess of the file can also be set
 * through the usage of gadget_setFileEndianess().
 *
 * @param[in,out]  gadget
 *                    The file object to work with.  A file must be opened
 *                    for reading and must be positioned at the beginning of
 *                    a block.  The endianess will be determined and stored
 *                    in the file object.
 *
 * @return  Returns nothing.
 */
extern void
gadget_detectByteswappingOfCurrentFile(gadget_t gadget);


/**
 * @brief  Autodetects the version of the file.
 *
 * @param[in,out]  gadget
 *                    The file object to work with.  A file must be opened
 *                    for reading and may be positioned anywhere in the file
 *                    (the position is restored when the function returns).
 *                    The file version is identified from the first block in
 *                    the file, which will either be the header description
 *                    block (version 2) or the header itself (version 1).
 *
 * @return  Returns nothing.
 */
extern void
gadget_detectFileVersionOfCurrentFile(gadget_t gadget);


/**
 * @brief  Reads a block from the current file.
 *
 * @param[in,out]  gadget
 *                    The fileset from which to read.
 * @param[in]      block
 *                    The block from which to read.
 * @param[in]      pSkipFile
 *                    The number of particles to skip in the block before
 *                    starting to read.
 * @param[in]      pReadFile
 *                    The number of particles to read.
 * @param[in]      stai
 *                    The abstract description of the external memory that
 *                    will receive the payload.
 *
 * @return  Returns the number of particles that have been read.
 */
extern uint64_t
gadget_readBlockFromCurrentFile(gadget_t      gadget,
                                gadgetBlock_t block,
                                uint64_t      pSkipFile,
                                uint64_t      pReadFile,
                                stai_t        stai);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilFilesGadget Gadget
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to Gadget files.
 *
 */


#endif
