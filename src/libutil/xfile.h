// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef XFILE_H
#define XFILE_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/xfile.h
 * @ingroup libutilCore
 * @brief This file provides the interface to utility functions dealing
 *        with files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * \brief  A wrapper function for fopen.
 *
 * This function will abort the program, if the file could not be
 * opened.
 *
 * \param  *path  The name of the file to open.
 * \param  *mode  The mode with which to open, see fopen(3).
 *
 * \return  Returns a file pointer to the opened file.
 */
extern FILE *
xfopen(const char *path, const char *mode);


/**
 * \brief  A wrapper function for fclose.
 *
 * This function will close the given file stream and also set the
 * variable holding the stream to NULL.
 *
 * \param  **fp  Pointer to the variable holding the stream;
 *
 * \return  This function will always return 0.
 */
extern int
xfclose(FILE **fp);


/**
 * \brief  A wrapper function for fread that will abort the program, if
 *         the read did not succeed.
 *
 * \param  *ptr     A pointer to the external variable that will hold
 *                  the results.  This needs to be large enough.
 * \param  size     The size in bytes of one element.
 * \param  nmemb    The number of elements to read.
 * \param  *stream  The stream to read from.
 *
 * \return  This function always returns nmemb.
 */
extern size_t
xfread(void *ptr, size_t size, size_t nmemb, FILE *stream);


/**
 * \brief  A wrapper function for fwrite that will abort the program if
 *         anything went wrong.
 *
 * \param  *ptr     The pointer to the data which should be printed.
 * \param  size     The size of one element.
 * \param  nmemb    The number of elements to write.
 * \param  *stream  The stream to write to.
 *
 * \return  This function always returns nmemb.
 */
extern size_t
xfwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);


/**
 * \brief  A wrapper function for fseek that will terminate the program
 *         in the occurrence of errors.
 *
 * \param  *stream  The stream to seek in.
 * \param  offset   The offset.
 * \param  whence   The anchor of the offset.
 *
 * \return  This function will always return 0.
 */
extern int
xfseek(FILE *stream, long offset, int whence);

/**
 * \brief  Creates a new file and ensures that it contains bytes number
 *         of bytes.
 *
 * \param  *fname  The name of the file to create.
 * \param  bytes   The file size in bytes.
 *
 * \return  Always returns 0.
 */
extern int
xfile_createFileWithSize(const char *fname, size_t bytes);

/**
 * \brief  Check whether a file exists.
 *
 * \param *fname  The name of the file whose existance should be
 *                checked.
 *
 * \returns Returns true if the file exists and false if not.
 */
extern bool
xfile_checkIfFileExists(const char *fname);

#endif
