// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef ARTHEADER_H
#define ARTHEADER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/artHeader.h
 * @ingroup libutilFilesART
 * @brief  This file provides the interface to ART file header.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>
#include <stdint.h>
#include "endian.h"


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for an ART header object.
 */
typedef struct artHeader_struct *artHeader_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new empty ART header object.
 *
 * @return  Returns a new header and empty ART header object.
 */
extern artHeader_t
artHeader_new(void);


/**
 * @brief  Creates a new ART header object and fills is from a PMcrd.DAT
 *         file.
 *
 * @param[in]  *headerFileName
 *                The name of the PMcrd.DAT file to use to set up the
 *                header values.  This must be a valid string, passing
 *                @c NULL is undefined.
 *
 * @return  Returns a new header object filled with the values found in
 *          the file.
 */
extern artHeader_t
artHeader_newFromFile(const char *headerFileName);


/**
 * @brief  Deletes a given ART header object.
 *
 * @param[in,out]  *header
 *                    A pointer to an external variable holding the
 *                    reference to the header object.  After deletion
 *                    the external variable will be set to @c NULL.
 *                    Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_del(artHeader_t *header);


/** @} */

/**
 * @name Setter
 *
 * @{
 */

/**
 * @brief  Sets the header string.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      *string
 *                    The new header string, this must be a proper
 *                    string and at most the first
 *                    #ARTHEADER_HEADERSTRING_LENGTH bytes are used, if
 *                    the string is shorter, the rest will be padded
 *                    with zeroes.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setHeaderString(artHeader_t header, const char *string);


/**
 * @brief  Sets the current expansion factor
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      aexpn
 *                    This must be a positive value giving the current
 *                    expansion factor.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setAexpn(artHeader_t header, float aexpn);


/**
 * @brief  Sets initial expansion factor.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      aexp0
 *                    This must be a positive value giving the initial
 *                    expansion factor.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setAexp0(artHeader_t header, float aexp0);


/**
 * @brief  Sets the amplitude.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      amplt
 *                    This must be a positive value giving an amplitude.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setAmplt(artHeader_t header, float amplt);


/**
 * @brief  Sets the expansion time-step.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      astep
 *                    This must be a positive value giving the current
 *                    time-step in a.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setAstep(artHeader_t header, float astep);


/**
 * @brief  Sets the current time step number
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      istep
 *                    This must be a positive value giving the current
 *                    time step number.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setIstep(artHeader_t header, int istep);


/**
 * @brief  Sets partw.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      partw
 *                    Gives the new value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setPartw(artHeader_t header, float partw);


/**
 * @brief  Sets tintg.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      tintg
 *                    Gives the new value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setTintg(artHeader_t header, float tintg);


/**
 * @brief  Sets ekin.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      ekin
 *                    Gives the new value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setEkin(artHeader_t header, float ekin);


/**
 * @brief  Sets ekin1.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      ekin1
 *                    Gives the new value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setEkin1(artHeader_t header, float ekin1);


/**
 * @brief  Sets ekin2.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      ekin2
 *                    Gives the new value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setEkin2(artHeader_t header, float ekin2);


/**
 * @brief  Sets au0.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      au0
 *                    Gives the new value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setAu0(artHeader_t header, float au0);


/**
 * @brief  Sets aeu0.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      aeu0
 *                    Gives the new value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setAeu0(artHeader_t header, float aeu0);


/**
 * @brief  Sets the finest employable grid.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      nrowc
 *                    Gives the grid dimension of the finest grid
 *                    employable.  This must be a positive power of two
 *                    integer and equal or larger than @c ngridc.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setNrowc(artHeader_t header, int nrowc);


/**
 * @brief  Sets the coarsest grid dimension (defines the particle mass).
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      ngridc
 *                    Gives the grid dimension of the coarsest grid,
 *                    which also defines the mass scale.  This must be a
 *                    positive power of two integer and equal or less
 *                    than @c nrowc.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setNgridc(artHeader_t header, int ngridc);


/**
 * @brief  Sets the number of particle species used.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      nspecies
 *                    The number of particle species used.  This must be
 *                    a positive integer not larger than 10.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setNspecies(artHeader_t header, int nspecies);


/**
 * @brief  Sets the original seed of the random number generator.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      nseed
 *                    The seed value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setNseed(artHeader_t header, int nseed);


/**
 * @brief  Sets the matter density parameter at z = 0.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      Om0
 *                    The value of Omega_Matter_0.  Must be a positive
 *                    value (possibly 0.0).
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setOm0(artHeader_t header, float Om0);


/**
 * @brief  Sets the density parameter of the cosmological constant at z = 0.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      Oml0
 *                    The value of Omega_Lambda_0.  Must be a positive
 *                    value (possibly 0.0).
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setOml0(artHeader_t header, float Oml0);


/**
 * @brief  Sets the hubble parameter in units of <tt>100 km/s/Mpc</tt>.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      hubble
 *                    The hubble parameter.  Must be a positive value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setHubble(artHeader_t header, float hubble);


/**
 * @brief  Sets wp5.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      wp5
 *                    Gives the new value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setWp5(artHeader_t header, float wp5);


/**
 * @brief  Sets density parameter of the curvature term at z = 0.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      Ocurv
 *                    Set value of the curvature contribution to the
 *                    energy density at z = 0.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setOcurv(artHeader_t header, float Ocurv);


/**
 * @brief  Sets an extra value.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      value
 *                    Gives the new value.
 * @param[in]      position
 *                    The position in the extra array at which to enter
 *                    the value.  This must be between (inclusive) 0 and
 *                    (inclusive) 99.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setExtras(artHeader_t header, float value, int position);


/**
 * @brief  Sets the weight of a particle species.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      value
 *                    Gives the new value.  This must be a positive
 *                    value.
 * @param[in]      position
 *                    The position in the wspecies array at which to enter
 *                    the value.  This must be between (inclusive) 0 and
 *                    (exclusive) @c nspecies.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setWspecies(artHeader_t header, float value, int position);


/**
 * @brief  Sets number of particles of a given particle species.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      value
 *                    Gives the new value.  This must be a positive
 *                    integer value.
 * @param[in]      position
 *                    The position in the lspecies array at which to enter
 *                    the value.  This must be between (inclusive) 0 and
 *                    (exclusive) @c nspecies.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setLspecies(artHeader_t header, int64_t value, int position);


/**
 * @brief  Sets the boxsize.
 *
 * @param[in,out]  header
 *                    The header that should be updated, passing @c NULL
 *                    is undefined.
 * @param[in]      boxsize
 *                    The box-size in <tt>Mpc/h</tt>.  This must be a postive
 *                    value.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setBoxsizeInMpch(artHeader_t header, float boxsize);


/** @} */

/**
 * @name Extra Setters
 *
 * @{
 */

/**
 * @brief  Sets the endianess of the file.
 *
 * This is meant to be able to write the header with a specific
 * endianess.  When reading from a file, the endianess of the file is
 * always detected automatically and the stored value for the endianess
 * is ignored.
 *
 * The proper way to use this function is the call it shortly before a
 * call to artHeader_write() is issued to ensure that the file is
 * written in the required endianess.
 *
 * Note that the only way to overwrite a user-set endianess is when
 * reading header values from a file.
 *
 * @param[in,out]  header
 *                    The header for which to set the endianess, passing
 *                    @c NULL is undefined.
 * @param[in]      fileEndianess
 *                    The endianess to set for the header.  Allowed
 *                    values are #ENDIAN_LITTLE and #ENDIAN_BIG.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_setFileEndianess(artHeader_t header, endian_t fileEndianess);

/** @} */

/**
 * @name Getter
 *
 * @{
 */

/**
 * @brief  Retrieves the header string.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns a pointer to the private memory holding the header
 *          string.  The calling function must not try to modify this
 *          string and it also not responsible for freeing it.
 */
extern const char *
artHeader_getHeaderString(const artHeader_t header);


/**
 * @brief  Retrieves the current expansion factor.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the current expansion factor.
 */
extern float
artHeader_getAexpn(const artHeader_t header);


/**
 * @brief  Retrieves the initial expansion factor.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the initial expansion factor.
 */
extern float
artHeader_getAexp0(const artHeader_t header);


/**
 * @brief  Retrieves the amplitude.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the amplitude
 */
extern float
artHeader_getAmplt(const artHeader_t header);


/**
 * @brief  Retrieves the expansion time-step.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the expansion time-step.
 */
extern float
artHeader_getAstep(const artHeader_t header);


/**
 * @brief  Retrieves the current step number.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the current step number.
 */
extern int
artHeader_getIstep(const artHeader_t header);


/**
 * @brief  Retrieves partw.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns partw.
 */
extern float
artHeader_getPartw(const artHeader_t header);


/**
 * @brief  Retrieves tintg.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns tintg.
 */
extern float
artHeader_getTintg(const artHeader_t header);


/**
 * @brief  Retrieves ekin.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns ekin.
 */
extern float
artHeader_getEkin(const artHeader_t header);


/**
 * @brief  Retrieves ekin1.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the ekin1.
 */
extern float
artHeader_getEkin1(const artHeader_t header);


/**
 * @brief  Retrieves ekin2.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns ekin2.
 */
extern float
artHeader_getEkin2(const artHeader_t header);


/**
 * @brief  Retrieves au0.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns au0.
 */
extern float
artHeader_getAu0(const artHeader_t header);


/**
 * @brief  Retrieves aeu0.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns aeu0.
 */
extern float
artHeader_getAeu0(const artHeader_t header);


/**
 * @brief  Retrieves the dimension of the finest grid.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the dimension of the finest grid.
 */
extern int
artHeader_getNrowc(const artHeader_t header);


/**
 * @brief  Retrieves the dimension of the coarsest grid.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the dimension of the coarsest grid.
 */
extern int
artHeader_getNgridc(const artHeader_t header);


/**
 * @brief  Retrieves the number of particle species used.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the number of particle species used.
 */
extern int
artHeader_getNspecies(const artHeader_t header);


/**
 * @brief  Retrieves the random number seed of the initial conditions.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the random number seed of the initial conditions.
 */
extern int
artHeader_getNseed(const artHeader_t header);


/**
 * @brief  Retrieves Omega_Matter_0.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns Omega_Matter_0.
 */
extern float
artHeader_getOm0(const artHeader_t header);


/**
 * @brief  Retrieves Omega_Lambda_0.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns Omega_Lambda_0.
 */
extern float
artHeader_getOml0(const artHeader_t header);


/**
 * @brief  Retrieves Hubble constant.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns the Hubble constant in units of <tt>100 km/s/Mpc</tt>.
 */
extern float
artHeader_getHubble(const artHeader_t header);


/**
 * @brief  Retrieves wp5.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns wp5.
 */
extern float
artHeader_getWp5(const artHeader_t header);


/**
 * @brief  Retrieves Omega_curv_0.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 *
 * @return  Returns Omega_curv_0.
 */
extern float
artHeader_getOcurv(const artHeader_t header);


/**
 * @brief  Retrieves an extra value.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 * @param[in]  position
 *                The position of the extra value to retrieve.  This
 *                must be between (inclusive) 0 and (inclusive) 99.
 *
 * @return  Returns the requested extra value.
 */
extern float
artHeader_getExtra(const artHeader_t header, int position);


/**
 * @brief  Retrieves the weight of a particular particle species.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 * @param[in]  position
 *                The particle species to retrieve the weight for.  This
 *                must be value between (inclusive) 0 and (exclusive) @c
 *                nspecies.
 *
 * @return  Returns the weight of the requested particle species.
 */
extern float
artHeader_getWspecies(const artHeader_t header, int position);


/**
 * @brief  Retrieves the number of particles of a particular species.
 *
 * @param[in]  header
 *                The header to query, passing @c NULL is undefined.
 * @param[in]  position
 *                The particle species to retrieve the number of
 *                particles for.  This must be value between (inclusive)
 *                0 and (exclusive) @c nspecies.
 *
 * @return  Returns the number of particles of the requested particle
 *          species.
 */
extern int64_t
artHeader_getLspecies(const artHeader_t header, int position);


/**
 * @brief  Retrieves the boxsize of the simulation.
 *
 * @param[in]  header
 *                The ART file object to query, passing @c NULL is
 *                undefined.
 *
 * @return  Returns the size of the simulation box in <tt>Mpc/h</tt>.
 */
extern double
artHeader_getBoxsizeInMpch(const artHeader_t header);

/** @} */

/**
 * @name Extra Getters
 *
 * @{
 */

/**
 * @brief  Retrieves the total number of particles.
 *
 * @param[in]  header
 *                The ART header object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the total number of particles.
 */
extern uint64_t
artHeader_getNumParticlesTotal(const artHeader_t header);

/**
 * @brief  Retrieves the factor to convert file weights to proper
 *         masses.
 *
 * @param[in]  header
 *                The header object to query, passing @c NULL is
 *                undefined.
 *
 * @return  Returns the factor with which the particles weights in the
 *          ART file need to be multiplied to convert them to masses in
 *          units of <tt>M_sun/h</tt>.
 */
extern double
artHeader_getFactorFileWeightToMsunh(const artHeader_t header);

/**
 * @brief  Retrieves the factor to convert file position to proper
 *         positions.
 *
 * @param[in]  header
 *                The header object to query, passing @c NULL is
 *                undefined.
 *
 * @return  Returns the factor with which the particles positions need
 *          to be multipled to convert them to <tt>Mpc/h</tt>.
 */
extern double
artHeader_getFactorFilePositionToMpch(const artHeader_t header);

/**
 * @brief  Retrieves the factor to convert file velocities to proper
 *         units.
 *
 * @param[in]  header
 *                The header object to query, passing @c NULL is
 *                undefined.
 *
 * @return  Returns the factor with which the particles velocities need
 *          to be multiplied to convert them to <tt>km/s</tt>.
 */
extern double
artHeader_getFactorFileVelocityToKms(const artHeader_t header);

/**
 * @brief  Retrievs the endianess of the file.
 *
 * @param[in]  header
 *                The header object to query, passing @c NULL is
 *                undefined.
 *
 * @return  Returns the endianess of the file.
 */
extern endian_t
artHeader_getFileEndianess(const artHeader_t header);

/** @} */

/**
 * @name Input/Output
 *
 * @{
 */

/**
 * @brief  Fills a provided ART header objects with the values from a
 *         file.
 *
 * @param[in,out]  header
 *                    The ART header object that should be filled with
 *                    the data from a file.
 * @param[in]      *headerFileName
 *                    The name of the PMcrd.DAT file to use to set up
 *                    the header values.  This must be a valid string,
 *                    passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_read(artHeader_t header, const char *headerFileName);


/**
 * @brief  Writes a given ART header object to a file.
 *
 * @param[in]  header
 *                The ART header object that should be written to a
 *                file.  Note the header might be byteswapped to write
 *                it in the correct endianess, it will, however be in
 *                the system endianess again once the function returns.
 * @param[in]  *headerFileName
 *                 The name of the PMcrd.DAT file the header should be
 *                 written to.  This must be a valid string, passing
 *                 @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_write(artHeader_t header, const char *headerFileName);


/**
 * @brief  Prints a formatted ASCII version of the header.
 *
 * @param[in]      header
 *                    The header to print.  Passing @c NULL is
 *                    undefined.
 * @param[in]      prefix
 *                    A string that should be each line should be
 *                    prefixed with.  The may be @c NULL, which is
 *                    treated as an empty string.
 * @parm[in,out]  *f
 *                    The file to print to.  This must be a proper
 *                    stream opened for writing.  Passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
extern void
artHeader_prettyPrint(const artHeader_t header,
                      const char        *prefix,
                      FILE              *f);


/** @} */


#endif
