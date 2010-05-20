// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#include "xstring.h"
#include "xmem.h"
#include <string.h>


extern char *
xstrdup(const char *s)
{
	char   *dummy;
	size_t length;

	if (s == NULL)
		return NULL;

	length = strlen(s) + 1;
	dummy  = xmalloc(sizeof(char) * length);
	memcpy(dummy, s, length);

	return dummy;
}

extern size_t
xgetline(char **line, size_t *n, FILE *f)
{
	size_t num_chars = 0;
	long   offset;
	int    c, oldc;
	size_t i;

	/* Sanity check */
	if ((line == NULL) || (n == NULL) || (f == NULL))
		return -1;

	/* Count the number of characters that we need to read */
	offset = ftell(f);
	oldc   = 'a';
	while ((oldc != '\n') && ((c = fgetc(f)) != EOF)) {
		oldc = c;
		num_chars++;
	}
	offset -= ftell(f);
	fseek(f, offset, SEEK_CUR);

	/* Verify that the buffer is large enough, to be NULL terminated we
	 * need to allocate one more byte than characters to read.
	 */
	if (*line == NULL) {
		*n    = (num_chars + 1);
		*line = xmalloc(sizeof(char) * (*n));
	} else {
		if (*n < (num_chars + 1)) {
			*line = xrealloc(*line, (num_chars + 1));
			*n    = (num_chars + 1);
		}
	}

	/* Read the line in */
	for (i = 0; i < num_chars; i++)
		(*line)[i] = (char)fgetc(f);

	/* And NULL terminate it */
	(*line)[i] = '\0';

	/* And be done */
	return num_chars;
} /* xgetline */

extern char *
xstrmerge(const char *s1, const char *s2)
{
	char *s;
	size_t numCharsInS1 = strlen(s1);
	size_t numCharsInS2 = strlen(s2);

	s = xmalloc(sizeof(char) * (numCharsInS1 + numCharsInS2 + 1));

	memcpy(s, s1, numCharsInS1);
	memcpy(s+numCharsInS1, s2, numCharsInS2);

	s[numCharsInS1+numCharsInS2] = '\0';

	return s;
}

