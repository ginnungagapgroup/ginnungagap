// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDSILOTOOLS_H
#define GRIDSILOTOOLS_H
#ifdef WITH_SILO


/*--- Includes ----------------------------------------------------------*/
#  include "grid_config.h"


/*--- Prototypes of exported functions ----------------------------------*/
extern char *
gridSiloTools_getSiloFileName(char       *siloFileName,
                              size_t     *n,
                              const char *siloBaseName,
                              int        rank);

extern char *
gridSiloTools_getSiloMasterFileName(char       *siloFileName,
                                    size_t     *n,
                                    const char *siloBaseName);

extern char *
gridSiloTools_getSiloMeshName(char       *siloMeshName,
                              size_t     *n,
                              const char *meshName,
                              int        rank);

extern char *
gridSiloTools_getSiloVarName(char       *siloVarName,
                             size_t     *n,
                             const char *varName,
                             int        rank);

#endif
#endif
