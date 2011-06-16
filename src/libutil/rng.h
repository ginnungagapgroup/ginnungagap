// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef RNG_H
#define RNG_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/rng.h
 * @ingroup libutilMisc
 * @brief  This file provides the interface of the RNG.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief Gives the type of the data stored.
 */
typedef struct rng_struct *rng_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new generator interface.
 *
 * @param[in]  generatorType
 *                The type of the generator.
 * @param[in]  numStreamsTotal
 *                The total number of streams.
 * @param[in]  randomSeed
 *                The seed to initialize the streams.
 *
 * @return  Returns a new generator object.
 */
extern rng_t
rng_new(int generatorType, int numStreamsTotal, int randomSeed);

/**
 * @brief  Creates a new random generator from information in an ini
 *         file.
 *
 * @param[in,out]  ini
 *                    The ini object to use.
 * @param[in]      *sectionName
 *                    The section within the ini file to use.
 *
 * @return  Returns a new generator object.
 */
extern rng_t
rng_newFromIni(parse_ini_t ini, const char *sectionName);

/**
 * @brief  Deletes a generator object and frees the associated memory.
 *
 * @param[in,out]  *bov
 *                    A pointer to the variable holding the handle of
 *                    the generator object.  After deletion, the
 *                    external variable will be set to NULL.
 *
 * @return  Returns nothing.
 */
extern void
rng_del(rng_t *rng);


/** @} */

/**
 * @name Using
 *
 * @{
 */

/**
 * @brief  Resets the generator into the initial state.
 *
 * This works like calling rng_del() and then rng_new() with the same
 * parameters as the original generator, except that generator object is
 * re-used, saving the memory (de)allocation calls.
 *
 * @param[in,out]  rng
 *                    The generator object to reset.
 *
 * @return  Returns nothing.
 */
extern void
rng_reset(rng_t rng);


/**
 * @brief  Retrieves the number of streams locally used (which might be
 *         different from the total number for parallel applications).
 *
 * @param[in]  rng
 *                The generator object to query.
 *
 * @return  Returns the number of streams locally available to the
 *          generator.
 */
extern int
rng_getNumStreamsLocal(const rng_t rng);


/**
 * @brief  Generates a Gaussian random number with the given mean and
 *         sigma.
 *
 * @param[in]  rng
 *                The random generator object to use.
 * @param[in]  streamNumber
 *                The stream number to use.
 * @param[in]  mean
 *                The required mean of the distribution.
 * @param[in]  sigma
 *                The required sigma of the distribution.
 *
 * @return  A Gaussian distributed random number.
 */
extern double
rng_getGauss(const rng_t  rng,
             const int    streamNumber,
             const double mean,
             const double sigma);


/**
 * @brief  Generates a Gaussian random number with zero mean and unit
 *         variance.
 *
 * This is like calling rng_getGauss() with 0.0 and 1.0 for mean and
 * sigma, respectively.
 */
extern double
rng_getGaussUnit(const rng_t rng, const int streamNumber);


/** @} */

#endif
