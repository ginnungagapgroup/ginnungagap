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
#include "generateICsMode.h"
#include "generateICsData.h"
#include "generateICsIn.h"
#include "generateICsOut.h"
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

extern void
generateICs_setMode(generateICs_t genics, generateICsMode_t mode);

extern void
generateICs_setData(generateICs_t genics, generateICsData_t data);

extern void
generateICs_setIn(generateICs_t genics, generateICsIn_t in);

extern void
generateICs_setOut(generateICs_t genics, generateICsOut_t out);


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


/** @} */

/**
 * @name Getting
 * @{
 */

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
