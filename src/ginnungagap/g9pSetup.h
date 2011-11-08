// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PSETUP_H
#define G9PSETUP_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pSetup.h
 * @ingroup  ginnungagapSetup
 * @brief  Provides the interface to the setup routine.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pNorm.h"
#include <stdint.h>
#include <stdbool.h>
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Defines the handle to the setup structure. */
typedef struct g9pSetup_struct *g9pSetup_t;


/*--- Implementation of main structure ----------------------------------*/

/** @brief  The setup structure */
struct g9pSetup_struct {
	/** @brief  The size of the target grid. */
	uint32_t       dim1D;
	/** @brief  Size of the simulation box in Mpc/h. */
	double         boxsizeInMpch;
	/** @brief  Initial redshift. */
	double         zInit;
	/** @brief  The name of the Grid (only relevant for some file types). */
	char           *gridName;
	/** @brief  Gives the employed normalisation mode. */
	g9pNorm_mode_t normalisationMode;
	/** @brief  Selects if 2nd order corrections should be calculated. */
	bool           do2LPTCorrections;
#ifdef WITH_MPI
	/** @brief  The process grid. */
	int nProcs[NDIM];
#endif
	/** @brief  Flags whether the density field should be written. */
	bool     writeDensityField; ///< Defaults to @c true.
	/** @brief  Gives the name of the P(k) of the white noise. */
	char     *namePkWN; ///< Defaults to #local_namePkWn.
	/** @brief  Gives the name of the P(k) of the overdensity field. */
	char     *namePkDeltak; ///< Defaults to #local_namePkDeltak.
	/** @brief  Gives the name of the P(k) of the input P(k). */
	char     *namePkInput; ///< Defaults to #local_namePkInput.
	/** @brief  Gives the name of the P(k) of the input P(k) at z_init. */
	char     *namePkInputZinit; ///< Defaults to #local_namePkInputZinit.
	/** @brief  Gives the name of the P(k) of the input P(k) at z=0. */
	char     *namePkInputZ0; ///< Defaults to #local_namePkInputZ0.
	/** @brief  Flags whether histograms should be calculated. */
	bool     doHistograms; ///< Defaults to @c false.
	/** @brief  The name for the density histogram. */
	char     *nameHistogramDens; ///< Defaults to #local_nameHistoDens;
	/** @brief  The name for the x-velocity histogram. */
	char     *nameHistogramVelx; ///< Defaults to #local_nameHistoVelx.
	/** @brief  The name for the y-velocity histogram. */
	char     *nameHistogramVely; ///< Defaults to #local_nameHistoVely.
	/** @brief  The name for the z-velocity histogram. */
	char     *nameHistogramVelz; ///< Defaults to #local_nameHistoVelz.
	/** @brief  The number of bins of the histograms. */
	uint32_t histogramNumBins;
	/** @brief  The extreme value for the density histogram. */
	double   histogramExtremeDens;
	/** @brief  The extreme value for the velocity histograms. */
	double   histogramExtremeVel;
};


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creates a setup structure from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file to parse.
 *
 * @return  Returns a new setup structure.
 */
extern g9pSetup_t
g9pSetup_new(parse_ini_t ini);


/**
 * @brief  Deletes a setup structure and frees the associated memory.
 *
 * @param[in,out]  *setup
 *                    Pointer to the external variable holding the
 *                    setup.  Will be set to @c NULL after completion of
 *                    the function.
 *
 * @return  Returns nothing.
 */
extern void
g9pSetup_del(g9pSetup_t *setup);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagapSetup  Setup
 * @ingroup ginnungagap
 * @brief Provides the setup of ginnungagap (from an ini file).
 *
 * @section ginnungagapSetupIniFormat  Expected Format of [Ginnungagap]
 *
 * The following block gives a detailed overview of the allowed keys in
 * the main section of the ginnungagap ini file.  Note that
 * g9pSetup_new() parses the section <tt>[Ginnungagap]</tt> only.
 * For an up-to-date description of the actual keys please see the
 * implementation of g9pSetup_new().
 *
 * @code
 * [Ginnungagap]
 * #
 * #################
 * # Required keys #
 * #################
 * #
 * # The dimension of the grid that should be generated.
 * dim1D = <integer>
 * #
 * # The size of the simulation box in Mpc/h.
 * boxsizeInMpch = <positive double>
 * #
 * # The initial redshift.
 * zInit = <double>
 * #
 * # The name of the grid (only relevant for some output formats).
 * gridName = <string>
 * #
 * # The normalisation method that should be used to P(k).
 * normalisationMode = <ASCII name of mode>
 * #
 * #
 * #################
 * # Optional Keys #
 * #################
 * #
 * # This can be used to switch on the calculation of an additional set of
 * # velocity fields which encode the second order corrections to linear
 * # theory.  To calculate this corrections, a significantly larger amount
 * # of memory is required.  If this key is not set, no corrections will be
 * # calculated.
 * do2LPTCorrections = <true|false>
 * #
 * # A tag whether or not to write the density field.  Note: This should
 * # not be disabled for the Grafic writer, as it will then have wrong file
 * # names:  Instead of velx, vely, and velz, the velocity files will have
 * # the names delta, velx, and vely, respectively.
 * writeDensityField = <true|false>
 * #
 * # The name of the text file that will contain the P(k) of the white
 * # noise field.
 * namePkWN = <string>
 * #
 * # The name of the text file that will contain the P(k) of the density
 * # field.
 * namePkDeltak = <string>
 * #
 * # The name of the text file that will contain the input P(k)
 * # (oversampled to show the interpolation result).
 * namePkInput = <string>
 * #
 * # The file that will contain the input P(k) shifted to the initial
 * # redshift, this will also be oversampled.
 * namePkInputZinit = <string>
 * #
 * # The file for the input P(k) at z=0, again oversampled.
 * namePkInputZ0 = <string>
 * #
 * # A flag to switch on/off the calculation of histograms.  The default
 * # is to not calculate any histograms, if it is switched on, the
 * # following three keys are required.
 * doHistograms = <true|false>
 * #
 * # The number of bins used for the histograms.
 * histogramNumBins = <positive integer>
 * #
 * # The extreme value of the density, the histogram will be calculated
 * # from -extreme to extreme.
 * histogramExtremeDens = <positive double>
 * #
 * # The extreme value of the velocity, the histogram will be calculated
 * # from -extreme to extreme.
 * histogramExtremeVel = <positive double>
 * #
 * # The name of the text file that will contain the density histogram.
 * nameHistogramDens = <string>
 * #
 * # The name of the text file that will contain the histogram of the
 * # x-component of the velocity.
 * nameHistogramVelx = <string>
 * #
 * # The name of the text file that will contain the histogram of the
 * # y-component of the velocity.
 * nameHistogramVely = <string>
 * #
 * # The name of the text file that will contain the histogram of the
 * # z-component of the velocity.
 * nameHistogramVelz = <string>
 * #
 * @endcode
 *
 *
 * @section ginnungagapSetupIniFormatMPI  Expected Format of [MPI]
 *
 * Should MPI be enabled in the code, another section is evaluated:
 *
 * @code
 * [MPI]
 * #
 * #################
 * # Required keys #
 * #################
 * #
 * # This gives the processor grid employed in the domain decomposition,
 * # it depends on the dimensionality of the code how many integers are
 * # expected.  The first number must be 1 (the x dimension is not
 * # partitioned) and the second (and third) may be any integer,
 * # provided that it is compatible with the total number of tasks used.
 * # It is possible to use 0 for the second (and/or third) value, in
 * # which case the partitioning is done automatically to achieve
 * # domains that are 'as square as possible'.  Using nProcs = 1 1 0
 * # effectively forces a slab decomposition.
 * nProcs = <2 or 3 integers>
 * #
 * @endcode
 */


#endif
