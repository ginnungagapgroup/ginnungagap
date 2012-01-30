// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef FILENAME_H
#define FILENAME_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/filename.h
 * @ingroup  libutilMiscFilename
 * @brief  This file provides the interface to the filename object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct filename_struct *filename_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new (empty) filename object.
 *
 * @return  Returns a new filename object that does not describe a valid
 *          file.
 */
extern filename_t
filename_new(void);


/**
 * @brief  Creates a new (minimal) filename object.
 *
 * @param[in]  *path
 *                The path to the directory in which the file is located.
 *                If provided, this must be a string of valid directoy
 *                characters including '/'.  The last character should be
 *                '/', but if it is not, the trailing '/' it will be
 *                appended.
 * @param[in]  *prefix
 *                The prefix of the filename.  If provided, this must be a
 *                string of valid filename characters.  The string must not
 *                contain any '/'.
 * @param[in]  *qualifier
 *                The qualifier of the file.  If provided, this must be a
 *                string of valid filename characters.  The string
 *                must not contain any '/'.
 * @param[in]  *suffix
 *                The suffic of the file.  If provided, this must be a
 *                string of valid filename characters.  The string
 *                must not contain any '/'.
 *
 * @return  Returns a new filename object that has
 */
extern filename_t
filename_newFull(const char *path,
                 const char *prefix,
                 const char *qualifier,
                 const char *suffix);


/**
 * @brief  Creates a clone of the provided filename object.
 *
 * @param[in]  fn
 *                The file name object that should be cloned.
 *
 * @return  Returns a new filename objects that has the same values as the
 *          original one.
 */
extern filename_t
filename_clone(const filename_t fn);

/**
 * @brief  Destroys a filename oject and frees all associated memory.
 *
 * @param[in,out]  *fn
 *                    A pointer to the filename object that should be
 *                    deleted.  The external variable holding the filename
 *                    object will be set to @c NULL.  Passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
extern void
filename_del(filename_t *fn);


/** @} */

/**
 * @brief  Setting
 *
 * @{
 */

/**
 * @brief  Sets the path of the filename.
 *
 * @param[in,out]  fn
 *                    The filename object to modify.
 * @param[in]      *path
 *                    The path of the file.  See filename_newFull() for the
 *                    allowed characters in this string.  Passing @c NULL is
 *                    allowed and equivalent to passing the empty string.
 *
 * @return  Returns nothing.
 */
extern void
filename_setPath(filename_t fn, const char *path);


/**
 * @brief  Sets the prefix of the filename.
 *
 * @param[in,out]  fn
 *                    The filename object to modify.
 * @param[in]      *prefix
 *                    The prefix of the file.  See filename_newFull() for
 *                    the allowed characters in this string.  Passing @c
 *                    NULL is  allowed and equivalent to passing the empty
 *                    string.
 *
 * @return  Returns nothing.
 */
extern void
filename_setPrefix(filename_t fn, const char *prefix);

/**
 * @brief  Sets the qualifier of the filename.
 *
 * @param[in,out]  fn
 *                    The filename object to modify.
 * @param[in]      *qualifier
 *                    The qualifier of the file.  See filename_newFull() for
 *                    the allowed characters in this string.  Passing @c
 *                    NULL is  allowed and equivalent to passing the empty
 *                    string.
 *
 * @return  Returns nothing.
 */
extern void
filename_setQualifier(filename_t fn, const char *qualifier);


/**
 * @brief  Sets the suffix of the filename.
 *
 * @param[in,out]  fn
 *                    The filename object to modify.
 * @param[in]      *suffix
 *                    The suffix of the file.  See filename_newFull() for
 *                    the allowed characters in this string.  Passing @c
 *                    NULL is  allowed and equivalent to passing the empty
 *                    string.
 *
 * @return  Returns nothing.
 */
extern void
filename_setSuffix(filename_t fn, const char *suffix);


/** @} */

/**
 * @brief  Getting
 *
 * @{
 */

/**
 * @brief  Retrieves the path part of the full file name.
 *
 * @param[in]  fn
 *                The filename object for which to retrieve the full name.
 *                Passing @c NULL is undefined.
 *
 * @return  Returns a read-only string containing the path.
 */
extern const char *
filename_getPath(const filename_t fn);

/**
 * @brief  Retrieves the prefix part of the full file name.
 *
 * @param[in]  fn
 *                The filename object for which to retrieve the full name.
 *                Passing @c NULL is undefined.
 *
 * @return  Returns a read-only string containing the prefix.
 */
extern const char *
filename_getPrefix(const filename_t fn);

/**
 * @brief  Retrieves the qualifier part of the full file name.
 *
 * @param[in]  fn
 *                The filename object for which to retrieve the full name.
 *                Passing @c NULL is undefined.
 *
 * @return  Returns a read-only string containing the qualifier.
 */
extern const char *
filename_getQualifier(const filename_t fn);

/**
 * @brief  Retrieves the suffix part of the full file name.
 *
 * @param[in]  fn
 *                The filename object for which to retrieve the full name.
 *                Passing @c NULL is undefined.
 *
 * @return  Returns a read-only string containing the suffix.
 */
extern const char *
filename_getSuffix(const filename_t fn);

/** @} */

/**
 * @brief  Using
 *
 * @{
 */

/**
 * @brief  Retrieves the full name.
 *
 * @param[in]  fn
 *                The filename object for which to retrieve the full name.
 *                Passing @c NULL is undefined.
 *
 * @return  Returns a read-only string containing the full name.
 */
extern const char *
filename_getFullName(const filename_t fn);

/**
 * @brief  Sets the payload field of a filename object to the set fields in
 *         a template one.
 *
 * This will only update the fields in the target which a corresponding set
 * field in the template.  If the target field was already set, it will be
 * replaced with the one in the template.
 *
 * @param[in,out]  trgt
 *                    The filename object that should be updated.  Passing
 *                    @c NULL is undefined.
 * @param[in]      tmpl
 *                    The template from which to read the new fields.
 *                    Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
filename_copySetFields(filename_t trgt, const filename_t tmpl);

/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilMiscFilename  Filename
 * @ingroup libutilMisc
 * @brief Provides and object to deal with file names.
 */

#endif
