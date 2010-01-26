// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#include "xfile.h"
#include <stdlib.h>
#include <stdio.h>


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
	if (!fseek(stream, offset, whence)) {
		fprintf(stderr,
		        "Seeking did not yield in the anticipated result.\n");
		fprintf(stderr, "Exiting... :-(\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
