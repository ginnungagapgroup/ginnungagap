// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETHEADER_H
#define GADGETHEADER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetHeader.h
 * @ingroup libutilFilesGadget
 * @brief  This file provides the interface to Gadget headers.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>
#include <stdint.h>


/*--- Exported defines --------------------------------------------------*/

/** @brief Gives the size of the header in bytes. */
#define GADGETHEADER_SIZE 256

/** @brief Gives the number of bytes used for data in the header. */
#define GADGETHEADER_SIZE_USED 208

/** @brief Gives the number of unused bytes in the header. */
#define GADGETHEADER_SIZE_UNUSED (256 - 208)


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a Gadget header object.
 */
typedef struct gadgetHeader_struct *gadgetHeader_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  This creates a new (and empty) Gadget header object.
 *
 * @return  Returns the handle to an empty Gadget header object.
 */
extern gadgetHeader_t
gadgetHeader_new(void);


/**
 * @brief  Makes a deep copy of a Gadget header object.
 *
 * @param[in]  gadgetHeader
 *                The header that should be cloned.
 *
 * @return  Returns a new Gadget header object that has the same state
 *          as the input header.
 */
extern gadgetHeader_t
gadgetHeader_clone(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Deletes a Gadget header object and frees the associated
 *         memory.
 *
 * @param[in,out]  *gadgetHeader
 *                    Pointer to the variable that holds the reference
 *                    to the header object that should be deleted.  Note
 *                    that after deletion the external variable will be
 *                    set to NULL.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_del(gadgetHeader_t *gadgetHeader);


/** @} */

/**
 * @name Setting
 *
 * @{
 */

/**
 * @brief  Sets the number of particles for all types.
 *
 * This sets the number of particles in the file the header is
 * associated with.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      np
 *                    The number of particles for each type.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setNp(gadgetHeader_t gadgetHeader, const uint32_t np[6]);


/**
 * @brief  Sets the masses for all particle types.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      massarr
 *                    The masses of the particle types; this must be
 *                    either a postive number or zero, negative values
 *                    are not allowed.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setMassArr(gadgetHeader_t gadgetHeader,
                        double const   massarr[6]);


/**
 * @brief  Sets the time of the snapshot the header is associated with.
 *
 * Note that this corresponds to the expansion factor for cosmological
 * simulations.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      time
 *                    The time to set the header to.
 *
 * @return  Returns nothing.
 *
 * @sa gadgetHeader_setRedshift();
 */
extern void
gadgetHeader_setTime(gadgetHeader_t gadgetHeader, double time);


/**
 * @brief  Sets the redshift of the snapshot the associated with.
 *
 * This should be set according to the time for cosmological
 * simulations.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      redshift
 *                    The redshift to set the header to.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setRedshift(gadgetHeader_t gadgetHeader, double redshift);


/**
 * @brief  Sets the flag whether the simulation used star formation.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      flag
 *                    This should be 0 for no star formation and 1 for
 *                    with star formation.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setFlagSfr(gadgetHeader_t gadgetHeader, int32_t flag);


/**
 * @brief  Sets the flag whether the simulation used feedback.
 *
 * This is marked as obsolete in the Gadget sources.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      flag
 *                    Any number is permitted.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setFlagFeedback(gadgetHeader_t gadgetHeader, int32_t flag);


/**
 * @brief  Sets the total number of particles in the snapshot.
 *
 * This really sets the total number of particles.  Note that the Gadget
 * header for historical reasons stores the number of particles as two
 * separate 32bit integers: the low word part and the high word part of
 * the 64bit total number of particles.  This functions will deal with
 * this and accept 64bit values which will be split accordingly and
 * assigned to the two storage locations.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      nall
 *                    The number of particles for each type.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setNall(gadgetHeader_t gadgetHeader, const uint64_t nall[6]);


/**
 * @brief  Sets whether the simulations used cooling.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      flag
 *                    This should be either 0 (without cooling) or 1
 *                    (with cooling).
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setFlagCooling(gadgetHeader_t gadgetHeader, int32_t flag);


/**
 * @brief  Sets the number of files the snapshot is distributed over.
 *
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      numFiles
 *                    The number of files the snapshot is distributed
 *                    over.  This must be a non-negative positive
 *                    value.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setNumFiles(gadgetHeader_t gadgetHeader, int32_t numFiles);


/**
 * @brief  Sets the size of the simulation box (for cosmological
 *         simulations).
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      boxsize
 *                    The size of the box, must be greater than or equal
 *                    to 0.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setBoxsize(gadgetHeader_t gadgetHeader, double boxsize);


/**
 * @brief  Sets matter density of the simulation.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      omega0
 *                    The matter density at @$ z = 0.0 @$ in units of
 *                    the critical density.  Must be greater than or
 *                    equal to 0.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setOmega0(gadgetHeader_t gadgetHeader, double omega0);


/**
 * @brief  Sets density parameter of the cosmological constant.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      omegaLambda
 *                    The density parameter of the cosmological constant
 *                    at @$ z = 0.0 @$ in units of the critical density.
 *                    Must be greater than or equal to 0.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setOmegaLambda(gadgetHeader_t gadgetHeader, double omegaLambda);


/**
 * @brief  Sets the Hubble parameter of the simulation.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      h
 *                    The Hubble parameter in units of 100 km/s/Mpc.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setHubbleParameter(gadgetHeader_t gadgetHeader, double h);


/**
 * @brief  Sets the flag indicating whether the simulation stores the
 *         formation times of the star particles.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      flag
 *                    Should be either 0 (no formation times) or 1 (with
 *                    formation times).
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setFlagStellarAge(gadgetHeader_t gadgetHeader, int32_t flag);


/**
 * @brief  Sets the flag indicating whether the metallicities of star
 *         and gas particles are stored.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      flag
 *                    The flag, should be either 0 (no metallicity) or 1
 *                    (with metallicity).
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setFlagMetal(gadgetHeader_t gadgetHeader, int32_t flag);


/**
 * @brief  Sets the flag indicating whether the file stores entropy
 *         instead of internal energy.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      flag
 *                    The flag, should be either 0 (storing internal
 *                    energy) or 1 (storing entropy).
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setFlagEntropy(gadgetHeader_t gadgetHeader, int32_t flag);


/**
 * @brief  Sets the flag indicating whether the file uses single
 *         precision or double precision.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      flag
 *                    The flag, should be either 0 (no metallicity) or 1
 *                    (with metallicity).
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setFlagDoublePrecision(gadgetHeader_t gadgetHeader,
                                    int32_t        flag);


/**
 * @brief  Sets the flag indicating what type of initial conditions are
 *         used.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      flag
 *                    The flag, should be 0 unless you know what other
 *                    values mean (see Gadget sources).
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setFlagICInfo(gadgetHeader_t gadgetHeader, int32_t flag);


/**
 * @brief  Sets the scaling factor for Lagrangian Perturbation Theory
 *         initial conditions.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object to modify.
 * @param[in]      lptScalingFactor
 *                    The scaling factor.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_setLPTScalingFactor(gadgetHeader_t gadgetHeader,
                                 float          lptScalingFactor);


/** @} */

/**
 * @name Getting
 *
 * @{
 */

/**
 * @brief  Gets the number of particles in a given file associated with
 *         the header object.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 * @param[out] np
 *                Array that will be filled with the number of
 *                particles.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_getNp(const gadgetHeader_t gadgetHeader, uint32_t np[6]);


/**
 * @brief  Gets the masses of the particle types.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 * @param[out] massarr
 *                Array that will be filled with the masses of the
 *                particle types.
 *
 * @return
 */
extern void
gadgetHeader_getMassArr(const gadgetHeader_t gadgetHeader,
                        double               massarr[6]);


/**
 * @brief  Queries the time value stored in the header object.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the time stored in the header.
 */
extern double
gadgetHeader_getTime(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries the redshift value stored in the header object.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return Returns the time redshift stored in the header.
 */
extern double
gadgetHeader_getRedshift(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries whether star formation was used.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the flag stored in the header, this should be 0 if
 *          star formation was not used and something else otherwise.
 */
extern int32_t
gadgetHeader_getFlagSfr(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries whether feedback was used (this deprecated).
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the flag stored in the header, this should be 0 if
 *          star formation was not used and something else otherwise.
 */
extern int32_t
gadgetHeader_getFlagFeedback(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Retrieves the total number of particles.
 *
 * This function accesses two fields of the header.  Historically, the
 * total number of particles is stored separate for the leading and
 * trailing 32bit of the 64bit number.  This function combines both
 * field to a proper 64bit integer and returns the result in the
 * provided array.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 * @param[out] nall
 *                The array that will reveice the total number of
 *                particles of each particle type.
 *
 * @return   Returns nothing.
 */
extern void
gadgetHeader_getNall(const gadgetHeader_t gadgetHeader, uint64_t nall[6]);


/**
 * @brief  Queries whether cooling was used.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the flag stored in the header, this should be 0 if
 *          cooling was not used and something else otherwise.
 */
extern int32_t
gadgetHeader_getFlagCooling(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries for the number of files the snapshot is distributed
 *         over.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the number of files, this ought to be a non-negative
 *          positive value.
 */
extern int32_t
gadgetHeader_getNumFiles(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Retrieves the box size.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the box size in internal units.
 */
extern double
gadgetHeader_getBoxsize(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Retrieves the matter density.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the matter density at @$ z = 0.0 @$ in units of the
 *          critical density.
 */
extern double
gadgetHeader_getOmega0(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Retrieves the density of the cosmological constant.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 * @param[out]
 *
 * @return  Returns the density of the cosmological constant @$ z = 0.0
 *          @$ in units of the critical density.
 */
extern double
gadgetHeader_getOmegaLambda(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Retrieves the Hubble constant.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the Hubble constant in units of 100 km/s/Mpc.
 */
extern double
gadgetHeader_getHubbleParameter(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries whether the file stores formation times of stars.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the flag stored in the header, this should be 0 if
 *          formation times are stored and something else otherwise.
 */
extern int32_t
gadgetHeader_getFlagStellarAge(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries whether metallicities are stored for gas and stars.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the flag stored in the header, this should be 0 if
 *          metallicities are stored and something else otherwise.
 */
extern int32_t
gadgetHeader_getFlagMetal(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries whether gas particles store entropy instead of
 *         internal energy.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the flag stored in the header, this should be 0 if
 *          internal energies are stored and something else otherwise.
 */
extern int32_t
gadgetHeader_getFlagEntropy(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries whether the file uses single or double precision.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 * @param[out]
 *
 * @return  Returns the flag stored in the header, this should be 0 if
 *          single precision is used.
 */
extern int32_t
gadgetHeader_getFlagDoublePrecision(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Queries the type of initial conditions used.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the flag stored in the header, read the Gadget
 *          sources for what they mean (it should be zero for
 *          simulations that don't use that flag).
 */
extern int32_t
gadgetHeader_getFlagICInfo(const gadgetHeader_t gadgetHeader);


/**
 * @brief  Retrieves the scaling factor used in the Lagrangian
 *         Perturbation Theory ICs.
 *
 * @param[in]  gadgetHeader
 *                The header object to query.
 *
 * @return  Returns the scaling factor.
 */
extern float
gadgetHeader_getLPTScalingFactor(const gadgetHeader_t gadgetHeader);


/** @} */

/**
 * @name Extra getters
 *
 * @{
 */

/**
 * @brief  Queries the total number of particles stored in the file.
 *
 * @param[in]  gadgetHeader
 *                The header object that should be queried.
 *
 * @return  Returns the sum of np array.
 *
 * @sa gadgetHeader_getNp()
 */
extern uint64_t
gadgetHeader_getNumPartsInFile(const gadgetHeader_t gadgetHeader);


/** @} */

/**
 * @name Input/Output
 *
 * @{
 */


/**
 * @brief  Writes a Gadget Header to a file stream
 *
 * This will only write the leading and trailing block delimiters
 * (storing GADGETHEADER_SIZE) and in between the header structure.
 * writing the block descriptor is the responsibility of the calling
 * function.
 *
 * @param[in]      gadgetHeader
 *                    The header structure that should be written to the
 *                    file.
 * @param[in,out]  *f
 *                    A file pointer to readable file positioned
 *                    correctly to receive the header.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_write(const gadgetHeader_t gadgetHeader, FILE *f);


/**
 * @brief  Reads a header structure from the file and fills it into a
 *         provided Gadget header object.
 *
 * This expects the file to be positioned at the start of the header
 * block (and NOT the header descriptor block).  As such, it is the
 * inverse of gadgetHeader_write().  The function may terminate the
 * program by calling exit() if the reading did not succeed.
 *
 * @param[in,out]  gadgetHeader
 *                    The header object that will be filled with the
 *                    information in the file.
 * @param[in,out]  *f
 *                    A file pointer to an open and (binary) readable
 *                    file that is positioned at the beginning of the
 *                    header block.
 *
 * @return  Returns nothing.
 */
extern void
gadgetHeader_read(gadgetHeader_t gadgetHeader, FILE *f);


/** @} */

#endif
