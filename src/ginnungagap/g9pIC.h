// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PIC_H
#define G9PIC_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pIC.h
 * @ingroup  ginnungagapIC
 * @brief  Provides the interface to IC routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdint.h>
#include "../libgrid/gridRegularFFT.h"
#include "../libcosmo/cosmoPk.h"
#include "../libcosmo/cosmoModel.h"


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  Flags which velocity component should be calculated. */
typedef enum {
	/** @brief  Do the x-component of the velocity. */
	G9PIC_MODE_VX,
	/** @brief  Do the y-component of the velocity. */
	G9PIC_MODE_VY,
	/** @brief  Do the z-component of the velocity. */
	G9PIC_MODE_VZ,
	/** @brief  Do the x-component of the large scale velocity. */
	G9PIC_MODE_LVX,
	/** @brief  Do the y-component of the large scale velocity. */
	G9PIC_MODE_LVY,
	/** @brief  Do the z-component of the large scale velocity. */
	G9PIC_MODE_LVZ,
	/** @brief  Do the x-component of the small scale velocity. */
	G9PIC_MODE_SVX,
	/** @brief  Do the y-component of the small scale velocity. */
	G9PIC_MODE_SVY,
	/** @brief  Do the z-component of the small scale velocity. */
	G9PIC_MODE_SVZ
} g9pICMode_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Calculates the overdensity field in Fourier space from a
 *         white noise field in Fourier space and a power spectrum.
 *
 * This essential only calculates
 * @f[
 *    \hat{\delta}(k_0,k_1,k_2) = \hat{\xi}(k_0,k_1,k_2) \times
 *    \sqrt{P(k)}
 * @f]
 * where @f$ \hat{\delta} @f$ is the overdensity field in Fourier space,
 * @f$ \hat{\xi} @f$ is the white noise field in Fourier space,
 * @f$ k_0,k_1,k_2 @f$ are the wave numbers in the three dimensions and
 * @f$ k = 2 \pi \sqrt{k_0^2 + k_1^2 + k_2^2} / L @f$ is the frequency
 * of the wave (@f$ L @f$ gives the size of the box in Mpc/h).  Note
 * that the wave numbers are in the range
 * @f$ [ -k_{\mbox{max}} + 1,  k_{\mbox{max}} ]@f$.
 *
 * As the fields in Fourier space are generated from a real-to-complex
 * transform, in one dimension only the positive wave numbers are stored
 * and represented (the requirement that the back-transform yields a
 * real field allows for this saving).  While @f$ k_{\mbox{max}}@f$ is
 * given by @f$ k_{\mbox{max}} = d/2 @f$, where @f$ d @f$ is the
 * dimension of the grid (in real space, passed as @c dim1D), the
 * dimension corresponding to the real-to-complex transform has
 * @f$ k_{\mbox{max}} = d/2 + 1 @f$.
 *
 * The function forces the AC mode (@f$k_i = 0 \quad \forall i@f$) to be
 * zero, i.e. @f$ P(0) \equiv 0 @f$.
 *
 * @bug The function currently assumes that the dimension corresponding
 *      to the real-to-complex transform is @f$k_1@f$, i.e. the array
 *      has been permuted in such a way that it is now in the order
 *      @f$ z, x, y @f$.  This is true only if the MPI transform has
 *      been executed, a plain OpenMP transform most likely has a
 *      different permutation of the original dimensions.
 *
 * @param[in,out]  gridFFT
 *                    The interface to the FFT'ed grid.  The underlying
 *                    grid must be in Fourier space and contain the
 *                    white noise field in Fourier space as the first
 *                    variable.  Passing @c NULL is undefined.
 * @param[in]      dim1D
 *                    The dimension of the grid.  This is needed to
 *                    calculate the largest represented frequencies.
 * @param[in]      boxsizeInMpch
 *                    The size @f$ L @f$ of the box in Mpc/h.  Required
 *                    to translate wave numbers to proper frequencies.
 *                    Note that @f$ f = 2 \pi k / L @f$.
 * @param[in]      pk
 *                    The power spectrum.
 *
 * @return  Returns nothing.
 */
extern void
g9pIC_calcDeltaFromWN(gridRegularFFT_t gridFFT,
                      uint32_t         dim1D,
                      double           boxsizeInMpch,
                      cosmoPk_t        pk);


/**
 * @brief  Calculates a velocity component from the overdensity in
 *         Fourer space.
 *
 * @f[
 *    \hat{v}_i = \frac{\hat{\delta} \times k_i (2\pi/L)}
 *                     {(k_0^2 + k_1^2 + k_2^2) \times (2\pi/L)^2}
 *                \times \mbox{norm}
 * @f]
 * with
 * @f[
 *    \mbox{norm} = \dot{a} H_0 \frac{\mbox{d} \ln G}{ \mbox{d} \ln a}
 * @f]
 * see cosmoModel_calcADot() and cosmoModel_calcDlnGrowthDlna() for more
 * details on the calculations of the first and last factor in the last
 * equation.
 *
 * @bug This is only valid in MPI mode, where the grid is permuted in
 *      the order @f$ z, x, y @f$.
 *
 * @param[in,out]  gridFFT
 *                    The interface to the FFT'ed grid.  The underlying
 *                    grid must be in Fourier space and contain the
 *                    overdensity field in Fourier space as the first
 *                    variable.  Passing @c NULL is undefined.
 * @param[in]      dim1D
 *                    The dimension of the grid.  This is needed to
 *                    calculate the largest represented frequencies.
 * @param[in]      boxsizeInMpch
 *                    The size @f$ L @f$ of the box in Mpc/h.  Required
 *                    to translate wave numbers to proper frequencies.
 *                    Note that @f$ f = 2 \pi k / L @f$.
 * @param[in]      model
 *                    The cosmological model (required to get the
 *                    scaling factors).
 * @param[in]      aInit
 *                    The expansion factor at which to generate the
 *                    velocity.
 * @param[in]      cutoffScale
 *                    The scale of large or small scale cutoff. Which cutoff
 *                    is done is determined by @c mode parameter.
 * @param[in]      mode
 *                    Selects which velocity component should be
 *                    calculated.
 *
 * @return  Returns nothing.
 */
extern void
g9pIC_calcVelFromDelta(gridRegularFFT_t gridFFT,
                       uint32_t         dim1D,
                       double           boxsizeInMpch,
                       cosmoModel_t     model,
                       double           aInit,
                       double 			cutoffScale,
                       g9pICMode_t      mode);


/**
 * @brief  Calculates the second derivative of the linear potential.
 *
 * @param[in,out]  gridFFT
 *                    The interface to the FFT'ed grid.  The underyling grid
 *                    must be in Fourier space and contain the overdensity
 *                    field in Fourier space as the first variable.  Passing
 *                    @c NULL is undefined.
 * @param[in]      dim1D
 *                    The dimension of the grid.  Needed for the Nyquist
 *                    frequency calculation.
 * @param[in]      d1
 *                    The direction of the first derivative.  Must be
 *                    0, 1, or 2 (i.e. x, y, or z direction) w.r.t to the
 *                    real space field.
 * @param[in]      d2
 *                    The direction of the second derivative.  Must be
 *                    0, 1, or 2 (i.e. x, y, or z direction) w.r.t to the
 *                    real space field.
 *
 * @return  Returns nothing.
 */
extern void
g9pIC_calcDDPhiFromDelta(gridRegularFFT_t gridFFT,
                         uint32_t         dim1D,
                         uint32_t         d1,
                         uint32_t         d2);

/**
 * @brief  Calculates the power spectrum of the overdensity field.
 *
 * @param[in]  gridFFT
 *                    The interface to the FFT'ed grid.  The underlying
 *                    grid must be in Fourier space and contain the
 *                    overdensity field in Fourier space as the first
 *                    variable.  Passing @c NULL is undefined.
 * @param[in]  dim1D
 *                    The dimension of the grid.  This is needed to
 *                    calculate the largest represented frequencies.
 * @param[in]  boxsizeInMpch
 *                    The size @f$ L @f$ of the box in Mpc/h.  Required
 *                    to translate wave numbers to proper frequencies.
 *                    Note that @f$ f = 2 \pi k / L @f$.
 *
 * @return  Returns a new power spectrum object that contains the
 *          derived power spectrum of the provided overdensity field.
 */
extern cosmoPk_t
g9pIC_calcPkFromDelta(gridRegularFFT_t gridFFT,
                      uint32_t         dim1D,
                      double           boxsizeInMpch);


/**
 * @brief  Translates a mode to a string.
 *
 * @param[in]  mode
 *                The mode to get the name for.
 *
 * @return  Returns the string describing the provided mode.  This is a
 *          private memory region and thus may be used read-only.
 */
extern const char *
g9pIC_getModeStr(g9pICMode_t mode);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagapIC  Calculation of Initial Conditions
 * @ingroup ginnungagap
 * @brief Provides the actual IC calculations.
 */


#endif
