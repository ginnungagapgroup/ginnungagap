// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOMODEL_H
#define COSMOMODEL_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libcosmo/cosmoModel.h
 * @ingroup  libcosmoModel 
 * @brief  Provides the interface to the cosmological model object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct cosmoModel_struct *cosmoModel_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern cosmoModel_t
cosmoModel_new(void);

extern cosmoModel_t
cosmoModel_newFromFile(const char *fname);

/**
 * @brief  Creates a new cosmological model from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file to parse.  See
 *                    @ref libcosmoModelIniFormat for the format of this
 *                    file.
 * @param[in]      *sectionName
 *                    The name of the section in which to look for the
 *                    construction information.
 *
 * @return  Returns a new model with the value initialised according to
 *          the information in the ini file.
 */
extern cosmoModel_t
cosmoModel_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
cosmoModel_del(cosmoModel_t *model);

extern double
cosmoModel_getOmegaRad0(const cosmoModel_t model);

extern double
cosmoModel_getOmegaLambda0(const cosmoModel_t model);

extern double
cosmoModel_getOmegaMatter0(const cosmoModel_t model);

extern double
cosmoModel_getOmegaBaryon0(const cosmoModel_t model);

extern double
cosmoModel_getSmallH(const cosmoModel_t model);

extern double
cosmoModel_getSigma8(const cosmoModel_t model);

extern double
cosmoModel_getNs(const cosmoModel_t model);

extern double
cosmoModel_getTempCMB(const cosmoModel_t model);

extern void
cosmoModel_setOmegaRad0(cosmoModel_t model, const double omegaRad0);

extern void
cosmoModel_setOmegaLambda0(cosmoModel_t model, const double omegaLambda0);

extern void
cosmoModel_setOmegaMatter0(cosmoModel_t model, const double omegaMatter0);

extern void
cosmoModel_setOmegaBaryon0(cosmoModel_t model, const double omegaBaryon0);

extern void
cosmoModel_setSmallH(cosmoModel_t model, const double hubble);

extern void
cosmoModel_setSigma8(cosmoModel_t model, const double sigma8);

extern void
cosmoModel_setNs(cosmoModel_t model, const double ns);

extern void
cosmoModel_setTempCMB(cosmoModel_t model, const double tempCMB);

extern double
cosmoModel_calcAgeFromExpansion(cosmoModel_t model,
                                double       a,
                                double       *error);

extern double
cosmoModel_calcExpansionFromAge(cosmoModel_t model,
                                double       t,
                                double       *error);

extern double
cosmoModel_calcADot(cosmoModel_t model, double a);

extern double
cosmoModel_calcHubble(cosmoModel_t model, double a);

extern double
cosmoModel_calcOmegaRad(cosmoModel_t model, double a);

extern double
cosmoModel_calcOmegaMatter(cosmoModel_t model, double a);

extern double
cosmoModel_calcOmegaBaryon(cosmoModel_t model, double a);

extern double
cosmoModel_calcOmegaLambda(cosmoModel_t model, double a);

extern double
cosmoModel_calcGrowth(cosmoModel_t model, double a, double *error);

extern double
cosmoModel_calcDlnGrowthDlna(cosmoModel_t model, double a, double *error);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libcosmoModel Cosmological Models
 * @ingroup libcosmo
 * @brief Provides cosmological models.
 *
 * @section libcosmoModelIniFormat  Ini Format for Cosmological Models
 *
 * The following block describes the ini file structure as expected by
 * cosmoModel_newFromIni().  Essentially, all key have to be set to
 * sensible values, but for some the code will substitute defaults if
 * they are not given, see #LOCAL_DEFAULT_OMEGARAD0,
 * #LOCAL_DEFAULT_OMEGABARYON0, #LOCAL_DEFAULT_NS and
 * #LOCAL_DEFAULT_TEMPCMB for their values.
 *
 * @code
 * [SectionName]
 * #
 * #################
 * # Required keys #
 * #################
 * #
 * # The value of the energy density of the cosmological constant at z=0
 * # in units of the critical density.
 * modelOmegaLambda0 = <double>
 * # The value of the matter density at z=0 in units of the critical
 * # density.
 * modelOmegaMatter0 = <double>
 * # The value of the Hubble parameter in units of 100 km/s/Mpc.
 * modelHubble = <double>
 * # The value of sigma8 for the given model (at z=0).
 * modelSigma8 = <double>
 * #
 * #
 * #################
 * # Optional Keys #
 * #################
 * # 
 * # The value of the energy density of the radiation component at z=0
 * # in units of the critical density.  A default value is used if this
 * # parameter is not given.
 * modelOmegaRad0 = <double>
 * # The value of the baryonic matter density at z=0 in units of the
 * # critical density.  A default value is used if this parameter is not
 * # given.
 * modelOmegaBaryon0 = <double>
 * # The slope of the primordial power spectrum.  A default parameter is
 * # used, if this parameter is not given.
 * modelNs = <double>
 * # The temperature of the CMB in K.  If not given, a default value
 * # will be used.
 * modelTempCMB = <double>
 * #
 * @endcode
 */


#endif
