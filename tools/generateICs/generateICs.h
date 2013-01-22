// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GENERATEICS_H
#define GENERATEICS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICs.h
 * @ingroup  toolsGICSApp
 * @brief  Provides the interface to the generateICs tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include <stdio.h>
#include "../../src/libcosmo/cosmoModel.h"
#include "../../src/libg9p/g9pHierarchy.h"
#include "../../src/libg9p/g9pDataStore.h"
#include "../../src/libg9p/g9pMask.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Provides a handle for the generateICs application. */
typedef struct generateICs_struct *generateICs_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 * @{
 */

/**
 * @brief  Creates an empty application.
 *
 * @return  Returns a handle to an empty generateICs application.
 */
extern generateICs_t
generateICs_new(void);


/**
 * @brief  Deletes a generateICs application and frees the
 *         associated memory.
 *
 * @param[in,out]  *genics
 *                     The realSpaceApplication that should be deleted.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_del(generateICs_t *genics);


/** @} */

/**
 * @name Setting
 * @{
 */

/**
 * @brief  Sets a cosmological model.
 *
 * If a model is already attached, it will be replaced with the new one.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      model
 *                    The model that should be set.  The caller relinquishes
 *                    control of the object. Passing @c NULL is allowed.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setCosmoModel(generateICs_t genics,
                          cosmoModel_t  model);


/**
 * @brief  Sets the hierarchy to use.
 *
 * If a hierarchy is already attached, the execution will fail.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      hierarchy
 *                    The hierarchy that should be attached.  The caller
 *                    relinquishes control of the object. Passing @c NULL is
 *                    allowed.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setHierarchy(generateICs_t  genics,
                         g9pHierarchy_t hierarchy);

/**
 * @brief  Sets the datastore to use.
 *
 * If a datastore is already attached, the execution will fail.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      datastore
 *                    The datastore that should be attached.  The caller
 *                    relinquishes control of the object. Passing @c NULL is
 *                    allowed.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setDataStore(generateICs_t  genics,
                         g9pDataStore_t datastore);

/**
 * @brief  Sets the mask to use.
 *
 * If a mask is already attached, the execution will fail.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      mask
 *                    The mask that should be attached.  The caller
 *                    relinquishes control of the object. Passing @c NULL is
 *                    allowed.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setMask(generateICs_t genics,
                    g9pMask_t     mask);

/**
 * @brief  Sets the boxsize in Mpc/h.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      boxsizeInMpch
 *                    The boxsize in Mpc/h that should be set. Must be
 *                    larger than 0.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setBoxsizeInMpch(generateICs_t genics, double boxsizeInMpch);


/**
 * @brief  Sets the initial expansion factor.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      aInit
 *                    The initial expansion factor.  Must be larger than 0.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setAInit(generateICs_t genics, double aInit);


/**
 * @brief  Sets the flag indicating the usage of gas particles.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      doGas
 *                    If @c false, then no gas particles will be created, if
 *                    @c true, then gas particles will be created.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setDoGas(generateICs_t genics, bool doGas);


/**
 * @brief  Sets the flag indicating the usage of long ids.
 *
 * This can be used to overwrite the implicit choice of ID size done
 * internally, the code will figure out for how many particles it needs to
 * generate IDs and use 32bit IDs if that is sufficient, otherwise 64bit
 * integers will be used.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      doLongIDs
 *                    If @c true then the IDs will be generated as 64bit
 *                    integers, otherwise 32bit integers are used.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setDoLongIDs(generateICs_t genics, bool doGas);


/**
 * @brief  Sets the number of files the output is split over.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      numFiles
 *                    The number of files, must be larger than 0.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setNumFiles(generateICs_t genics, uint32_t numFiles);


/**
 * @brief  Sets prefix of the output files.
 *
 * @param[in,out]  genics
 *                    The application object to work with.  Passing @c NULL
 *                    is undefined.
 * @param[in]      *prefix
 *                    The initial prefix of the output file.  Must be a
 *                    valid filename.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_setPrefix(generateICs_t genics, char *prefix);


/** @} */

/**
 * @name Getting
 * @{
 */

/**
 * @brief  Retrieves the cosmological model.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns the cosmological model used.  This might be @c NULL if
 *          no model has been previously set.  The application keeps control
 *          of the model object, the calling function should hence take care
 *          with the retrieved handle.
 */
extern cosmoModel_t
generateICs_getCosmoModel(const generateICs_t genics);


/**
 * @brief  Retrieves the hierarchy.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns the hierarchy used.  This might be @c NULL if no
 *          hierarchy has been previously set.  The application keeps
 *          control of the hierarchy object, the calling function should
 *          hence take care with the retrieved handle.
 */
extern g9pHierarchy_t
generateICs_getHierarchy(const generateICs_t genics);

/**
 * @brief  Retrieves the datastore.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns the datastore used.  This might be @c NULL if no
 *          datastore has been previously set.  The application keeps
 *          control of the datastore object, the calling function should
 *          hence take care with the retrieved handle.
 */
extern g9pDataStore_t
generateICs_getDataStore(const generateICs_t genics);


/**
 * @brief  Retrieves the mask.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns the mask used.  This might be @c NULL if no mask has
 *          been previously set.  The application keeps control of the mask
 *          object, the calling function should hence take care with the
 *          retrieved handle.
 */
extern g9pMask_t
generateICs_getMask(const generateICs_t genics);


/**
 * @brief  Retrieves the boxsize in Mpc/h.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns the boxsize in Mpc/h.
 */
extern double
generateICs_getBoxsizeInMpch(const generateICs_t genics);


/**
 * @brief  Retrieves the usage of gas particles.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns @c true if gas particles are used, @c false otherwise..
 */
extern bool
generateICs_getDoGas(const generateICs_t genics);


/**
 * @brief  Retrieves the usage of long IDs.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns @c true if the IDs are 64bit and @c false if they are
 *          32bit.
 */
extern bool
generateICs_getDoLongIDs(const generateICs_t genics);


/**
 * @brief  Retrieves the initial expansion factor.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns the initial expansion factor.
 */
extern double
generateICs_getAInit(const generateICs_t genics);


/**
 * @brief  Retrieves the number of output files.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns the number of output files.
 */
extern uint32_t
generateICs_getNumFiles(const generateICs_t genics);


/**
 * @brief  Retrieves the prefix of the output files.
 *
 * @param[in]  genics
 *                The application object to query.
 *
 * @return  Returns the prefix of the output files.
 */
extern const char *
generateICs_getPrefix(const generateICs_t genics);


/** @} */

/**
 * @name Using
 * @{
 */

/**
 * @brief  Prints a short summary of what the code does to a stream.
 *
 * @param[in]      genics
 *                    The application object to use.  Must be a valid
 *                    application object, passing @c NULL is undefined.
 * @param[in,out]  *out
 *                    The stream to which to write. Must not be a valid
 *                    stream that can be written to, passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_printSummary(const generateICs_t genics, FILE *out);


/**
 * @brief  Executes the application.
 *
 * @param[in,out]  genics
 *                    The generateICs application to execute.  Must be a
 *                    valid application object, passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
extern void
generateICs_run(generateICs_t genics);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGICSApp generateICs
 * @ingroup  toolsGICS
 * @brief  Provides the generateICs tool.
 *
 * Write documentation here.
 */


#endif
