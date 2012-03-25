// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETBLOCK_H
#define GADGETBLOCK_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetBlock.h
 * @ingroup libutilFilesGadgetBlock
 * @brief  This file provides the interface to work with Gadget file blocks.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "gadgetVersion.h"


/*--- Defines -----------------------------------------------------------*/

/** @brief  Gives the number of allowed block types. */
#define GADGETBLOCK_NUM 14

/** @brief  Gives the length of the descriptor blocks (1 int, 4 char). */
#define GADGETBLOCK_DESCRIPTOR_BLOCK_LENGTH 8


/*--- Typedefs ----------------------------------------------------------*/

/**
 * @brief Declares the different blocks supported.
 *
 * This must have exactly #GADGETBLOCK_NUM entries numbered from 0
 * to #GADGETBLOCK_NUM - 1.  It must also match up with
 * #local_blockNames.
 */
typedef enum {
	/// @brief  Header.
	GADGETBLOCK_HEAD    = 0,
	/// @brief  Position.
	GADGETBLOCK_POS_    = 1,
	/// @brief  Velocity.
	GADGETBLOCK_VEL_    = 2,
	/// @brief  Particle identifier.
	GADGETBLOCK_ID__    = 3,
	/// @brief  Mass.
	GADGETBLOCK_MASS    = 4,
	/// @brief  Internal energy.
	GADGETBLOCK_U___    = 5,
	/// @brief  Density.
	GADGETBLOCK_RHO_    = 6,
	/// @brief  Electron abundance.
	GADGETBLOCK_NE__    = 7,
	/// @brief  Neutral hydrogen fraction.
	GADGETBLOCK_NH__    = 8,
	/// @brief  Smoothing length.
	GADGETBLOCK_HSML    = 9,
	/// @brief  The star formation rate.
	GADGETBLOCK_SFR_    = 10,
	/// @brief  Age.
	GADGETBLOCK_AGE_    = 11,
	/// @brief  Metallicity.
	GADGETBLOCK_Z___    = 12,
	/// @brief  Unknown block, if this is returned, it is an error.
	GADGETBLOCK_UNKNOWN = 13
} gadgetBlock_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Translating
 * @{
 */

/**
 * @brief  Retrieves the name of a given block type.
 *
 * @param[in]  blockType
 *                The block type to translate into a string.
 *
 * @return  Returns the ASCII name of the block type.
 */
extern const char *
gadgetBlock_getNameFromType(gadgetBlock_t blockType);


/**
 * @brief  Returns the block types that corresponds to a given block
 *         name.
 *
 * @param[in]  *name
 *                The name that should be translated into a block type.
 *                Only the first 4 characters are evaluated, i.e. the
 *                string must have at least 4 characters but does not
 *                need to be null terminated.
 *
 * @return  Returns the block type or the special type
 *          #GADGETBLOCK_UNKNOWN if there is no corresponding block
 *          type.
 */
extern gadgetBlock_t
gadgetBlock_getTypeFromName(const char *name);


/** @} */

/**
 * @name  Querying features of blocks
 * @{
 */

/**
 * @brief  Returns the number of components an element of a given block has.
 *
 * @param[in]  block
 *                The block to get the number of elements for.  This must
 *                not be neither #GADGETBLOCK_UNKNOWN nor
 *                #GADGETBLOCK_HEAD.
 *
 * @return  Returns the number of components, this should normally be either
 *          1 (for scalars) or 3 (for vectors).
 */
extern uint32_t
gadgetBlock_getNumComponents(const gadgetBlock_t block);


/**
 * @brief  Checks if a block stores integer values.
 *
 * @param[in]  block
 *                The block that should be checked.
 *
 * @return  Returns @c true if the block stores integers and @c false if
 *          not.
 */
extern bool
gadgetBlock_isInteger(const gadgetBlock_t block);


/**
 * @brief  Returns the number of particles contributing to a given block.
 *
 * @param[in]  block
 *                The block for which to find out how many particles
 *                contribute.
 * @param[in]  np
 *                The number of particles per type.
 * @param[in]  massarr
 *                The masses for the different particle type.  For every
 *                entry that is zero, the particles will have masses stored
 *                in the file.
 *
 * @return  Returns the number of particles that contribute to the given
 *          block.  if @c block is #GADGETBLOCK_UNKNOWN, @c 0 will be
 *          returned.
 */
extern uint32_t
gadgetBlock_getNumPartsInBlock(const gadgetBlock_t block,
                               const uint32_t      np[6],
                               const double        massarr[6]);


/** @} */

/**
 * @name Block-IO
 * @{
 */

/**
 * @brief  Writes the block descriptor into a file.
 *
 * @param[in,out]  *f
 *                    The file into which to write.  Needs to be
 *                    positioned at the correct position and opened for
 *                    binary writing.
 * @param[in]      block
 *                    The block that should be written.  Must not be
 *                    #GADGETBLOCK_UNKNOWN.
 * @param[in]      dataBlockSize
 *                    The size of the actual data block in bytes.  This
 *                    should not include the guarding Fortran delimiters
 *                    of the data block.
 * @param[in]      doByteSwap
 *                    Selects whether the binary values need to be byte
 *                    swapped for endianess adjustment.
 * @param[in]      fileVersion
 *                    The version of the Gadget file the block
 *                    descriptor should be written for.  Note that
 *                    #GADGETVERSION_ONE doesn't support block
 *                    descriptors, hence in this case the function will
 *                    do nothing.
 *
 * @return  Returns nothing.
 */
extern void
gadgetBlock_writeDescriptor(FILE            *f,
                            gadgetBlock_t   block,
                            uint32_t        dataBlockSize,
                            bool            doByteSwap,
                            gadgetVersion_t fileVersion);


/**
 * @brief  Reads a block descriptor from a file.
 *
 * @param[in,out]  *f
 *                    The file from which to read.  Needs to be
 *                    positioned at the correct position and opened for
 *                    binary reading.
 * @param[out]     *block
 *                    This will receive the type of the block.  May be
 *                    @c NULL, then nothing is returned.
 * @param[out]     *dataBlockSize
 *                    Pointer to an external variable that will receive
 *                    the size of the data block in bytes.  This will be
 *                    the size of the actual data, excluding the two
 *                    four-byte integer block delimiters.  May be
 *                    @c NULL, then nothing is returned.
 * @param[in]      doByteSwap
 *                    Selects whether the binary values need to be byte
 *                    swapped for endianess adjustment.
 * @param[in]      fileVersion
 *                    The version of the Gadget file the block
 *                    descriptor has been written for.  Note that
 *                    #GADGETVERSION_ONE doesn't support block
 *                    descriptors, hence in this case the function will
 *                    do nothing and return @c false.
 *
 * @return  Returns @c true if the block has been parsed and the return
 *          parameters filled, otherwise @c false is returned.  Note
 *          that if @c fileVersion corresponds to a Gadget format that
 *          does not have block descriptors, then @c false is always
 *          returned.
 */
extern bool
gadgetBlock_readDescriptor(FILE            *f,
                           gadgetBlock_t   *block,
                           uint32_t        *dataBlockSize,
                           bool            doByteSwap,
                           gadgetVersion_t fileVersion);


/**
 * @brief  Reads a block descriptor from a file, returning the name of the
 *         block.
 *
 * Works as gadgetBlock_readDescriptor() but instead of returning the block
 * type, it returns the name of block.
 *
 * @param[in,out]  *f
 *                    As in gadgetBlock_readDescriptor().
 * @param[out]     *name
 *                    This will receive the name of the block.
 * @param[out]     *dataBlockSize
 *                    As in gadgetBlock_readDescriptor().
 * @param[in]      doByteSwap
 *                    As in gadgetBlock_readDescriptor().
 * @param[in]      fileVersion
 *                    As in gadgetBlock_readDescriptor().
 *
 * @return   As in gadgetBlock_readDescriptor().
 */
extern bool
gadgetBlock_readDescriptorString(FILE            *f,
                                 char            *name,
                                 uint32_t        *dataBlockSize,
                                 bool            doByteSwap,
                                 gadgetVersion_t fileVersion);


/**
 * @brief  Will skip over a block and check if it worked.
 *
 * A block is everything that is encapuslated into a leading and
 * trailing block size integer, which have to be equal and give the size
 * of block in bytes.
 *
 * @param[in,out]  *f
 *                    The file pointer.  Must be positioned at the
 *                    beginning of a block (at the position of the
 *                    leading block size integer).  Will be seeked
 *                    forward to just after the trailing block size
 *                    integer.  If any error occured, the position the
 *                    file pointer had at entry will be restored.
 * @param[in]      doByteSwap
 *                    Indicates whether an endian correction is needed
 *                    (required to read the correct block size).
 *
 * @return  Returns @c true if the block could be correctly skipped.  If
 *          @c false is returned, some error prevented the function from
 *          correctly skipping over the block.  This might happen if
 *          reading from or seeking in the file did not succeed.  The
 *          function will also return @c false if the leading and
 *          trailing block size integers are not equal.  If @c false is
 *          returned, the file pointer is moved back to the position it
 *          had when entering into the function.
 */
extern bool
gadgetBlock_skip(FILE *f, bool doByteSwap);


/**
 * @brief  Moves over one block, checks that it worked and returns the
 *         blocksize.
 *
 * This works like gadgetBlock_skip() with the additional report of the size
 * of the block that was skipped.
 *
 * @param[in,out]  *f
 *                    The file pointer, see gadgetBlock_skip() for more
 *                    description.
 * @param[in]      doByteSwap
 *                    Indicates whether an endian correction is needed.
 * @param[out]     *blockSizeInBytes
 *                    External variable that will receive the size of the
 *                    block in bytes.  This will only be set if the skipping
 *                    worked.
 *
 * @return  Retruns @c true if the block could be correctly skipped.  In
 *          this case, @c *blockSizeInBytes will also be set correctly.  If
 *          @c false is returned, then the blocksize will not be set and the
 *          file pointer will be returned to the position it had before
 *          entering the function and all errors on the stream will be
 *          cleared.  See gadgetBlock_skip() for reasons why this might
 *          occur.
 */
extern bool
gadgetBlock_skip2(FILE *f, bool doByteSwap, uint32_t *blockSizeInBytes);


/**
 * @brief  Will write an empty block with a given size into a file.
 *
 * This will write the leading and trailing block size integers and fill
 * the data part of the block with zeros.
 *
 * @param[in,out]  *f
 *                    The file pointer.  Must be positioned at the
 *                    position at which to write the empty block.
 */
extern void
gadgetBlock_writeEmptyBlockToFile(FILE     *f,
                                  uint32_t blockSize,
                                  bool     doByteSwap);


/**
 * @brief  Will write a block size integer to the file dealing with
 *         endianess isssues.
 *
 * @param[in,out]  *f
 *                    The file to write to, must not be @c NULL and has to
 *                    be opened for binary writing.
 * @param[in]      blockSize
 *                    The block size integer to write.
 * @param[in]      doByteSwap
 *                    Toggles whether the endianess needs to be adjusted
 *                    (@c true) or not (@c false).
 *
 * @return  Returns nothing.
 */
extern void
gadgetBlock_writeBlockSize(FILE *f, uint32_t blockSize, bool doByteSwap);


/**
 * @brief  Will read a block size integer from the file dealing with
 *         endianess isssues.
 *
 * @param[in,out]  *f
 *                    The file to read from, must not be @c NULL and has to
 *                    be opened for binary reading.
 * @param[in]      *blockSize
 *                    The block size integer to read into.
 * @param[in]      doByteSwap
 *                    Toggles whether the endianess needs to be adjusted
 *                    (@c true) or not (@c false).
 *
 * @return  Returns nothing.
 */
extern void
gadgetBlock_readBlockSize(FILE *f, uint32_t *blockSize, bool doByteSwap);


/** @} */


/*--- Doxygen group definition ------------------------------------------*/

/**
 * @defgroup libutilFilesGadgetBlock Gadget Blocks
 * @ingroup libutilFilesGadget
 * @brief Provides low-level interfaces to Gadget blocks.
 */


#endif
