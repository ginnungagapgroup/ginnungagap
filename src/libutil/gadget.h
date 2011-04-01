// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGET_H
#define GADGET_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadget.h
 * @ingroup libutilFilesGadget
 * @brief  This file provides the interface to Gadget (type 1 and 2)
 *         files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include "gadgetHeader.h"


/*--- Typedefs ----------------------------------------------------------*/
typedef enum {
	GADGET_MODE_WRITE,
	GADGET_MODE_READ
} gadgetMode_t;

/*--- ADT handle --------------------------------------------------------*/
typedef struct gadget_struct *gadget_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gadget_t
gadget_new(const char *fileNameStem, int numFiles);

extern void
gadget_del(gadget_t *gadget);

extern int
gadget_getNumFiles(gadget_t gadget);

extern void
gadget_attachHeader(gadget_t gadget, gadgetHeader_t header, int numFile);

extern void
gadget_open(gadget_t gadget, gadgetMode_t mode, int numFile);

extern void
gadget_close(gadget_t gadget, int numFile);

extern void
gadget_write(gadget_t gadget,
             int      numFile,
             float    *pos,
             float    *vel,
             uint32_t *id);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilFilesGadget Gadget
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to Gadget files.
 *
 */


#endif
