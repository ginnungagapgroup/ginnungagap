// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef CUBEPM_H
#define CUBEPM_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/cubepm.h
 * @ingroup  libutilFilesCubePM
 * @brief  This file provides the interface to CubePM files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include "endian.h"
#include "stai.h"


/*--- Exported defines --------------------------------------------------*/

/**
 * @brief  Signifies that a number is not set yet or invalid.
 *
 * This may be returned by some functions, when requesting values that
 * haven't been initialized or used yet.
 */
#define CUBEPM_NUMBER_NONE -1


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  Gives the mode with which the files are opened. */
typedef enum {
	/**
	 * @brief  This is for writing, corresponds to "r+b".
	 *
	 * This should be used to write to an existing file.  The file will
	 * be created if it does not exist.
	 */
	CUBEPM_MODE_WRITE,
	/** @brief  This is for writing, corresponds to "rb".  */
	CUBEPM_MODE_READ
} cubepmMode_t;


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for an CUBEPM object.
 */
typedef struct cubepm_struct *cubepm_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creation and Deletion
 *
 * @{
 */

/**
 * @brief  Creates a new CubePM object.
 *
 * @param[in]  path
 *                The (absolute or relative) path to the data files of
 *                the CubePM file set.  This must be proper, non-empty
 *                string and the last character should be a '/', though
 *                this is not required.  Passing @c NULL is undefined.
 * @param[in]  stem
 *                The stem of the data file names.  This must be a
 *                proper, non-empty string and the unique part of the
 *                file names up to the file number, excluding the path.
 *                Passing @c NULL is undefined.
 * @param[in]  nodesDim
 *                The number of data nodes in one dimension, the total
 *                number of files in the CubePM set is the cube of this
 *                number.  Must be a positive, non-zero integer value.
 * @param[in]  ngrid
 *                The dimension of the grid of the CubePM file set.
 *                Presumably this must be divisible by @c nodesDim.  It
 *                must be a positive, non-zero integer.
 *
 * @return  Returns a new CubePM object for a set of
 *          <code>nodesDim^3</code> files.  The only initialized values
 *          are the file names and the grid size associated with the
 *          file.
 */
extern cubepm_t
cubepm_new(const char *path, const char *stem, int nodesDim, int ngrid);


/**
 * @brief  Deletes a given CubePM file object and frees the associated
 *         memory.
 *
 * @param[in,out]  *cubepm
 *                    A pointer to the external variable holding the
 *                    CubePM file object.  After deletion the external
 *                    variable will be set to @c NULL.  Passing @c NULL,
 *                    or a variable that is @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_del(cubepm_t *cubepm);


/**
 * @brief  Reads the header values from the file set.
 *
 * If the file endianess is different from the system endianess, the
 * header values are byte-swapped into system endianess after reading
 * (see cubepm_setFileEndianess(), note that the endianess for CubePM
 * files cannot be auto-detected and has to be set explicitly if
 * different from the system endianess).
 *
 * @param[in,out]  cubepm
 *                    The file set for which to read the header values
 *                    from the associated files.  Passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_initHeaderValuesFromFiles(cubepm_t cubepm);


/** @} */


/**
 * @name  Set File Handling Information
 *
 * @{
 */

/**
 * @brief  Sets the file endianess.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object for which to set the
 *                    endianess.  Passing @c NULL is undefined.
 * @param[in]      endianess
 *                    The endianess of the file object.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setFileEndianess(cubepm_t cubepm, endian_t endianess);


/** @} */


/**
 * @name  Get File Handling Information
 *
 * @{
 */

/**
 * @brief  Retrieves the number of files.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the number of files in the CubePM file set.
 */
extern int
cubepm_getNumFiles(const cubepm_t cubepm);


/**
 * @brief  Retrieves the file name of certain file.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 * @param[in]  fileNumber
 *                The number of file to query.  This must be a number
 *                between (inclusive) 0 and (exclusive) the total number
 *                of files in the file set.
 *
 * @return  Returns the file of the requested file.
 */
extern const char *
cubepm_getFileName(const cubepm_t cubepm, int fileNumber);


/**
 * @brief  Retrieves the endianess of the file.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the endianess of the CubePM file set.
 */
extern endian_t
cubepm_getFileEndianess(const cubepm_t cubepm);


/**
 * @brief  Retrieves the current (or last) mode in which any file was
 *         opened.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the mode with which the latest file in the CubePM
 *          file was opened.  If no file has ever been opened, this will
 *          return #CUBEPM_MODE_READ.
 */
extern cubepmMode_t
cubepm_getMode(const cubepm_t cubepm);


/**
 * @brief  Retrieves the number of the last file that has been opened.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the number of the last file that has been opened.
 *          If no file has ever been opened, this will return
 *          #CUBEPM_NUMBER_NONE.
 */
extern int
cubepm_getLastOpened(const cubepm_t cubepm);


/**
 * @brief  Retrieves the offset of the last file that has been opened.
 *
 * @param[in]   cubepm
 *                 The file object to query.  Passing @c NULL is
 *                 undefined.
 * @param[out]  *offset
 *                 An array of at least 3 @c double values which will
 *                 hold the offset of the file in x (y, z).
 *
 * @return  Returns nothing.
 */
extern void
cubepm_getFileCoordOffset(const cubepm_t cubepm, double *offset);


/** @} */


/**
 * @name  Set Header Values
 *
 * @{
 */

/**
 * @brief  Sets the number of particles in a certain file of the CubePM
 *         file set.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      np_local
 *                    The number of particles in the file.
 * @param[in]      fileNumber
 *                    The file number of the file within the CubePM file
 *                    set.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setNPLocal(cubepm_t cubepm, int32_t np_local, int fileNumber);


/**
 * @brief  Sets the current expansion factor of the file set.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      a
 *                    The expansion to set.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setA(cubepm_t cubepm, float a);


/**
 * @brief  Sets cubepm_t::t.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      t
 *                    The new value for cubepm_t::t.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setT(cubepm_t cubepm, float t);


/**
 * @brief  Sets cubepm_t::tau.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      tau
 *                    The new value for cubepm_t::tau.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setTau(cubepm_t cubepm, float tau);


/**
 * @brief  Sets cubepm_t::nts.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      nts
 *                    The new value for cubepm_t::nts.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setNts(cubepm_t cubepm, int32_t nts);


/**
 * @brief  Sets cubepm_t::dt_f_acc.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      dt_f_acc
 *                    The new value for cubepm_t::dt_f_acc.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setDtFAcc(cubepm_t cubepm, float dt_f_acc);


/**
 * @brief  Sets cubepm_t::dt_pp_acc.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      dt_pp_acc
 *                    The new value for cubepm_t::dt_pp_acc.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setDtPpAcc(cubepm_t cubepm, float dt_pp_acc);


/**
 * @brief  Sets cubepm_t::dt_c_acc.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      dt_c_acc
 *                    The new value for cubepm_t::dt_c_acc.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setDtCAcc(cubepm_t cubepm, float dt_c_acc);


/**
 * @brief  Sets cubepm_t::cur_checkpoint.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      cur_checkpoint
 *                    The new value for cubepm_t::cur_checkpoint.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setCurCheckpoint(cubepm_t cubepm, int32_t cur_checkpoint);


/**
 * @brief  Sets cubepm_t::cur_projection.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      cur_projection
 *                    The new value for cubepm_t::cur_projection.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setCurProjection(cubepm_t cubepm, int32_t cur_projection);


/**
 * @brief  Sets cubepm_t::cur_halofind.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      cur_halofind
 *                    The new value for cubepm_t::cur_halofind.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setCurHalofind(cubepm_t cubepm, int32_t cur_halofind);


/**
 * @brief  Sets how many cells constitute one particle.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      mass_p
 *                    The number of cells that contribute to form one
 *                    particle.  This is normally 8.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setMassP(cubepm_t cubepm, float mass_p);


/** @} */


/**
 * @name  Get Header Values
 *
 * @{
 */


/**
 * @brief  Retrieves the number of particles in a file.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 * @param[in]  fileNumber
 *                The number of the file within the file set to query
 *                for.  This must be an integer value between
 *                (inclusive) 0 and (exclusive) the total number of
 *                files in the file set.
 *
 * @return  Returns the number of partilces in the requested file.
 */
extern int32_t
cubepm_getNPLocal(const cubepm_t cubepm, int fileNumber);


/**
 * @brief  Retrieves the expansion factor.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the expansion factor.
 */
extern float
cubepm_getA(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::t.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::t.
 */
extern float
cubepm_getT(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::tau.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::tau.
 */
extern float
cubepm_getTau(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::nts.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::nts.
 */
extern int32_t
cubepm_getNts(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::dt_f_acc.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::dt_f_acc.
 */
extern float
cubepm_getDtFAcc(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::dt_pp_acc.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::dt_pp_acc.
 */
extern float
cubepm_getDtPpAcc(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::dt_c_acc.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::dt_c_acc.
 */
extern float
cubepm_getDtCAcc(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::cur_checkpoint.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::cur_checkpoint.
 */
extern int32_t
cubepm_getCurCheckpoint(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::cur_projection.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::cur_projection.
 */
extern int32_t
cubepm_getCurProjection(const cubepm_t cubepm);


/**
 * @brief  Retrieves cubepm_t::cur_halofind.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns cubepm_t::cur_halofind.
 */
extern int32_t
cubepm_getCurHalofind(const cubepm_t cubepm);


/**
 * @brief  Retrieves the number of cells that constitute one particle.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the number of cells that constitute one particle.
 */
extern float
cubepm_getMassP(const cubepm_t cubepm);


/** @} */


/**
 * @name  Set Extra Header Values
 *
 * @{
 */

/**
 * @brief  Sets Omega0 for the simulation.
 *
 * @param[in,out]  cubepm
 *                    The file object to work with.  Passing @c NULL is
 *                    undefined.
 * @param[in]      omega0
 *                    The value for Omega0 in units of the critical
 *                    density.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setOmega0(cubepm_t cubepm, double omega0);


/**
 * @brief  Sets OmegaLambda0 for the simulation.
 *
 * @param[in,out]  cubepm
 *                    The file object to work with.  Passing @c NULL is
 *                    undefined.
 * @param[in]      lambda0
 *                    The value for OmegaLambda0 in units of the
 *                    critical density.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setLambda0(cubepm_t cubepm, double lambda0);


/**
 * @brief  Sets the box-size for the simulation.
 *
 * @param[in,out]  cubepm
 *                    The file object to work with.  Passing @c NULL is
 *                    undefined.
 * @param[in]      boxsize
 *                    The box-size in Mpc/h.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_setBoxsizeInMpch(cubepm_t cubepm, double boxsize);


/** @} */


/**
 * @name  Get Extra Header Values
 *
 * @{
 */

/**
 * @brief  Retrieves the value for Omega0.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns Omega0.
 */
extern double
cubepm_getOmega0(const cubepm_t cubepm);


/**
 * @brief  Retrieves the value for Lambda0.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns Lambda0.
 */
extern double
cubepm_getLambda0(const cubepm_t cubepm);


/**
 * @brief  Retrieves the value for the box-size.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the box-size in Mpc/h.
 */
extern double
cubepm_getBoxsizeInMpch(const cubepm_t cubepm);


/**
 * @brief  Retrieves the number of IO nodes per dimension.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the number of IO nodes per dimension.
 */
extern int
cubepm_getNodesDim(const cubepm_t cubepm);


/**
 * @brief  Retrieves the size of the main grid.
 *
 * @param[in]  cubepm
 *                The CubePM file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the size of the main grid.
 */
extern int
cubepm_getNGrid(const cubepm_t cubepm);


/** @} */


/**
 * @name  IO Operations
 *
 * @{
 */

/**
 * @brief  Will fopen a given data file of a CubePM file set.
 *
 * If a file is already opened, it will either be closed and the
 * requested one opened.  If the same file is opened with a different
 * mode, it will be closed and then opened with the requested mode.
 * Opening the same file with the same mode will simply rewind the file.
 *
 * After opening, the file will be positioned at the beginning of the
 * file and internally the coordinate offset of the file will be
 * calculated.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work on.  Passing
 *                    @c NULL is undefined.
 * @param[in]      mode
 *                    The mode with which to open the data file.
 * @param[in]      fileNumber
 *                    The file number of the data file within the CubePM
 *                    file set to open.  This must be a number between
 *                    (inclusive) 0 and (exclusive) the total number of
 *                    files.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_open(cubepm_t cubepm, cubepmMode_t mode, int fileNumber);


/**
 * @brief  This will fclose the currently opened data file.
 *
 * If no file was opened, this function will do nothing.
 *
 * @param[in,out]  cubepm
 *                    The file object for which to close the current
 *                    file handle.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_close(cubepm_t cubepm);

/**
 * @brief  This will create an empty file with the correct size in which
 *         all IO operations are guaranteed to succeed if within the
 *         limits of the file.
 *
 * @param[in]  cubepm
 *                The CubePM object.  This is required to obtain the
 *                correct file name and the correct size of the file
 *                (number of particles in the file).  Passing @c NULL is
 *                undefined and the number of particles must be
 *                initialized.
 * @param[in]  fileNumber
 *                The file that should be created.  This must be a valid
 *                file number within the CubePM file set.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_createEmptyFile(const cubepm_t cubepm, int fileNumber);

/**
 * @brief  This will write the header data of a file.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object for which to write a
 *                    header.  For the writing of the header data to be
 *                    sensible, all header fields should be initialized
 *                    correctly.
 * @param[in]      fileNumber
 *                    The number of the file for which to write the
 *                    header data.  This must be a valid file number
 *                    within the CubePM file set.
 *
 * @return  Returns nothing.
 */
extern void
cubepm_writeHeaderToFile(cubepm_t cubepm, int fileNumber);

/**
 * @brief  This will read a subset of particles from a specific file.
 *
 * The particles will be returned in quasi-internal units: The positions
 * of the particles will be in the global context of the file set, not
 * in the file internal positions; the velocities are in internal units.
 *
 * If a file has been opened before calling this functions, the file
 * will make sure that the correct file is opened and leave it opened
 * when returning.  If no file was opened, the file that is being read
 * from, will be closed before returning.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to read from.  Passing
 *                    @c NULL is undefined.
 * @param[in]      fileNumber
 *                    The number of the file to read from.  This must be
 *                    a valid file number.
 * @param[in]      pSkip
 *                    The number of particles to skip in the file.
 * @param[in]      pRead
 *                    The number of particles to read from the file.
 *                    The sum of @c pSkip and @c pRead must be less or
 *                    equal to the total number of particles in the
 *                    file.
 * @param[in,out]  *data
 *                    The external data arrays into which to write the
 *                    particle data.  See cubepm_read() for more
 *                    details.
 *
 * @return  Returns the number of particles that actually have been
 *          read.  This should be equal @c pRead.
 */
extern uint64_t
cubepm_readFromFile(cubepm_t cubepm,
                    int      fileNumber,
                    uint64_t pSkip,
                    uint64_t pRead,
                    stai_t   *data);


/**
 * @brief  This will read a subset of particles from a CubePM file set.
 *
 * The particles will be returned in quasi-internal units.  They will be
 * in the correct system endianess and the according file offset will be
 * applied, so that the particle positions make sense in the global
 * picture.  They will also be periodically wrapped to be within the
 * simulation volume.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to read from.  Passing
 *                    @c NULL is undefined.
 * @param[in]      pSkip
 *                    The total number of particles to skip.
 * @param[in]      pRead
 *                    The total number of particles to read.
 * @param[in,out]  *data
 *                    This must be an array of at least 6 stais (it may
 *                    be more, but only the first 6 are used), which
 *                    hold the description of the x, y, and z components
 *                    of the position and velocity, respectively.  The
 *                    mapping is
 *                    <ul>
 *                      <li> data[0]: x component of the position
 *                      <li> data[1]: y component of the position
 *                      <li> data[2]: z component of the position
 *                      <li> data[3]: x component of the velocity
 *                      <li> data[4]: y component of the velocity
 *                      <li> data[5]: z component of the velocity
 *                    </ul>
 *                    Any or all of the those stais may be @c NULL,
 *                    signifying that for those components nothing it to
 *                    be to read from the file and to be skipped over.
 *
 * @return  Returns the number of particles that have been read, this
 *          should be the same number as @c pRead.
 */
extern uint64_t
cubepm_read(cubepm_t cubepm, uint64_t pSkip, uint64_t pRead, stai_t *data);


/**
 * @brief  This will write a selection of particles to a specific file
 *         in a CubePM file set.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to write to. Passing
 *                    @c NULL is undefined.
 * @param[in]      fileNumber
 *                    The file within the CubePM file set to write to.
 *                    This must be a valid file number.
 * @param[in]      pSkip
 *                    The number of particles to skip before starting to
 *                    write.
 * @param[in]      pWrite
 *                    The number of particles to write.  The sum of
 *                    @c pSkip and @c pWrite must be less or equal to
 *                    the total number of particles in the file.
 * @param[in,out]  *data
 *                    The external arrays holding the particle data.
 *                    See cubepm_read() and cubepm_write() for more
 *                    details on the structure on the units.
 *
 * @return  Returns the number of particles that have been written.
 *          This should be equal to @c pWrite.
 */
extern uint64_t
cubepm_writeToFile(cubepm_t cubepm,
                   int      fileNumber,
                   uint64_t pSkip,
                   uint64_t pWrite,
                   stai_t   *data);


/**
 * @brief  This will write a subset of particles to a CubePM file set.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to write to.  Passing
 *                    @c NULL is undefined.
 * @param[in]      pSkip
 *                    The number of particles to skip over before to
 *                    start writing.
 * @param[in]      pWrite
 *                    The number of particles to write.  The sum of
 *                    @c pWrite and @c pSkip must be less of equal to
 *                    the total number of particles in the file set.
 * @param[in]      *data
 *                    The external arrays holding the particle data.
 *                    See cubepm_read() for more details.  The particle
 *                    data must be in CubePM units.  This is not
 *                    enforced though.
 *
 * @return  Returns the number of particles that have been written.
 *          This should be equal to @c pWrite.
 */
extern uint64_t
cubepm_write(cubepm_t cubepm, uint64_t pSkip, uint64_t pWrite, stai_t *data);


/** @} */


/**
 * @name  Extras
 *
 * @{
 */

/**
 * @brief  Prints a pretty version of the CubePM object to a provided
 *         stream.
 *
 * @param[in]   cubepm
 *                 The file object to query.  Passing @c NULL is
 *                 undefined.
 * @param[in]   *prefix
 *                 Every line of the output is prefixed with this string.
 *                 This must be a proper string, or @c NULL which is
 *                 interpreted as the empty string.
 * @param[out]  *f
 *                 The stream to write to.  This must be a proper
 *                 stream, that can be written to.  Passing @c NULL is
 *                 undefined.
 *
 * @return  Returns the mass of a single particle in Msun/h.
 */
extern void
cubepm_prettyPrint(const cubepm_t cubepm, const char *prefix, FILE *f);


/**
 * @brief  Calculates the position conversion factor.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the conversion factor from internal units to Mpc/h.
 */
extern double
cubepm_calcFactorFilePositionToMpch(const cubepm_t cubepm);


/**
 * @brief  Calculates the conversion factor from internal velocity units
 *         to km/s.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the conversion factor for the velocities from
 *          internal units to km/s.
 */
extern double
cubepm_calcFactorFileVelocityToKms(const cubepm_t cubepm);


/**
 * @brief  Calculates the mass of the particles in solar masses per h.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the mass of a single particle in Msun/h.
 */
extern double
cubepm_calcFactorParticleMassInMsunh(const cubepm_t cubepm);


/**
 * @brief  This will calculate the total number of particles in the file
 *         set.
 *
 * @param[in]  cubepm
 *                The file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  The total number of particles in the file set.  This is
 *          the sum of the number of particles in the single files over
 *          all files.
 */
extern uint64_t
cubepm_calcTotalNumParts(const cubepm_t cubepm);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilFilesCubePM CubePM
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to CubePM files.
 *
 */

#endif
