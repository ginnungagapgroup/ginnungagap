// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/parse_ini.c
 * @ingroup libutilFilesIni
 * @brief  This file provides the implementation of the ini parser.
 */


/*--- Includes ----------------------------------------------------------*/
#include "parse_ini.h"
#include "xmem.h"
#include "xstring.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


/*--- Implemention of local structures ----------------------------------*/

/** @brief  Short name for the section structure. */
typedef struct local_section_struct local_section_struct_t;

/** @brief  Handle to the section structure. */
typedef local_section_struct_t      *local_section_t;

/** @brief  Short name for the key structure. */
typedef struct local_key_struct     local_key_struct_t;

/** @brief  Handle to a key structure. */
typedef local_key_struct_t          *local_key_t;

/**
 * \brief  Helper structure for the sections.
 */
struct local_section_struct {
	/** The name of the section. */
	char        *name;
	/** The number of keys in this section */
	int         num_keys;
	/** And a list of keys */
	local_key_t keys;
};

/**
 * \brief  Helper structure for the key-value pairs.
 */
struct local_key_struct {
	/** The name of the key */
	char *key_name;
	/** The value of the key */
	char *value;
	/** Set this to true once the value was requested */
	bool requested;
};

/**
 * \brief  Describes the ini file structure.
 */
struct parse_ini_struct {
	/** Keep track of the file name */
	char            *fname;
	/** Keep track of the file pointer */
	FILE            *f;
	/** The number of sections */
	int             num_sections;
	/** An ordered list of all sections */
	local_section_t sections;
	/** The section we are currently in */
	int             cursec;
};


/*--- Prototypes of local functions -------------------------------------*/

/**
 * \brief  Parses a line into a section name
 *
 * \param  *line  The line to parse.
 *
 * \return  Returns a newly allocated, NULL terminated string holding
 *          the section name.
 */
static char *
local_parse_secname(const char *line);


/**
 * \brief  Parses a line into a keyname and it's value.
 *
 * \param  *line       The line to parse.
 * \param  **key_name  Pointer to the variable that will hold the key
 *                     name.
 * \param  *value      Pointer to the variable that will hold the value.
 *
 * \return  Returns nothing, however, the key_name an the value will be
 *          set with accordingly allocated strings.
 */
static void
local_parse_key(const char *line, char **key_name, char **value);


/**
 * \brief  Finds a section in a given ini file.
 *
 * \param  ini            The ini file to look in.
 * \param  *section_name  The section to look for.
 *
 * \return  Returns the number of the section, or -1 if the section
 *          could not be found.
 */
static int
local_find_section(parse_ini_t ini, const char *section_name);


/**
 * \brief  Finds a key in a given section.
 *
 * \param  sec        The section to look in.
 * \param  *key_name  The key name to search for.
 *
 * \return  Returns the number of the key within the section structure,
 *          or -1, if the key could not be found.
 */
static int
local_find_key(local_section_t sec, const char *key_name);


/*--- Implementations of exported functios ------------------------------*/
extern parse_ini_t
parse_ini_open(const char *fname)
{
	parse_ini_t     dummy    = NULL;
	local_section_t sec      = NULL;
	local_key_t     key      = NULL;
	char            *line    = NULL;
	size_t          n        = 0;
	size_t          line_len = 0;
	int             i;

	/* Get memory */
	dummy = xmalloc(sizeof(parse_ini_struct_t));

	/* Try to open the file */
	dummy->f = fopen(fname, "r");
	if (dummy->f == NULL) {
		xfree(dummy);
		return NULL;
	}

	/* Copy the filename over */
	dummy->fname = xstrdup(fname);

	/* Some init stuff */
	dummy->sections     = NULL;
	dummy->num_sections = 0;
	dummy->cursec       = -1;

	/* Parse the file */
	while (xgetline(&line, &n, dummy->f) > 0) {
		/* Check for empty (only blanks) line, wipe it */
		i        = 0;
		line_len = strlen(line);
		while ((line[i] != '\0') && (isblank(line[i])))
			i++;
		if (i == (int)line_len - 1) {
			line[0] = '\0';
		}

		switch (line[0]) {
		case '\0':
		/* This is an empty line, we do nothing. */
		case '#':
			/* Ignore comment lines */
			break;
		case '[':
			/* This is a new section */
			dummy->num_sections++;
			/* Get memory for it  */
			dummy->sections = xrealloc(dummy->sections,
			                           dummy->num_sections
			                           * sizeof(local_section_struct_t));
			/* Use a shortcut to access the current section */
			sec           = dummy->sections + dummy->num_sections - 1;
			/* Initialize the section structure correctly */
			sec->name     = local_parse_secname(line);
			sec->num_keys = 0;
			sec->keys     = NULL;
			/* Verify the section structure */
			if (sec->name == NULL) {
				/* Wrong file format. We die. */
				fprintf(stderr, "Could not parse '%s'\n", line);
				xfree(line);
				parse_ini_close(&dummy);
				return NULL;
			}
			break;
		default:
			/* This is potentially a key */
			if (sec == NULL) {
				/* Huh.. we haven't had a section yet, so lets
				 * ignore this gibberish.
				 */
			} else {
				/* Okay, it is a new key */
				sec->num_keys++;
				/* Get memory for it */
				sec->keys = xrealloc(sec->keys,
				                     sec->num_keys
				                     * sizeof(local_key_struct_t));
				/* Use a shortcut */
				key = sec->keys + (sec->num_keys - 1);
				/* Initialize the key correctly */
				local_parse_key(line, &(key->key_name), &(key->value));
				/* Check that it worked */
				if ((key->key_name == NULL) || (key->value == NULL)) {
					/* Wrong file format. We die. */
					fprintf(stderr, "Could not parse '%s'\n", line);
					xfree(line);
					parse_ini_close(&dummy);
					return NULL;
				}
				/* This key has not yet been requested */
				key->requested = false;
			}
		} /* switch */
	} /* while */

	/* Clean */
	fclose(dummy->f);
	dummy->f = NULL;
	xfree(line);

	/* And done */
	return dummy;
} /* parse_ini_open */

extern void
parse_ini_close(parse_ini_t *ini)
{
	local_section_t sec;
	local_key_t     key;
	int             i, j;

	if ((ini == NULL) || (*ini == NULL))
		return;

	/* Loop over all sections and free keys */
	for (i = 0; i < (*ini)->num_sections; i++) {
		sec = (*ini)->sections + i;
		for (j = 0; j < sec->num_keys; j++) {
			key = sec->keys + j;
			xfree(key->key_name);
			xfree(key->value);
		}
		xfree(sec->keys);
		xfree(sec->name);
	}
	xfree((*ini)->sections);

	/* Free the thing */
	if ((*ini)->f != NULL)
		fclose((*ini)->f);
	xfree((*ini)->fname);
	xfree(*ini);

	/* Remove the reference */
	*ini = NULL;

	return;
} /* parse_ini_close */

extern void
parse_ini_dump(parse_ini_t ini, FILE *f)
{
	int             i, j;
	local_section_t sec;
	local_key_t     key;

	/* Sanity check */
	if ((ini == NULL) || (f == NULL))
		return;

	/* Dump everything */
	fprintf(f, "# Dump based on ini structure from %s\n\n",
	        ini->fname);
	for (i = 0; i < ini->num_sections; i++) {
		sec = ini->sections + i;
		fprintf(f, "[%s]\n", sec->name);
		for (j = 0; j < sec->num_keys; j++) {
			key = sec->keys + j;
			if (!key->requested)
				fprintf(f, "# IGNORED:  ");
			fprintf(f, "%s = %s\n", key->key_name, key->value);
		}
		fprintf(f, "\n");
	}

	/* Done */
	return;
} /* parse_ini_dump */

extern bool
parse_ini_get_int32(parse_ini_t ini,
                    const char  *key_name,
                    const char  *section_name,
                    int32_t     *value)
{
	int     sec, key, rtn;
	int32_t tmp;
	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key */
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Parse the value */
	rtn = sscanf(ini->sections[sec].keys[key].value, "%" SCNi32, &tmp);
	/* Check if the parsing went well */
	if (rtn != 1)
		return false;

	/* Set the value */
	*value                                 = tmp;
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_int32 */

extern bool
parse_ini_get_uint32(parse_ini_t ini,
                     const char  *key_name,
                     const char  *section_name,
                     uint32_t    *value)
{
	int      sec, key, rtn;
	uint32_t tmp;
	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key */
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Parse the value */
	rtn = sscanf(ini->sections[sec].keys[key].value, "%" SCNu32, &tmp);
	/* Check if the parsing went well */
	if (rtn != 1)
		return false;

	/* Set the value */
	*value                                 = tmp;
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_uint32 */

extern bool
parse_ini_get_int64(parse_ini_t ini,
                    const char  *key_name,
                    const char  *section_name,
                    int64_t     *value)
{
	int     sec, key, rtn;
	int64_t tmp;
	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key */
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Parse the value */
	rtn = sscanf(ini->sections[sec].keys[key].value, "%" SCNi64, &tmp);
	/* Check if the parsing went well */
	if (rtn != 1)
		return false;

	/* Set the value */
	*value                                 = tmp;
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_int64 */

extern bool
parse_ini_get_uint64(parse_ini_t ini,
                     const char  *key_name,
                     const char  *section_name,
                     uint64_t    *value)
{
	int      sec, key, rtn;
	uint64_t tmp;
	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key */
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Parse the value */
	rtn = sscanf(ini->sections[sec].keys[key].value, "%" SCNu64, &tmp);
	/* Check if the parsing went well */
	if (rtn != 1)
		return false;

	/* Set the value */
	*value                                 = tmp;
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_uint64 */

extern bool
parse_ini_get_double(parse_ini_t ini,
                     const char  *key_name,
                     const char  *section_name,
                     double      *value)
{
	int    sec, key, rtn;
	double tmp;
	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key */
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Parse the value */
	rtn = sscanf(ini->sections[sec].keys[key].value, "%lf", &tmp);
	/* Check if the parsing went well */
	if (rtn != 1)
		return false;

	/* Set the value */
	*value                                 = tmp;
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_double */

extern bool
parse_ini_get_string(parse_ini_t ini,
                     const char  *key_name,
                     const char  *section_name,
                     char        **value)
{
	int  sec, key;
	char *tmp = NULL;
	/* Sanity check */
	if (value == NULL)
		return false;

	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key */
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Duplicate the value */
	tmp = xstrdup(ini->sections[sec].keys[key].value);
	/* Check if the parsing went well */
	if (tmp == NULL)
		return false;

	*value                                 = tmp;
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_string */

extern bool
parse_ini_get_bool(parse_ini_t ini,
                   const char  *key_name,
                   const char  *section_name,
                   bool        *value)
{
	int sec, key;
	/* Sanity check */
	if (value == NULL)
		return false;

	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key */
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Duplicate the value */
	if ((strcmp(ini->sections[sec].keys[key].value, "true") == 0)
	    || (strcmp(ini->sections[sec].keys[key].value, "yes") == 0)
	    || (strcmp(ini->sections[sec].keys[key].value, "1") == 0)) {
		*value = true;
	} else if ((strcmp(ini->sections[sec].keys[key].value, "false") == 0)
	           || (strcmp(ini->sections[sec].keys[key].value, "no") == 0)
	           || (strcmp(ini->sections[sec].keys[key].value, "0") == 0)) {
		*value = false;
	} else {
		/* Did not parse as a boolean! */
		return false;
	}
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_bool */

extern bool
parse_ini_get_stringlist(parse_ini_t ini,
                         const char  *key_name,
                         const char  *section_name,
                         uint32_t    num_values,
                         char        ***values)
{
	int      sec, key;
	uint32_t i;
	size_t   j, k, key_length;
	char     *keyval;
	/* Sanity check */
	if (values == NULL)
		return false;

	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key*/
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Set helpful variables */
	keyval     = ini->sections[sec].keys[key].value;
	key_length = strlen(keyval);
	/* Get memory for the values */
	*values    = xmalloc(sizeof(char *) * num_values);
	/* Chunk the key value into substrings */
	for (i = 0, j = 0; i < num_values; i++) {
		/* Ignore leading white-space */
		while (j < key_length && isspace(keyval[j]))
			j++;
		if (j >= key_length) {
			/* Not good.. we need to clean up */
			while (i > 0) {
				xfree((*values)[i - 1]);
				i--;
			}
			xfree(*values);
			return false;
		}
		/* Remember where the value starts */
		k = j;
		/* Find the end of the value */
		while (j < key_length && !isspace(keyval[j]))
			j++;
		if (j >= key_length) {
			if ((j == key_length) && (keyval[j] == '\0')) {
			} else {
				/* Not good.. we need to clean up */
				while (i > 0) {
					xfree((*values)[i - 1]);
					i--;
				}
				xfree(*values);
				return false;
			}
		}
		/* Get memory for the subvalue and copy it over */
		(*values)[i]        = xmalloc(sizeof(char) * (j - k + 1));
		memcpy((*values)[i], keyval + k, j - k);
		(*values)[i][j - k] = '\0';
	}
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_stringlist */

extern bool
parse_ini_get_int32list(parse_ini_t ini,
                        const char  *key_name,
                        const char  *section_name,
                        uint32_t    num_values,
                        int32_t     **values)
{
	int      sec, key;
	uint32_t i;
	size_t   j, k, key_length;
	char     *keyval;
	/* Sanity check */
	if (values == NULL)
		return false;

	/* Find the section */
	sec = local_find_section(ini, section_name);
	if (sec == -1)
		return false;

	/* Now find the key*/
	key = local_find_key(ini->sections + sec, key_name);
	if (key == -1)
		return false;

	/* Set helpful variables */
	keyval     = ini->sections[sec].keys[key].value;
	key_length = strlen(keyval);
	/* Get memory for the values */
	*values    = xmalloc(sizeof(int32_t *) * num_values);
	/* Chunk the key value into substrings */
	for (i = 0, j = 0; i < num_values; i++) {
		/* Ignore leading white-space */
		while (j < key_length && isspace(keyval[j]))
			j++;
		if (j >= key_length) {
			/* Not good.. we need to clean up */
			xfree(*values);
			return false;
		}
		/* Remember where the value starts */
		k = j;
		/* Find the end of the value */
		while (j < key_length && !isspace(keyval[j]))
			j++;
		if (j >= key_length) {
			if ((j == key_length) && (keyval[j] == '\0')) {
			} else {
				/* Not good.. we need to clean up */
				xfree(*values);
				return false;
			}
		}
		/* Parse the data */
		sscanf(keyval + k, "%" PRIi32, (*values) + i);
	}
	/* This was now (successfully) requested */
	ini->sections[sec].keys[key].requested = true;
	/* Success! */
	return true;
} /* parse_ini_get_int32list */

/*--- Implementations of local functions --------------------------------*/
static char *
local_parse_secname(const char *line)
{
	size_t n           = 1;
	size_t secname_len = 0;
	char   *rtn;

	/* Count all alphanumeric character in the section name */
	while ((line[n] != '\0') && (line[n] != ']')) {
		if (isalnum(line[n]))
			secname_len++;
		n++;
	}

	/* Check for bad section name */
	if (secname_len == 0)
		return NULL;

	/* Get a string that is large enough to also contain the terminating
	 * NULL.
	 */
	rtn = xmalloc(sizeof(char) * (secname_len + 1));

	/* Copy the section name over */
	n           = 1;
	secname_len = 0;
	while ((line[n] != '\0') && (line[n] != ']')) {
		if (isalnum(line[n])) {
			rtn[secname_len] = line[n];
			secname_len++;
		}
		n++;
	}

	/* Correctly terminate*/
	rtn[secname_len] = '\0';

	/* Done! */
	return rtn;
} /* local_parse_secname */

static void
local_parse_key(const char *line, char **key_name, char **value)
{
	int start     = 0;
	int end;
	int separator = 0;

	/* Initialize the result strings */
	*key_name = NULL;
	*value    = NULL;

	/* Ignore leading blanks */
	while (isblank(line[start]))
		start++;

	/* Find the key-value separator */
	separator = start;
	while ((line[separator] != '\0') && (line[separator] != '='))
		separator++;

	/* Sanity check */
	if (separator == start)
		return;

	/* Find the end of the key name */
	end = separator - 1;
	while (isblank(line[end]))
		end--;

	/* Generate the key name, correct for the terminating NULL and the
	 * fact that end is the position of the last character that still
	 * belongs to the name, hence the +2.
	 */
	*key_name                    = xmalloc(sizeof(char) * (end - start + 2));
	memcpy(*key_name, line + start, (end - start + 1));
	(*key_name)[end - start + 1] = '\0';

	/* Now find the beginning of the value */
	if (line[separator] == '\0') {
		start = separator;
	} else {
		start = separator + 1;
		while ((line[start] != '\0') && isblank(line[start]))
			start++;
	}
	/* If there is no value, return now. */
	if (line[start] == '\0')
		return;

	/* And now go to the end of the value */
	end = start + 1;
	while (line[end] != '\0')
		end++;
	/* Ignore the whitespace at the end */
	end--;
	while (isblank(line[end]) || (line[end] == '\n'))
		end--;

	/* Generate the value, correct for the terminating NULL and the fact
	 * that end points to the last char of the value.
	 */
	*value                    = xmalloc(sizeof(char) * (end - start + 2));
	memcpy(*value, line + start, (end - start + 1));
	(*value)[end - start + 1] = '\0';


	return;
} /* local_parse_key */

static int
local_find_section(parse_ini_t ini, const char *section_name)
{
	int             i;
	int             len;
	int             search_len = strlen(section_name);
	local_section_t sec;

	/* Iterate over all section and find the matching one */
	for (i = 0; i < ini->num_sections; i++) {
		sec = ini->sections + i;
		len = strlen(sec->name);
		/* Check if this is it */
		if ((len == search_len)
		    && (strncmp(section_name, sec->name, len) == 0))
			return i;
	}

	/* We didn't find it */
	return -1;
} /* local_find_section */

static int
local_find_key(local_section_t sec, const char *key_name)
{
	int         i;
	int         len;
	int         search_len = strlen(key_name);
	local_key_t key;

	/* Iterarte of all keys */
	for (i = 0; i < sec->num_keys; i++) {
		key = sec->keys + i;
		len = strlen(key->key_name);
		/* Check if this is it */
		if ((len == search_len)
		    && (strncmp(key_name, key->key_name, len) == 0))
			return i;
	}

	/* We didn't find it */
	return -1;
} /* local_find_key */
