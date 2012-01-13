// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/filename.c
 * @ingroup  libutilMiscFilename
 * @brief  This file implements the filename object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "filename.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "xmem.h"
#include "xstring.h"
#include "diediedie.h"


/*--- Implementation of main structure ----------------------------------*/
#include "filename_adt.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the empty string. */
static const char *local_emptyString = "";


/*--- Callback types ----------------------------------------------------*/

/**
 * @brief The signature of the function used to check the validity of
 *        characters
 */
typedef bool
(*local_checkFunc_t)(int c);


/*--- Implementation of inlined functions -------------------------------*/

/**
 * @brief  Helper function to verify the characters in a string.
 *
 * @param[in]  *value
 *                The string that should be checked.  Must be either @c NULL
 *                or a properly terminated string.
 * @param[i]   check
 *                The function that is applied to every character in the
 *                string to test its validity.
 *
 * @return  Returns nothing.
 */
static inline void
local_checkValue(const char *value, local_checkFunc_t check)
{
	if (value != NULL) {
		size_t length = strlen(value);
		for (size_t i = 0; i < length; i++) {
			if (!check(value[i])) {
				fprintf(stderr, "ERROR: %s is not a valid string\n", value);
				diediedie(EXIT_FAILURE);
			}
		}
	}
}

/**
 * @brief  Helper to set the path member of the main structure.
 *
 * @param[out]  **member
 *                 Pointer to the variable that should be set.  Passing
 *                 @c NULL is undefined.
 * @param[in]   *memberValue
 *                 The value that should be set.
 *
 * @return  Returns nothing.
 */
static inline void
local_setPathMember(char const **member, const char *memberValue)
{
	assert(member != NULL);

	if (memberValue == NULL) {
		*member = local_emptyString;
	} else {
		size_t length = strlen(memberValue);

		if (length == 0)
			*member = local_emptyString;
		else if (memberValue[length - 1] != '/')
			*member = xstrmerge(memberValue, "/");
		else
			*member = xstrdup(memberValue);
	}
}

/**
 * @brief  Helper to set an ordinary member of the main structure.
 *
 * @param[out]  **member
 *                 Pointer to the variable that should be set.  Passing
 *                 @c NULL is undefined.
 * @param[in]   *memberValue
 *                 The value that should be set.
 *
 * @return  Returns nothing.
 */
static inline void
local_setOrdinaryMember(char const **member, const char *memberValue)
{
	assert(member != NULL);

	if (memberValue == NULL) {
		*member = local_emptyString;
	} else {
		size_t length = strlen(memberValue);

		if (length == 0)
			*member = local_emptyString;
		else
			*member = xstrdup(memberValue);
	}
}

/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Helper function to check path characters.
 *
 * @param[in]  c
 *                Character to test.
 *
 * @return  Returns @c true if @c c is a valid path character.  Otherwise
 *          @c false is returned.
 */
static bool
local_checkPathCharacters(int c);


/**
 * @brief  Helper function to check non-path characters.
 *
 * @param[in]  c
 *                Character to test.
 *
 * @return  Returns @c true if @c c is a valid non-path character.
 *          Otherwise @c false is returned.
 */
static bool
local_checkNonPathCharacters(int c);


/**
 * @brief  Helper function to re-generate the full file name from its
 *         pieces.
 *
 * @param[in,out]  fn
 *                    The filename structure to work with.
 *
 * return  Returns nothing.
 */
static void
local_resetFullName(filename_t fn);


/*--- Implementations of exported functions -----------------------------*/
extern filename_t
filename_new(void)
{
	filename_t fn;

	fn            = xmalloc(sizeof(struct filename_struct));
	fn->path      = local_emptyString;
	fn->prefix    = local_emptyString;
	fn->qualifier = local_emptyString;
	fn->suffix    = local_emptyString;
	fn->fullName  = local_emptyString;

	return fn;
}

extern filename_t
filename_newFull(const char *path,
                 const char *prefix,
                 const char *qualifier,
                 const char *suffix)
{
	filename_t fn = filename_new();

	local_checkValue(path, &local_checkPathCharacters);
	local_setPathMember(&(fn->path), path);

	local_checkValue(prefix, &local_checkNonPathCharacters);
	local_setOrdinaryMember(&(fn->prefix), prefix);

	local_checkValue(qualifier, &local_checkNonPathCharacters);
	local_setOrdinaryMember(&(fn->qualifier), qualifier);

	local_checkValue(suffix, &local_checkNonPathCharacters);
	local_setOrdinaryMember(&(fn->suffix), suffix);

	local_resetFullName(fn);

	return fn;
}

extern void
filename_del(filename_t *fn)
{
	assert(fn != NULL && *fn != NULL);

	if ((*fn)->path != local_emptyString)
		xfree((char *)((*fn)->path));
	if ((*fn)->prefix != local_emptyString)
		xfree(((char *)(*fn)->prefix));
	if ((*fn)->qualifier != local_emptyString)
		xfree(((char *)(*fn)->qualifier));
	if ((*fn)->suffix != local_emptyString)
		xfree(((char *)(*fn)->suffix));
	if ((*fn)->fullName != local_emptyString)
		xfree(((char *)(*fn)->fullName));
	xfree(*fn);

	*fn = NULL;
}

extern void
filename_setPath(filename_t fn, const char *path)
{
	if (fn->path != local_emptyString)
		xfree((char *)(fn->path));
	local_checkValue(path, &local_checkPathCharacters);
	local_setPathMember(&(fn->path), path);
	local_resetFullName(fn);
}

extern void
filename_setPrefix(filename_t fn, const char *prefix)
{
	if (fn->prefix != local_emptyString)
		xfree((char *)(fn->prefix));
	local_checkValue(prefix, &local_checkNonPathCharacters);
	local_setOrdinaryMember(&(fn->prefix), prefix);
	local_resetFullName(fn);
}

extern void
filename_setQualifier(filename_t fn, const char *qualifier)
{
	if (fn->qualifier != local_emptyString)
		xfree((char *)(fn->qualifier));
	local_checkValue(qualifier, &local_checkNonPathCharacters);
	local_setOrdinaryMember(&(fn->qualifier), qualifier);
	local_resetFullName(fn);
}

extern void
filename_setSuffix(filename_t fn, const char *suffix)
{
	if (fn->suffix != local_emptyString)
		xfree((char *)(fn->suffix));
	local_checkValue(suffix, &local_checkNonPathCharacters);
	local_setOrdinaryMember(&(fn->suffix), suffix);
	local_resetFullName(fn);
}

extern const char *
filename_getPath(const filename_t fn)
{
	assert(fn != NULL);

	return fn->path;
}

extern const char *
filename_getPrefix(const filename_t fn)
{
	assert(fn != NULL);

	return fn->prefix;
}

extern const char *
filename_getQualifier(const filename_t fn)
{
	assert(fn != NULL);

	return fn->qualifier;
}

extern const char *
filename_getSuffix(const filename_t fn)
{
	assert(fn != NULL);

	return fn->suffix;
}

extern const char *
filename_getFullName(const filename_t fn)
{
	assert(fn != NULL);

	return fn->fullName;
}

/*--- Implementations of local functions --------------------------------*/
static bool
local_checkPathCharacters(int c)
{
	return isprint(c);
}

static bool
local_checkNonPathCharacters(int c)
{
	return isprint(c) && (c != '/');
}

static void
local_resetFullName(filename_t fn)
{
	assert(fn != NULL);

	char   *tmp;
	size_t lengthPath      = strlen(fn->path);
	size_t lengthPrefix    = strlen(fn->prefix);
	size_t lengthQualifier = strlen(fn->qualifier);
	size_t lengthSuffix    = strlen(fn->suffix);
	size_t lengthFull      = lengthPath + lengthPrefix + lengthQualifier
	                         + lengthSuffix;

	if (fn->fullName != local_emptyString)
		xfree((char *)(fn->fullName));

	tmp          = xmalloc(sizeof(char) * (lengthFull + 1));
	fn->fullName = tmp;

	strncpy(tmp, fn->path, lengthPath);
	tmp   += lengthPath;
	strncpy(tmp, fn->prefix, lengthPrefix);
	tmp   += lengthPrefix;
	strncpy(tmp, fn->qualifier, lengthQualifier);
	tmp   += lengthQualifier;
	strncpy(tmp, fn->suffix, lengthSuffix);
	tmp   += lengthSuffix;
	tmp[0] = '\0';
}
