// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETTOC_H
#define GADGETTOC_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetTOC.h
 * @ingroup  libutilFilesGadgetTOC
 * @brief  Provides the table of content of Gadget files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include <stdbool.h>
#include "gadgetBlock.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the TOC object. */
typedef struct gadgetTOC_struct *gadgetTOC_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creation and Deletion
 * @{
 */

/**
 * @brief  Creates a new (empty) table of content for v2 files.
 *
 * @return  A new and empty table of content.
 */
extern gadgetTOC_t
gadgetTOC_new(void);


/**
 * @brief  Creates a new (and initialized) table of content from a file.
 *
 * @param[in,out]  *f
 *                    The file pointer of the Gadget file that should be
 *                    parsed to generate the TOC.  The file must be opened
 *                    for binary reading, after completion of the function,
 *                    the file pointer is returned to its initial position.
 *
 * @return  Returns a new TOC object representing the file.
 */
extern gadgetTOC_t
gadgetTOC_newFromFile(FILE *f);


/**
 * @brief  Creates a new TOC that has the same block defined as a provided
 *         template TOC.
 *
 * @param[in]  tmpl
 *                The TOC that should be cloned.
 *
 * @return  Returns a new TOC object with the same blocks as the template
 *          TOC.
 */
extern gadgetTOC_t
gadgetTOC_clone(const gadgetTOC_t tmpl);

/**
 * @brief  Deletes a TOC object and frees all associated memory.
 *
 * @param[in,out]  *toc
 *                    A pointer to the variable that holds the TOC
 *                    reference.  After deletion of the object, the external
 *                    variable is set to @c NULL.  Passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_del(gadgetTOC_t *toc);


/** @} */

/**
 * @name  Getter/Setter
 * @{
 */

/**
 * @brief  Sets the version of the Gadget file the TOC is for.
 *
 * The default is to use v2 files.  This function will call
 * gadgetTOC_calcOffset() to propagate the changes accordingly to the
 * offsets.
 *
 * @param[in,out]  toc
 *                    The toc for which to set the file version. Must not be
 *                    @c NULL.
 * @param[in]      ver
 *                    The file version to set.  Must not be
 *                    #GADGETVERSION_UNKNOWN.
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_setFileVersion(gadgetTOC_t toc, gadgetVersion_t ver);


/**
 * @brief  Retrieves the file version the TOC is for.
 *
 * @param[in]  toc
 *                The TOC to query.
 *
 * @return  Returns the file version.
 */
extern gadgetVersion_t
gadgetTOC_getFileVersion(const gadgetTOC_t toc);


/** @} */

/** @name  Queries
 * @{
 */

/**
 * @brief  Checks if a given block exists in the TOC.
 *
 * @param[in]  toc
 *                The TOC to query.  Must not be @c NULL.
 * @param[in]  block
 *                The block to look for.
 *
 * @return  Returns @c true if the block exists and @c false otherwise.
 */
extern bool
gadgetTOC_blockExists(const gadgetTOC_t toc, gadgetBlock_t block);


/**
 * @brief  Checks if a given block exist by checking the name.
 *
 * @param[in]  toc
 *                The TOC to query, must not be @c NULL.
 * @param[in]  *name
 *                The name of the block, must be a string of at least 4
 *                characters (only the first 4 are used).
 *
 * @return  Returns @c true if the block exists and @c false otherwise.
 */
extern bool
gadgetTOC_blockExistsByName(const gadgetTOC_t toc, const char *name);


/**
 * @brief  Retrieves the offset for a given block.
 *
 * @param[in]  toc
 *                The TOC to query, must not be @c NULL.
 * @param[in]  block
 *                The block for which to return the offset.
 *
 * @return  Returns the offset of the requested block.  This may be @c -1,
 *          if the block does not exist in the TOC or the offset is unknown.
 */
extern long
gadgetTOC_getOffsetForBlock(const gadgetTOC_t toc, gadgetBlock_t block);


/**
 * @brief  Retrieves the size of the given block.
 *
 * @param[in]  toc
 *                The TOC to query, must not be @c NULL.
 * @param[in]  block
 *                The block for which to return the size.
 *
 * @return  Returns the size of the requested block.  This may be @c 0, if
 *          the block does not exist in the TOC or the size is unknown.
 */
extern uint32_t
gadgetTOC_getSizeInBytesForBlock(const gadgetTOC_t toc, gadgetBlock_t block);


/**
 * @brief  Retrieves the name of the block.
 *
 * @param[in]  toc
 *                The TOC to query, must not be @c NULL.
 * @param[in]  block
 *                The block for which to return the size.
 *
 * @return  Returns a pointer to the internal storage of the name of the
 *          block.  May return @c NULL if the block did not exist.
 */
extern const char *
gadgetTOC_getNameInV2FilesForBlock(const gadgetTOC_t toc,
                                   gadgetBlock_t     block);


/**
 * @brief  Retrieves the offset of the block.
 *
 * @param[in]  toc
 *                The TOC to query, must not be @c NULL.
 * @param[in]  seqNumber
 *                The position of the block in the TOC, must be a valid
 *                sequence number.
 *
 * @return  Returns the offset of the block that can be used to xfseek()
 *          directly to the beginning of that block.
 */
extern long
gadgetTOC_getOffsetBySeqNumber(const gadgetTOC_t toc, int seqNumber);


/**
 * @brief  Retrieves the type of the block.
 *
 * @param[in]  toc
 *                The TOC to query, must not be @c NULL.
 * @param[in]  seqNumber
 *                The position of the block in the TOC, must be a valid
 *                sequence number.
 *
 * @return  Returns the type of the block.
 */
extern gadgetBlock_t
gadgetTOC_getTypeBySeqNumber(const gadgetTOC_t toc, int seqNumber);


/**
 * @brief  Retrieves the size of the block.
 *
 * @param[in]  toc
 *                The TOC to query, must not be @c NULL.
 * @param[in]  seqNumber
 *                The position of the block in the TOC, must be a valid
 *                sequence number.
 *
 * @return  Returns the size of the block in bytes.
 */
extern uint32_t
gadgetTOC_getSizeInBytesBySeqNumber(const gadgetTOC_t toc, int seqNumber);


/**
 * @brief  Retrieves the name of the block.
 *
 * @param[in]  toc
 *                The TOC to query, must not be @c NULL.
 * @param[in]  seqNumber
 *                The position of the block in the TOC, must be a valid
 *                sequence number.
 *
 * @return  Returns internal string holding the name of the block.
 */
extern const char *
gadgetTOC_getNameBySeqNumber(const gadgetTOC_t toc, int seqNumber);


extern size_t
gadgetTOC_getTotalFileSize(const gadgetTOC_t toc);


/** @} */

/** @name  Using
 * @{
 */

/**
 * @brief  Adds a new entry by name to the TOC.
 *
 * The entry will only be added if it does not already exist.
 *
 * @param[in,out]  toc
 *                    The TOC to which to add an element.
 * @param[in]      name
 *                    The name of the element to add.  This should be a
 *                    string containing at least 4 characters and it should
 *                    correspond to the according name for the block.
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_addEntryByName(gadgetTOC_t toc, const char *name);


/**
 * @brief  Adds a new entry by type to the TOC.
 *
 * The entry will only be added if it does not already exist.
 *
 * @param[in,out]  toc
 *                    The TOC to which to add an element.
 * @param[in]      block
 *                    The type to add.  This must not be
 *                    #GADGETBLOCK_UNKNOWN.
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_addEntryByType(gadgetTOC_t toc, gadgetBlock_t block);


/**
 * @brief  Will calculate the proper blocksize given a file setup.
 *
 * @param[in,out]  toc
 *                    The TOC to set the sizes for.  Must not be @c NULL.
 * @param[in]      np
 *                    The number of particles per particle type.
 * @param[in]      massarr
 *                    The masses of the particles per particle type
 *                    (influences how many particles are stored in the mass
 *                    block).
 * @param[in]      useDoublePrec
 *                    Flags whether the particle data is stored as double
 *                    (@c true) or single precision (@c false).
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_calcSizes(gadgetTOC_t    toc,
                    const uint32_t np[6],
                    const double   massarr[6],
                    bool           useDoublePrec,
                    bool           useLongIDs);


/**
 * @brief  Calculates the offsets of the blocks for the different blocks.
 *
 * For this function to work sensibly, the file version of the file the TOC
 * is for must be set properly (see gadgetTOC_setFileVersion()).
 *
 * @param[in,out]  toc
 *                    The TOC object on which to work.
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_calcOffset(gadgetTOC_t toc);


/**
 * @brief  Positions the provided stream to the beginning of the data
 *         section of the requested block.
 *
 * @param[in]      toc
 *                    The TOC to use, must not be @c NULL.
 * @param[in]      block
 *                    The block to which to seek, must exist in the TOC.
 * @param[in,out]  *f
 *                    The stream in which to seek, must be valid stream.
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_seekToData(const gadgetTOC_t toc, gadgetBlock_t block, FILE *f);


/**
 * @brief  Positions the provided stream to the beginning of the descriptor
 *         block of the requested block.
 *
 * @param[in]      toc
 *                    The TOC to use, must not be @c NULL.
 * @param[in]      block
 *                    The block to which to seek, must exist in the TOC.
 * @param[in,out]  *f
 *                    The stream in which to seek, must be valid stream.
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_seekToDescriptor(const gadgetTOC_t toc,
                           gadgetBlock_t     block,
                           FILE              *f);


/**
 * @brief  Verifies that the provided TOC is sensible.
 *
 * @param[in]  toc
 *                The TOC to check.  Mustnot be @c NULL.
 *
 * @return  Returns nothings.
 */
extern bool
gadgetTOC_isValid(const gadgetTOC_t toc);


/**
 * @brief  Pretty prints a TOC object to the provided stream.
 *
 * @param[in]      toc
 *                    The TOC that should be printed.  Must not be @c NULL.
 * @param[in,out]  *out
 *                    The stream to write to, must be opened for writing.
 * @param[in]      *prefix
 *                    Will be added in front of every line of output.  May
 *                    be @c NULL which will be interpreted as the empty
 *                    string.
 *
 * @return  Returns nothing.
 */
extern void
gadgetTOC_prettyPrint(const gadgetTOC_t toc, FILE *out, const char *prefix);


/** @} */


/*--- Doxygen group definition ------------------------------------------*/

/**
 * @defgroup libutilFilesGadgetTOC Gadget Table of Content
 * @ingroup libutilFilesGadget
 * @brief Provides an abstraction of the table of content of a Gadget file.
 *
 * The TOC is essentially a list holding the meta-information of each block
 * present in a Gadget file and its position in the file.
 */


#endif
