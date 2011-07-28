// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef ENDIAN_H
#define ENDIAN_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/endian.h
 * @ingroup libutilMiscEndian
 * @brief  This file provides functionality to deal with endianess.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>
#include <stdio.h>


/*--- Typedefs ----------------------------------------------------------*/

/**
 * @brief The possible endianesses.
 */
typedef enum {
	/** That would be little endian. */
	ENDIAN_LITTLE,
	/** That would be big endian. */
	ENDIAN_BIG
} endian_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  System Endianess
 *
 * @{
 */

/**
 * @brief  Discovers the endianess of the system.
 *
 * @return  Returns the system endianess.
 */
extern endian_t
endian_getSystemEndianess(void);


/**
 * @brief  Checks whether the system is little endian.
 *
 * This simply wraps the result of endian_getSystemEndianess().
 *
 * @return  Returns true if the system is little endian, false
 *          otherwise.
 */
extern bool
endian_systemIsLittle(void);


/**
 * @brief  Checks whether the system is big endian.
 *
 * This simply wraps the result of endian_getSystemEndianess().
 *
 * @return  Returns true if the system is big endian, false
 *          otherwise.
 */
extern bool
endian_systemIsBig(void);


/** @} */

/**
 * @name  File Endianess
 *
 * @{
 */


/**
 * @brief  Checks for the endianess of a file using block delimiter.
 *
 * This will open the file and use the Fortran block delimiter to figure
 * out the endianess of the file, i.e. the file is required to be of the
 * form [4b integer][data size Nb][4b integer].  The leading and
 * trailing integers (they have to be of type uint32_t) both give the
 * number of byte N of the data part.
 *
 * Hence the leading and trailing integer can be used to infer the
 * endianess of the file in the following manner: Reading the first
 * integer gives the number of bytes that need to be skipped before the
 * exactly same integer should be found again.  The function will
 * perform this test by reading the first integer, skips ahead in the
 * file to read the trailing integer and compare their values.  Should
 * they not match, then the value of the first integer is byte-swapped
 * and it is again tried if, when skipping this new amount of bytes, a
 * corresponding integer is found.
 *
 * Actually, the function is a bit smarter, it will read the first
 * integer, byte-swap it, and first use the value that is smallest for
 * skipping.  Depending on whether the byte-swapped block size or the
 * verbatim one value was correct, and by knowing the endianess of the
 * system, the endianess of the file is inferred.
 *
 * This function is not fool-proof.  First of all, it is only guaranteed
 * to work, if the true block contains at least 1 and at most 65535
 * bytes.  Otherwise it is possible to have block sizes that are
 * invariant under byte-swapping (though you need to be unlucky to hit
 * that problem).  Furthermore, if the first block is very large,
 * then it might be that the function first tries to find the ending
 * block size within the data fields and if then the value that is read
 * there happens to agree with the tried block size, then the wrong
 * endianess is returned.  It is not very likely that this happens
 * though and probably would required an engineered file.
 *
 * So, basically, this function should @b only be used for blocks that
 * are at least 1b and at most 65535b long.  The function will not test
 * that and might quietly pass the wrong endianess in that case.
 *
 * @param[in]  *fname
 *                The name of the file to check the endianess for.
 *
 * @return  Returns the endianess of the file.
 */
extern endian_t
endian_getFileEndianessByBlock(const char *fname);


/**
 * @brief  Checks for the endianess of a file using block delimiter.
 *
 * This works like endian_getFileEndianessByBlock(), but instead of a
 * file name, a properly positioned file pointer is provided.  This can
 * be used to either use of block that fulfills the required constraints
 * on the block size, or when the file is anyway going to be opened for
 * reading (to reduces the number of calls to fopen() and fclose()).
 *
 * When the function returns, the file pointer will be positioned at the
 * same position it had when entering the function.
 *
 * @param[in,out]  *f
 *                    A file pointer of an opened for reading file,
 *                    positioned at the beginning of a block.  Passing
 *                    @c NULL is undefined.
 *
 * @return  Returns the endianess of the file.
 */
extern endian_t
endian_getFileEndianessByBlockF(FILE *f);


/**
 * @brief  Checks whether a file is little endian using block delimiter.
 *
 * This will simply wrap the result of endian_getFileEndianessByBlock().
 *
 * @param[in]  *fname
 *                The name of the file to check the endianess for.
 *
 * @return  Returns true if the file is written in little endian, false
 *          otherwise.
 */
extern bool
endian_fileIsLittleByBlock(const char *fname);


/**
 * @brief  Checks whether a file is big endian using block delimiter.
 *
 * This will simply wrap the result of endian_getFileEndianessByBlock().
 *
 * @param[in]  *fname
 *                The name of the file to check the endianess for.
 *
 * @return  Returns true if the file is written in big endian, false
 *          otherwise.
 */
extern bool
endian_fileIsBigByBlock(const char *fname);


/**
 * @brief  Checks whether a file is little endian using block delimiter.
 *
 * This will simply wrap the result of endian_getFileEndianessByBlockF().
 *
 * @param[in,out ]  *f
 *                    A file pointer of an opened for reading file,
 *                    positioned at the beginning of a block.  Passing
 *                    @c NULL is undefined.
 *
 * @return  Returns true if the file is written in little endian, false
 *          otherwise.
 */
extern bool
endian_fileIsLittleByBlockF(FILE *f);


/**
 * @brief  Checks whether a file is big endian using block delimiter.
 *
 * This will simply wrap the result of endian_getFileEndianessByBlockF().
 *
 * @param[in,out]  *f
 *                    A file pointer of an opened for reading file,
 *                    positioned at the beginning of a block.  Passing
 *                    @c NULL is undefined.
 *
 * @return  Returns true if the file is written in big endian, false
 *          otherwise.
 */
extern bool
endian_fileIsBigByBlockF(FILE *f);


/** @} */

/**
 * @name  Extras
 *
 * @{
 */

/**
 * @brief  Converts an endian to a string usable for printing.
 *
 * @param[in]  endian
 *                The endianess to convert to a string.
 *
 * @return  Returns a constant string holding a string version of the
 *          endianess.
 */
extern const char *
endian_toString(const endian_t endian);

/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilMiscEndian  Endianess
 * @ingroup libutilMisc
 * @brief Provides functionality to deal with endian issues.
 */


#endif
