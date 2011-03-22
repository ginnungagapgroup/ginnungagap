// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOPK_H
#define COSMOPK_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libcosmo/cosmoPk.h
 * @ingroup libcosmo libcosmoPowerspectrum
 * @brief  This file provides the interface to the power spectrum.
 */


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoModel.h"
#include "cosmoTF.h"
#include "../libutil/parse_ini.h"
#include <stdint.h>


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a power spectrum.
 */
typedef struct cosmoPk_struct *cosmoPk_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * Hallo Welt.
 *
 * @{
 */

/**
 * @brief  Creates a new power spectrum by reading from a file.
 *
 * @param[in]  *fname
 *                Pointer to a NULL terminated string holding the name
 *                of the file that ought to be read in order to
 *                construct the power spectrum.
 *
 * @return  Returns a new power spectrum handle.
 */
extern cosmoPk_t
cosmoPk_newFromFile(const char *fname);


/**
 * @brief  This will generate a new power spectrum according to a model
 *         and a transfer function.
 *
 * @param[in]  model
 *                This should be a proper cosmological model, that
 *                provides all required parameters to compute the power
 *                spectrum via the transfer function.
 * @param[in]  kmin
 *                The lowest frequency from which the interpolation
 *                process starts.
 * @param[in]  kmax
 *                The largest frequency used in to tabulate the power
 *                spectrum.
 * @param[in]  numPoints
 *                The number of points used to tabulate the power
 *                spectrum. This must be at least #LOCAL_MINPOINTS.
 * @param[in]  transferFunctionType
 *                This gives the type of the transfer function that
 *                should be used to generate the power spectrum. See
 *                ::cosmoTF_t for possible options.
 *
 * @return  Returns a new power spectrum handle.
 */
extern cosmoPk_t
cosmoPk_newFromModel(const cosmoModel_t model,
                     double             kmin,
                     double             kmax,
                     uint32_t           numPoints,
                     cosmoTF_t          transferFunctionType);


/**
 * @brief  Creates a new power spectrum by reading a set of key/value
 *         pairs from a configuration file (in ini-Format).
 *
 * @param[in]  ini
 *                This is a handle to an ini-file parser, see
 *                parse_ini.h for more details.
 * @param[in]  *sectionName
 *                A NULL terminated string that gives the section in the
 *                configuration files from which parameters should be
 *                read.
 *
 * @return  Returns a new power spectrum handle.
 */
extern cosmoPk_t
cosmoPk_newFromIni(parse_ini_t ini, const char *sectionName);


/**
 * @brief  Creates a new power spectrum from two arrays (one for the
 *         frequency and one for the power).
 *
 *
 * @param[in] numPoints
 *               The number of points provide for the power spectrum.
 *               This needs to be at least #LOCAL_MINPOINTS.
 * @param[in] *k
 *               An array of at least numPoints entries containing the
 *               frequency values. Note that this array must be ordered
 *               from small to large values.
 * @param[in] *P
 *               An array of at least numPoints entries containing the
 *               power at the according frequency in the k array.
 * @param[in] slopeBeforeKmin
 *                Gives the slope of the power spectrum for k values
 *                lower than the first (usable) tabulated frequency.
 * @param[in] slopeBeyondKmax
 *                As above but the slope for k values larger than the
 *                last (usable) tabulated frequency.
 *
 * @return  Returns a new power spectrum handle.
 */
extern cosmoPk_t
cosmoPk_newFromArrays(uint32_t     numPoints,
                      const double *k,
                      const double *P,
                      double       slopeBeforeKmin,
                      double       slopeBeyondKmax);


/**
 * @brief  Deletes a given power spectrum and frees all associated
 *         memory.
 *
 * @param[in,out]  *pk
 *                    A pointer to the variable that holds the handle of
 *                    the power spectrum that should be deleted. After
 *                    deletion of the power spectrum, the (external)
 *                    handle will be set to NULL to remove access to
 *                    invalid memory regions.
 *
 * @return  Returns nothing.
 */
extern void
cosmoPk_del(cosmoPk_t *pk);


/** @} */

/**
 * @name Getting and Setting
 * @{
 */

/**
 * @brief  Returns the lowest frequency from which on the power spectrum
 *         is usable for interpolation.
 *
 * @param[in]  pk
 *                The handle of the power spectrum that should be queried.
 *
 * @return  Returns a positive non-negative frequency.
 */
extern double
cosmoPk_getKminSecure(const cosmoPk_t pk);


/**
 * @brief  Returns the highest frequency up to which on the power
 *         spectrum is usable for interpolation.
 *
 * @param[in]  pk
 *                The handle of the power spectrum that should be queried.
 *
 * @return  Returns a positive non-negative frequency.
 */
extern double
cosmoPk_getKmaxSecure(const cosmoPk_t pk);


/** @} */

/**
 * @name Output
 * @{
 */

/**
 * @brief  This will dump a power spectrum to a text file.
 *
 * @param[in]  pk
 *                A handle to the power spectrum that should be written
 *                to a text file.
 * @param[in]  *fname
 *                A NULL terminated string that holds the name of the
 *                file to which the power spectrum will be written.  If
 *                the file exists it will be truncated and overwritten.
 * @param[in]  numSubSample
 *                The number of sub-sampling points, i.e. the number of
 *                additional (found by interpolation) points besides the
 *                tabulated frequency values.
 *
 * @return  Returns nothing.
 */
extern void
cosmoPk_dumpToFile(cosmoPk_t pk, const char *fname, uint32_t numSubSample);


/** @} */

/**
 * @name Using
 * @{
 */

/**
 * @brief  Evaluates the power spectrum at a given frequency.
 *
 * The value of P(k) will be found by either an interpolation (k is
 * inside the safe tabulated frequencies) or extrapolation (k is outside
 * the safe tabulate frequencies).  The safe region can be check with
 * cosmoPk_getKminSecure() and cosmoPk_getKmaxSecure().
 *
 * @param[in]  pk
 *                A handle to the power spectrum that be evaluated.
 * @param[in]  k
 *                The frequency at which to evaluate (needs to be
 *                positive, non-zero value).
 *
 * @return  Returns P(k) as found by
 */
extern double
cosmoPk_eval(cosmoPk_t pk, double k);


/**
 * @brief  This is a function prototype that can be used as an GSL
 *         function for evaluating the power spectrum.
 *
 * This is essentially the same thing as cosmoPk_eval(), however the
 * parameters are in reverse order and the handle to the power spectrum
 * is defined as void * to satisfy the needs of GSL functions.
 *
 * @parami[in]  k
 *                The frequency at which to evaluate the power spectrum.
 * @param[in]  *param
 *                This must be the handle to the power spectrum (i.e.
 *                the same as in cosmoPk_eval()).
 *
 * @return  Returns the power spectrum value at k, same restrictions as
 *          for cosmoPk_eval() apply.
 */
extern double
cosmoPk_evalGSL(double k, void *param);


/**
 * @brief  Calculates various moments of the power spectrum.
 *
 * This implements the calculation of the moments of the power spectrum
 * filtered with a window function.  Solved is the following integral
 * (cf. Peacock 2007, 8th edition, eq. (16.22), p. 500):
 *
 * @f[
 *   \sigma_n^2 / V
 *       =    1/(8 \pi^3)
 *            \int_{k_{\mathrm{min}}}^{k_{\mathrm{max}}}
 *                k^{(2n + 2)} P(k) W_k^2(k)
 *             dk
 * @f]
 *
 * @param[in]   pk
 *                 A handle to the power spectrum that should be used.
 * @param[in]   moment
 *                 This gives the moment which should be evaluated (the
 *                 @f$n@f$ the above equation).
 * @param[in]   *windowFunc(double, void *)
 *                 A pointer to a function that should be used as a
 *                 window functions. See cosmoFunc.h for possible
 *                 functions to use here. Note: This must be
 *                 @f$W_k^2(k)@f$ and not @f$W_k(k)@f$.
 * @param[in]   *paramWindowFunc
 *                 A pointer to the parameters the window function needs
 *                 for evaluation.
 * @param[in]   kmin
 *                 This gives the lower limit of the integral.
 * @param[in]   kmax
 *                 This gives the upper limit of the integral.
 * @param[out]  *error
 *                A pointer to an external location that will hold an
 *                estimate of the integration error.
 *
 * @return  Returns something.
 */
extern double
cosmoPk_calcMomentFiltered(cosmoPk_t pk,
                           uint32_t moment,
                           double (*windowFunc
                                   )(double, void *),
                           void *paramWindowFunc,
                           double kmin,
                           double kmax,
                           double *error);


/**
 * @brief  Calculates @f$\sigma_8@f$ and NOT(!!!) @f$\sigma^2_8@f$.
 *         Uses the moments.
 *
 * This is essentially a wrapper for calling
 * cosmoPk_calcMomentFiltered() with cosmoFunc_tophatSqr() as the window
 * function and a scale of 8 for the top hat filter.
 *
 * @param[in]   pk
 *                 A handle to the power spectrum.
 * @param[in]   kmin
 *                 The lower frequency from which to start integrating.
 * @param[in]   kmax
 *                 The upper frequency to which to integrate.
 * @param[out]  *error
 *                 A pointer to an external location that will hold an
 *                 estimate of the integration error.
 *
 * @return  Returns @f$\sigma_8@f$.
 */
extern double
cosmoPk_calcSigma8(cosmoPk_t pk,
                   double    kmin,
                   double    kmax,
                   double    *error);


/** @} */

/**
 * @name Misc
 * @{
 */

/**
 * @brief  This will scale the power spectrum by a factor.
 *
 * @param[in,out]  pk
 *                    A handle to the power spectrum that should be
 *                    scaled.
 * @param[in]      factor
 *                    The factor by which to multiply @f$P(k)@f$.
 *
 * @return  Returns nothing.
 */
extern void
cosmoPk_scale(cosmoPk_t pk, double factor);

extern double
cosmoPk_forceSigma8(cosmoPk_t pk,
                    double    sigma8,
                    double    kmin,
                    double    kmax,
                    double    *error);

extern double
cosmoPk_forceAmplitude(cosmoPk_t pk, double amplitudeAtK, double k);

extern void
cosmoPk_findKWindowForSigma8(cosmoPk_t pk, double *kmin, double *kmax);


/** @} */


#endif
