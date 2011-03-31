// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef PARSE_INI_H
#define PARSE_INI_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/parse_ini.h
 * @ingroup libutilFilesIni
 * @brief  This file provides the interface of the INI file type.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


/*--- Exported defines --------------------------------------------------*/
#define getFromIni(trgt, func, ini, keyName, sectionName)      \
    if (!func(ini, keyName, sectionName, trgt)) {              \
		fprintf(stderr,                                        \
		        "FATAL:  Could not get %s from section %s.\n", \
		        keyName, sectionName);                         \
		exit(EXIT_FAILURE);                                    \
	}


/*--- ADT handle --------------------------------------------------------*/
typedef struct parse_ini_struct parse_ini_struct_t;
typedef parse_ini_struct_t      *parse_ini_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * \brief  Opens an ini file and creates an according object to work
 *         with.
 *
 * \param  *fname  The filename of the ini file.
 *
 * \return  Returns a pointer to a correctly initialized ini file
 *          structure.
 */
extern parse_ini_t
parse_ini_open(const char *fname);


/**
 * \brief  Closes an ini file and destroys the associated object.
 *
 * \param  *ini  Pointer to the external variable holding the object.
 *
 * \return  Returns nothing.
 */
extern void
parse_ini_close(parse_ini_t *ini);


/**
 * \brief  Dumps the ini file structure to a file pointer.
 *
 * \param  ini  The ini structure to dump.
 * \param  *f   The stream to dump to.
 *
 * \return  Returns nothing.
 */
extern void
parse_ini_dump(parse_ini_t ini, FILE *f);


/**
 * \brief  Returns the value of a given key in a given section as a
 *         signed 32bit integer.
 *
 * \param  ini            The ini file to work on.
 * \param  *key_name      The key name to look for.
 * \param  *section_name  The section in which to look.
 * \param  *value         A pointer to the variable that will hold the
 *                        result. This will only be set if the function
 *                        returns true.
 *
 * \return  Returns true if the value was found and something got passed
 *          back via the result pointer, otherwise false is returned.
 */
extern bool
parse_ini_get_int32(parse_ini_t ini,
                    const char  *key_name,
                    const char  *section_name,
                    int32_t     *value);


/**
 * \brief  Returns the value of a given key in a given section as an
 *         unsigned 32bit integer.
 *
 * \param  ini            The ini file to work on.
 * \param  *key_name      The key name to look for.
 * \param  *section_name  The section in which to look.
 * \param  *value         A pointer to the variable that will hold the
 *                        result. This will only be set if the function
 *                        returns true.
 *
 * \return  Returns true if the value was found and something got passed
 *          back via the result pointer, otherwise false is returned.
 */
extern bool
parse_ini_get_uint32(parse_ini_t ini,
                     const char  *key_name,
                     const char  *section_name,
                     uint32_t    *value);


/**
 * \brief  Returns the value of a given key in a given section as a
 *         signed 64bit integer.
 *
 * \param  ini            The ini file to work on.
 * \param  *key_name      The key name to look for.
 * \param  *section_name  The section in which to look.
 * \param  *value         A pointer to the variable that will hold the
 *                        result. This will only be set if the function
 *                        returns true.
 *
 * \return  Returns true if the value was found and something got passed
 *          back via the result pointer, otherwise false is returned.
 */
extern bool
parse_ini_get_int64(parse_ini_t ini,
                    const char  *key_name,
                    const char  *section_name,
                    int64_t     *value);


/**
 * \brief  Returns the value of a given key in a given section as an
 *         unsigned 32bit integer.
 *
 * \param  ini            The ini file to work on.
 * \param  *key_name      The key name to look for.
 * \param  *section_name  The section in which to look.
 * \param  *value         A pointer to the variable that will hold the
 *                        result. This will only be set if the function
 *                        returns true.
 *
 * \return  Returns true if the value was found and something got passed
 *          back via the result pointer, otherwise false is returned.
 */
extern bool
parse_ini_get_uint64(parse_ini_t ini,
                     const char  *key_name,
                     const char  *section_name,
                     uint64_t    *value);


/**
 * \brief  Returns the value of a given key in a given section as a
 *         double.
 *
 * \param  ini            The ini file to work on.
 * \param  *key_name      The key name to look for.
 * \param  *section_name  The section in which to look.
 * \param  *value         A pointer to the variable that will hold the
 *                        result. This will only be set if the function
 *                        returns true.
 *
 * \return  Returns true if the value was found and something got passed
 *          back via the result pointer, otherwise false is returned.
 */
extern bool
parse_ini_get_double(parse_ini_t ini,
                     const char  *key_name,
                     const char  *section_name,
                     double      *value);


/**
 * \brief  Returns the value of a given key in a given section as a
 *         signed 32bit integer.
 *
 * \param  ini            The ini file to work on.
 * \param  *key_name      The key name to look for.
 * \param  *section_name  The section in which to look.
 * \param  **value        A pointer to the variable that will hold the
 *                        result. This will only be set if the function
 *                        returns true.
 *
 * \return  Returns true if the value was found and something got passed
 *          back via the result pointer, otherwise false is returned.
 */
extern bool
parse_ini_get_string(parse_ini_t ini,
                     const char  *key_name,
                     const char  *section_name,
                     char        **value);


/**
 * \brief  Returns the value of a given key in a given section as a
 *         boolean.
 *
 * \param  ini            The ini file to work on.
 * \param  *key_name      The key name to look for.
 * \param  *section_name  The section in which to look.
 * \param  **value        A pointer to the variable that will hold the
 *                        result. This will only be set if the function
 *                        returns true.
 *
 * \return  Returns true if the value was found and something got passed
 *          back via the result pointer, otherwise false is returned.
 */
extern bool
parse_ini_get_bool(parse_ini_t ini,
                   const char  *key_name,
                   const char  *section_name,
                   bool        *value);


/**
 * \brief  Returns a list of strings, which are separated by
 *         white-spaces.
 *
 * \param ini            The ini file to work on.
 * \param *key_name      The key name to look for.
 * \param *section_name  The section in which to look.
 * \param num_values     The number of values expected in this key.
 * \param ***values      A pointer to the variable that will hold the
 *                       result. This will only be set of the function
 *                       returns true.
 *
 * \return  Returns true if the string list was found, well-formed and
 *          contained the correct number of elemets.  Otherwise false is
 *          returned.
 */
extern bool
parse_ini_get_stringlist(parse_ini_t ini,
                         const char  *key_name,
                         const char  *section_name,
                         uint32_t    num_values,
                         char        ***values);


/**
 * \brief  Returns the list of values.
 *
 * \param  ini            The ini file to work on.
 * \param  *key_name      The key name to look for.
 * \param  *section_name  The section in which to look.
 * \param  num_values     The numer of values expected in this key.
 * \param  **value        A pointer to the variable that will hold the
 *                        result. This will only be set if the function
 *                        returns true.
 *
 * \return  Returns true if the value was found and something got passed
 *          back via the result pointer, otherwise false is returned.
 */
extern bool
parse_ini_get_int32list(parse_ini_t ini,
                        const char  *key_name,
                        const char  *section_name,
                        uint32_t    num_values,
                        int32_t     **values);


#endif
