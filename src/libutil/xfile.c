// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/xfile.c
 * @ingroup libutilCore
 * @brief This file implements the utility functions dealing with files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "xfile.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#if (defined _XOPEN_SOURE && _XOPEN_SOURCE >= 600)
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#endif


/*--- Implementations of exported functios ------------------------------*/
extern FILE *
xfopen(const char *path, const char *mode)
{
	FILE *dummy = fopen(path, mode);

	if (dummy == NULL) {
		fprintf(stderr, "Could not open %s with mode %s..\n",
		        path, mode);
		fprintf(stderr, "Exiting... :-(\n");
		exit(EXIT_FAILURE);
	}

	return dummy;
}

extern int
xfclose(FILE **fp)
{
	if ((fp == NULL) || ((*fp) == NULL))
		return 0;

	fclose(*fp);
	*fp = NULL;

	return 0;
}

extern size_t
xfread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	if (fread(ptr, size, nmemb, stream) != nmemb) {
		fprintf(stderr,
		        "Expected to read %li (%lib each) but got less.\n",
		        (long)nmemb, (long)size);
		fprintf(stderr, "Exiting... :-(\n");
		exit(EXIT_FAILURE);
	}

	return nmemb;
}

extern size_t
xfwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	if (fwrite(ptr, size, nmemb, stream) != nmemb) {
		fprintf(stderr,
		        "Expected to write %li (%lib each) but wrote less.\n",
		        (long)nmemb, (long)size);
		fprintf(stderr, "Exiting... :-(\n");
		exit(EXIT_FAILURE);
	}

	return nmemb;
}

extern int
xfseek(FILE *stream, long offset, int whence)
{
	if (fseek(stream, offset, whence) != 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		fprintf(stderr, "Exiting... :-(\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

extern int
xfile_createFileWithSize(const char *fname, size_t bytes)
{
#if (defined _XOPEN_SOURE && _XOPEN_SOURCE >= 600)
	int fd;
	int errnum;

	fd = creat(fname, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1) {
		errnum = errno;
		fprintf(stderr, "Error in %s:%i %s\n",
		        __func__, __LINE__, strerror(errnum));
		exit(EXIT_FAILURE);
	}
	errnum = posix_fallocate(fd, (off_t)0, (off_t)bytes);
	if (errnum != 0) {
		fprintf(stderr, "Error in %s:%i %i\n",
		        __func__, __LINE__, errnum);
		exit(EXIT_FAILURE);
	}
	close(fd);
#else
	FILE *f;
	char nullData = 124;

	f = xfopen(fname, "wb");

	for (int i = 0; i < bytes; i++)
		xfwrite(&nullData, sizeof(char), 1, f);

	xfclose(&f);
#endif

	return 0;
} /* file_createWithSize */

extern bool
xfile_checkIfFileExists(const char *fname)
{
	FILE *f;
	bool fileExists = false;

	assert(fname != NULL);

	f = fopen(fname, "r");
	if (f != NULL) {
		fclose(f);
		fileExists = true;
	}

	return fileExists ? true : false;
}
