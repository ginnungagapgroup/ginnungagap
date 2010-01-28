// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAP_H
#define GINNUNGAGAP_H


typedef struct ginnungagap_struct *ginnungagap_t;

extern ginnungagap_t
ginnungagap_new(const char *iniFname, int flags);

extern void
ginnungagap_run(ginnungagap_t ginnungagap);

extern void
ginnungagap_del(ginnungagap_t *ginnungagap);


#endif
