// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/cubepm.c
 * @ingroup libutilFilesCubePM
 * @brief  The file implements the CubePM file object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "cubepm.h"
#include <assert.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include "xmem.h"
#include "xstring.h"
#include "xfile.h"
#include "byteswap.h"
#include "utilMath.h"
#include "diediedie.h"


/*--- Implementation of main structure ----------------------------------*/
#include "cubepm_adt.h"


/*--- Local defines -----------------------------------------------------*/

/**
 * @brief  Gives the maximum number of digits that may be used for the
 *         file number.
 */
#define CUBEPM_MAX_FILEDIGITS 7

/**
 * @brief  Gives the largest number of files that are allowed.
 *
 * This must be compatible with to #CUBEPM_MAX_FILEDIGITS, i.e. as the
 * number of files varies from 0 (inclusive) to #CUBEPM_MAX_NUMFILES
 * (exclusive), the highest file number (#CUBEPM_MAX_NUMFILES - 1) must
 * not have more digits than specified with #CUBEPM_MAX_FILEDIGITS.
 */
#define CUBEPM_MAX_NUMFILES 10000000

/**
 * @brief  Gives the size of the header in each CubePM file in bytes.
 */
#define CUBEPM_HEADER_SIZE 48

/**
 * @brief  Gives the size of a CubePM particle in the file in bytes.
 *
 * This is simply sizeof(float) * 6.
 */
#define CUBEPM_PARTICLE_SIZE 24

/**
 * @brief  Gives the critical density in (M_sun/h)/(Mpc/h)^3
 * @todo  This really should not be here but in a proper cosmology
 *        header.
 */
#define RHO_CRIT_0 2.7755397e11


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Initializes the values in the main structure to null values.
 *
 * @param[in,out]  cubepm
 *                    The main structure that should be nulled.  Passing
 *                    @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
static void
local_nullRestOfStructure(cubepm_t cubepm);


/**
 * @brief  Constructs the CubePM file names.
 *
 * The file names will have the form
 * "<code>@<path@>@<stem@>%i.dat</code>".
 *
 * @param[in]  path
 *                The path the files.  This must be a proper string that
 *                is an relative or absolute path.  The last character
 *                should be an '@c /', but if not, it will be added.
 *                Passing @c NULL is undefined.
 * @param[in]  stem
 *                The stem of files.  This should be proper string and
 *                giving the unique part of the file name up to the
 *                numbering of the files.  Passing @c NULL is undefined.
 * @param[in]  numFiles
 *                The number of files in the set.  This must be a
 *                positive value that is a cube of an integer.
 *
 * @return  Returns an array of @c numFiles strings containing the
 *          actual file names of the CubePM data files.
 */
static char **
local_constructFileNames(const char *path, const char *stem, int numFiles);


/**
 * @brief  Will byteswap those values in the CubePM structure that are
 *         read from (or written to) a file.
 *
 * @param[in,out]  cubepm
 *                    The CubePM object whose header values should be
 *                    byte swapped.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
static void
local_byteswapHeaderValues(cubepm_t cubepm);


/**
 * @brief  Calculates the offset of a specific file of a CubePM file set.
 *
 * The filenumber is a one dimensional number encoding the 3D position
 * of the data cube associated with the give file.  The offset will give
 * the offset (in grid cells) of the particles in that data cube.  The
 * filenumber @f$ i @f$ is calculated from the position @f$ (x,y,z) @f$ of
 * the datacube as
 * @f[
 *   i = z \times \mathrm{nodesDim}^2 + y \times \mathrm{nodesDim} + x
 * @f]
 *
 * @param[in]   fileNumber
 *                 The file for which to calculate the offset.  This
 *                 must be correct file number.
 * @param[in]   nodesDim
 *                 The number of data nodes per dimension, the total
 *                 number of files is the cube of this number.  This
 *                 must be a postive integer.
 * @param[in]   ngrid
 *                 The number of grid cells in each dimension.
 * @param[out]  *offset
 *                 Pointer to a double array of at least 3 values.  Only
 *                 the first 3 values will be filled with the according
 *                 x, (y, z) offset of the particles in the file.
 *
 * @return  Returns nothing.
 */
static void
local_calculateFileCoordOffset(int    fileNumber,
                               int    nodesDim,
                               int    ngrid,
                               double *offset);


/**
 * @brief  Seeks to the beginning of the requested particle.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object.  Passing @c NULL or a file
 *                    object that does not have a file opened leads to
 *                    undefined behaviour.
 * @param[in]      particleNumber
 *                    The particle to seek to.  This must be a valid
 *                    particle number or the seek will fail.
 *
 * @return  Returns nothing.
 */
static void
local_seekToParticle(cubepm_t cubepm, uint64_t particleNumber);


/**
 * @brief  This will read one particle from a file.
 *
 * @param[in,out]  *f
 *                    The file stream from which to read the particle.
 *                    This must be positioned at the correct position
 *                    for this particle.  Passing @c NULL is undefined.
 * @param[out]     *partData
 *                    An array of at least 6 floats that will receive
 *                    the particle data: the x (y, z) components of the
 *                    position and velocity respectively.  Passing
 *                    @c NULL is undefined.
 * @param[in]      doByteswap
 *                    This will indicated whether the data that has been
 *                    read from the file should be byte-swapped to
 *                    correct for endianess issues.
 *
 * @return  Returns nothing.
 */
static void
local_readParticle(FILE  *f,
                   float *partData,
                   bool  doByteswap);


/**
 * @brief  This will write one particle to a file.
 *
 * Generally, all six elements of the particle data are written to the
 * file, regardless of their values.  I.e. the caller must ensure that
 * the particle data has the correct units for a CubePM file and in
 * particular the file it is written to (offset issues).  However, it is
 * possible to only write a subset of the particle data to the file.  To
 * indicate, that a component should not be written, it should be set to
 * @c NAN.
 *
 * This feature will only work if the file already exists in fullness
 * and only parts are being overwritten.
 *
 * @param[in,out]  *f
 *                    The file stream to which the particle should be
 *                    written.  This must be positioned at the right
 *                    for this particle.  Passing @c NULL is undefined.
 * @param[in]      *partData
 *                    An array of at least 6 floats that will be used to
 *                    write data to file.  Any of those elements may be
 *                    @c NAN, which indicates that no value should be
 *                    written for this element.  Passing @c NULL is
 *                    undefined.
 * @param[in]      doByteswap
 *                    Indicates whether the data needs to be
 *                    byte-swapped before being written to the file to
 *                    correct for endianess issues.
 *
 * @return  Returns nothing.
 */
static void
local_writeParticle(FILE        *f,
                    const float *partData,
                    bool        doByteswap);


/**
 * @brief  Copies the particle data into the provided stais.
 *
 * @param[in]      *partData
 *                    An array of at least six elements.  Passing
 *                    @c NULL is undefined.
 * @param[in]      pos
 *                    The position within the stais at which to enter
 *                    the particle information.  This must be a valid
 *                    position within the arrays the stais describe.
 * @param[in,out]  *data
 *                    An array of at least six stais that hold the
 *                    description of the external arrays that will
 *                    receive the particle data.  Passing @c NULL is
 *                    undefined.  Any of the stais may be @c NULL
 *                    indicating that the corresponding particle data
 *                    should be ignored.
 *
 * @return  Returns nothing.
 */
static void
local_copyParticleDataToStais(const float *partData,
                              uint64_t    pos,
                              stai_t      *data);


/**
 * @brief  Retrieves the data for one particle from a set of stais.
 *
 * @param[out]  *partData
 *                 An array of at least six elements that will receive
 *                 the actual particle data.  Note that components that
 *                 have no according stai, will be set to @c NAN.
 * @param[in]   pos
 *                 The position of the particle within the stais to
 *                 retrieve.  This must be a valid position.
 * @param[in]   *data
 *                 An array of at least six stais that hold the
 *                 description of the external arrays holding the
 *                 particle data.  Passing @c NULL is undefined,
 *                 however, any of the stais itself may be @c NULL,
 *                 indicating that the corresponding component should
 *                 not be retrieved.
 *
 * @return  Returns nothing.
 */
static void
local_fillParticleDataFromStais(float *partData, uint64_t pos, stai_t *data);


/**
 * @brief  Resets the base of the data stais.
 *
 * This apply a correction to the base of the data stai to take into
 * account the number of particles read/written.
 *
 * @param[in,out]  *data
 *                    The stais to offset.  This array must hold at
 *                    least 6 stais and only the first 6 are used.  Any
 *                    of those may be @c NULL.
 * @param[in]      offsetElements
 *                    The number of elements to offset the base element.
 *                    This offset may be negative, according with
 *                    whether a later element (positive) or an earlier
 *                    element (negative) should be the new base element.
 *
 * @return  Returns nothing.
 */
inline static void
local_rebaseStaisData(stai_t *data, int64_t offsetElements);


/**
 * @brief  Calculates the first and last file that are being affected.
 *
 * @param[in]   cubepm
 *                 The CubePM file object to work with.  Passing @c NULL
 *                 is undefined.
 * @param[in]   pSkip
 *                 The total number of particles to skip in the file
 *                 set.
 * @param[in]   pAct
 *                 The total number of particles to act on in the file
 *                 set.  The sum of @c pSkip and @c pAct must be less or
 *                 equal than the total number of particles in the file
 *                 set.
 * @param[out]  *firstFile
 *                 This will receive the number of the first affected
 *                 file, passing @c NULL is undefined.
 * @param[out]  *lastFile
 *                 This will receive the number of the last affected
 *                 file, passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
static void
local_calcFirstLastFile(const cubepm_t cubepm,
                        uint64_t       pSkip,
                        uint64_t       pAct,
                        int            *firstFile,
                        int            *lastFile);


/**
 * @brief  Calculates the number of particle to skip and act on in a
 *         given file.
 *
 * @param[in]   cubepm
 *                 The CubePM file object to work with.  Passing @c NULL
 *                 is undefined.
 * @param[in]   pSkip
 *                 The total number of particles to skip in the file
 *                 set, excluding the particles that already have been
 *                 skipped to reach the file.
 * @param[in]   pAct
 *                 The total number of particles to act on in the file
 *                 set, excluding the particle that already have been
 *                 processed to reach this file.
 * @param[in]   fileNumber
 *                 The number of the file for which to calculate the
 *                 according skip and act numbers.
 * @param[out]  *pSkipFile
 *                 This will receive the number of particles that need
 *                 to be skipped in the given file.  Passing @c NULL is
 *                 undefined.
 * @param[out]  *pActFile
 *                 This will receive the number of particles that need
 *                 to be acted upon in the given file.  Passing @c NULL
 *                 is undefined.
 *
 * @return  Returns nothing.
 */
static void
local_calcSkipActFile(const cubepm_t cubepm,
                      uint64_t       pSkip,
                      uint64_t       pAct,
                      int            fileNumber,
                      uint64_t       *pSkipFile,
                      uint64_t       *pActFile);


/**
 * @brief  Adds a position offset to the particle data.
 *
 * @param[in,out]  *partData
 *                    Array of at least three elements.  Passing @c NULL
 *                    is undefined.
 * @param[in]      *offset
 *                    Array of at least three elements giving the offset
 *                    that should be applied.  Passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
inline static void
local_applyOffsetToParticle(float *partData, const double *offset);


/**
 * @brief  Subtracts a position offset from the particle data.
 *
 * @param[in,out]  *partData
 *                    Array of at least three elements.  Passing @c NULL
 *                    is undefined.
 * @param[in]      *offset
 *                    Array of at least three elements giving the offset
 *                    that should be applied.  Passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
inline static void
local_unapplyOffsetToParticle(float *partData, const double *offset);


/**
 * @brief  Makes sure that the particle positions are within the
 *         simulation volume.
 *
 * After this functions has been called, the particle positions will be
 * between (inclusive) 0 and (exclusive) ngrid.
 *
 * @param[in,out]  *partData
 *                    Array of at least three elements.  Passing @c NULL
 *                    is undefined.
 * @param[in]      ngrid
 *                    The size of the simulation volume.  This must be a
 *                    positive integer.
 *
 * @return  Returns nothing.
 */
inline static void
local_applyPeriodicityToParticle(float *partData, int ngrid);


/**
 * @brief  This will act (read or write) on a file set.
 *
 * The function will deal internally with opening and closing the
 * correct data files.
 *
 * @param[in,out]  cubepm
 *                    The CubePM file object to work with.  Passing
 *                    @c NULL is undefined.
 * @param[in]      pSkip
 *                    The total number of particles to skip in the file
 *                    set before to start acting.
 * @param[in]      pAct
 *                    The total number of particles to act on in the
 *                    file set.  The sum of @c pSkip and @c pAct must be
 *                    less or equal to the total number of particles in
 *                    the file set.
 * @param[in,out]  *data
 *                    See cubepm_read() for a further description.
 * @param[in]      mode
 *                    The action mode, this must be either
 *                    #CUBEPM_MODE_READ or #CUBEPM_MODE_WRITE.
 *
 * @return  Returns the total number of particles that has been acted
 *          upon.  This should be equal to @c pAct.
 */
static uint64_t
local_actOnFileSet(cubepm_t     cubepm,
                   uint64_t     pSkip,
                   uint64_t     pAct,
                   stai_t       *data,
                   cubepmMode_t mode);


/**
 * @brief  This will act (read or write) on a single file.
 *
 * This requires the file to act on to already be opened in with the
 * correct mode and the mode (read or write) will be taken to be the one
 * with which the file was opened.
 *
 * @param[in,out]  cubepm
 *                    The file object to work with.  Passing @c NULL is
 *                    undefined.
 * @param[in,out]  pSkip
 *                    The total number of particles to skip in the file
 *                    before to start acting.
 * @param[in,out]  pAct
 *                    The total number of particles to act on in the
 *                    file.  The sum of @c pSkip and @c pAct must be
 *                    less or equal to the total number of particles in
 *                    the file.
 * @param[in,out]  *data
 *                    See cubepm_read() for a further description.
 *
 * @return  Returns the total number of particles that has been acted
 *          upon.  This should be equal to @c pAct.
 */
static uint64_t
local_actOnSingleFile(cubepm_t cubepm,
                      uint64_t pSkip,
                      uint64_t pAct,
                      stai_t   *data);


/*--- Implementations of exported functions -----------------------------*/
extern cubepm_t
cubepm_new(const char *path, const char *stem, int nodesDim, int ngrid)
{
	cubepm_t cubepm;

	assert(path != NULL);
	assert(stem != NULL);
	assert(nodesDim > 0);
	assert((ngrid > 0) && (ngrid % nodesDim == 0));

	cubepm            = xmalloc(sizeof(struct cubepm_struct));
	cubepm->nodesDim  = nodesDim;
	cubepm->ngrid     = ngrid;

	cubepm->numFiles  = POW3(nodesDim);
	assert(cubepm->numFiles < CUBEPM_MAX_NUMFILES);
	cubepm->fileNames = local_constructFileNames(path, stem,
	                                             cubepm->numFiles);
	cubepm->np_local  = xmalloc(sizeof(int32_t) * cubepm->numFiles);
	local_nullRestOfStructure(cubepm);

	return cubepm;
}

extern void
cubepm_del(cubepm_t *cubepm)
{
	assert(cubepm != NULL && *cubepm != NULL);

	if ((*cubepm)->f != NULL)
		cubepm_close(*cubepm);

	if ((*cubepm)->fileNames != NULL) {
		for (int i = 0; i < (*cubepm)->numFiles; i++) {
			if ((*cubepm)->fileNames[i] != NULL)
				xfree((*cubepm)->fileNames[i]);
		}
		xfree((*cubepm)->fileNames);
	}
	if ((*cubepm)->np_local != NULL)
		xfree((*cubepm)->np_local);
	xfree(*cubepm);

	*cubepm = NULL;
}

extern void
cubepm_initHeaderValuesFromFiles(cubepm_t cubepm)
{
	assert(cubepm != NULL);
	assert(cubepm->fileNames != NULL);

	cubepm_open(cubepm, CUBEPM_MODE_READ, 0);
	xfread(cubepm->np_local, sizeof(int32_t), 1, cubepm->f);
	xfread(&(cubepm->a), sizeof(float), 1, cubepm->f);
	xfread(&(cubepm->t), sizeof(float), 1, cubepm->f);
	xfread(&(cubepm->tau), sizeof(float), 1, cubepm->f);
	xfread(&(cubepm->nts), sizeof(int32_t), 1, cubepm->f);
	xfread(&(cubepm->dt_f_acc), sizeof(float), 1, cubepm->f);
	xfread(&(cubepm->dt_pp_acc), sizeof(float), 1, cubepm->f);
	xfread(&(cubepm->dt_c_acc), sizeof(float), 1, cubepm->f);
	xfread(&(cubepm->cur_checkpoint), sizeof(int32_t), 1, cubepm->f);
	xfread(&(cubepm->cur_projection), sizeof(int32_t), 1, cubepm->f);
	xfread(&(cubepm->cur_halofind), sizeof(int32_t), 1, cubepm->f);
	xfread(&(cubepm->mass_p), sizeof(float), 1, cubepm->f);
	cubepm_close(cubepm);
	for (int i = 1; i < cubepm->numFiles; i++) {
		cubepm_open(cubepm, CUBEPM_MODE_READ, i);
		xfread(cubepm->np_local + i, sizeof(int32_t), 1, cubepm->f);
		cubepm_close(cubepm);
	}

	if (cubepm->fileEndianess != endian_getSystemEndianess())
		local_byteswapHeaderValues(cubepm);
}

extern void
cubepm_setFileEndianess(cubepm_t cubepm, endian_t endianess)
{
	assert(cubepm != NULL);
	assert(endianess == ENDIAN_LITTLE || endianess == ENDIAN_BIG);

	cubepm->fileEndianess = endianess;
}

extern int
cubepm_getNumFiles(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->numFiles;
}

extern const char *
cubepm_getFileName(const cubepm_t cubepm, int fileNumber)
{
	assert(cubepm != NULL);
	assert(fileNumber >= 0 && fileNumber < cubepm->numFiles);

	return cubepm->fileNames[fileNumber];
}

extern endian_t
cubepm_getFileEndianess(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->fileEndianess;
}

extern void
cubepm_getFileCoordOffset(const cubepm_t cubepm, double *offset)
{
	assert(cubepm != NULL);
	assert(offset != NULL);

	for (int i = 0; i < 3; i++)
		offset[i] = cubepm->fileCoordOffset[i];
}

extern cubepmMode_t
cubepm_getMode(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->mode;
}

extern int
cubepm_getLastOpened(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->lastOpened;
}

extern void
cubepm_setNPLocal(cubepm_t cubepm, int32_t np_local, int fileNumber)
{
	assert(cubepm != NULL);
	assert(fileNumber >= 0 && fileNumber < cubepm->numFiles);
	assert(np_local >= 0);

	cubepm->np_local[fileNumber] = np_local;
}

extern void
cubepm_setA(cubepm_t cubepm, float a)
{
	assert(cubepm != NULL);

	cubepm->a = a;
}

extern void
cubepm_setT(cubepm_t cubepm, float t)
{
	assert(cubepm != NULL);

	cubepm->t = t;
}

extern void
cubepm_setTau(cubepm_t cubepm, float tau)
{
	assert(cubepm != NULL);

	cubepm->tau = tau;
}

extern void
cubepm_setNts(cubepm_t cubepm, int32_t nts)
{
	assert(cubepm != NULL);

	cubepm->nts = nts;
}

extern void
cubepm_setDtFAcc(cubepm_t cubepm, float dt_f_acc)
{
	assert(cubepm != NULL);

	cubepm->dt_f_acc = dt_f_acc;
}

extern void
cubepm_setDtPpAcc(cubepm_t cubepm, float dt_pp_acc)
{
	assert(cubepm != NULL);

	cubepm->dt_pp_acc = dt_pp_acc;
}

extern void
cubepm_setDtCAcc(cubepm_t cubepm, float dt_c_acc)
{
	assert(cubepm != NULL);

	cubepm->dt_c_acc = dt_c_acc;
}

extern void
cubepm_setCurCheckpoint(cubepm_t cubepm, int32_t cur_checkpoint)
{
	assert(cubepm != NULL);

	cubepm->cur_checkpoint = cur_checkpoint;
}

extern void
cubepm_setCurProjection(cubepm_t cubepm, int32_t cur_projection)
{
	assert(cubepm != NULL);

	cubepm->cur_projection = cur_projection;
}

extern void
cubepm_setCurHalofind(cubepm_t cubepm, int32_t cur_halofind)
{
	assert(cubepm != NULL);

	cubepm->cur_halofind = cur_halofind;
}

extern void
cubepm_setMassP(cubepm_t cubepm, float mass_p)
{
	assert(cubepm != NULL);

	cubepm->mass_p = mass_p;
}

extern int32_t
cubepm_getNPLocal(const cubepm_t cubepm, int fileNumber)
{
	assert(cubepm != NULL);
	assert(fileNumber >= 0 && fileNumber < cubepm->numFiles);

	return cubepm->np_local[fileNumber];
}

extern float
cubepm_getA(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->a;
}

extern float
cubepm_getT(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->t;
}

extern float
cubepm_getTau(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->tau;
}

extern int32_t
cubepm_getNts(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->nts;
}

extern float
cubepm_getDtFAcc(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->dt_f_acc;
}

extern float
cubepm_getDtPpAcc(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->dt_pp_acc;
}

extern float
cubepm_getDtCAcc(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->dt_c_acc;
}

extern int32_t
cubepm_getCurCheckpoint(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->cur_checkpoint;
}

extern int32_t
cubepm_getCurProjection(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->cur_projection;
}

extern int32_t
cubepm_getCurHalofind(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->cur_halofind;
}

extern float
cubepm_getMassP(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->mass_p;
}

extern void
cubepm_setOmega0(cubepm_t cubepm, double omega0)
{
	assert(cubepm != NULL);

	cubepm->omega0 = omega0;
}

extern void
cubepm_setLambda0(cubepm_t cubepm, double lambda0)
{
	assert(cubepm != NULL);

	cubepm->lambda0 = lambda0;
}

extern void
cubepm_setBoxsizeInMpch(cubepm_t cubepm, double boxsize)
{
	assert(cubepm != NULL);

	cubepm->boxsize = boxsize;
}

extern double
cubepm_getOmega0(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->omega0;
}

extern double
cubepm_getLambda0(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->lambda0;
}

extern double
cubepm_getBoxsizeInMpch(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->boxsize;
}

extern int
cubepm_getNodesDim(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->nodesDim;
}

extern int
cubepm_getNGrid(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->ngrid;
}

extern void
cubepm_open(cubepm_t cubepm, cubepmMode_t mode, int fileNumber)
{
	assert(cubepm != NULL);
	assert(fileNumber >= 0 && fileNumber < cubepm->numFiles);
	assert(mode == CUBEPM_MODE_READ || mode == CUBEPM_MODE_WRITE);

	if (cubepm->f != NULL) {
		if ((cubepm->lastOpened != fileNumber) || (cubepm->mode != mode)) {
			xfclose(&(cubepm->f));
		} else if ((cubepm->lastOpened == fileNumber)
		           && (cubepm->mode == mode)) {
			rewind(cubepm->f);
			return;
		}
	}

	cubepm->mode = mode;
	if (cubepm->mode == CUBEPM_MODE_READ)
		cubepm->f = xfopen(cubepm->fileNames[fileNumber], "rb");
	else
		cubepm->f = xfopen(cubepm->fileNames[fileNumber], "r+b");

	cubepm->lastOpened = fileNumber;
	local_calculateFileCoordOffset(fileNumber, cubepm->nodesDim,
	                               cubepm->ngrid, cubepm->fileCoordOffset);
}

extern void
cubepm_close(cubepm_t cubepm)
{
	assert(cubepm != NULL);

	if (cubepm->f != NULL)
		xfclose(&(cubepm->f));
}

extern void
cubepm_createEmptyFile(const cubepm_t cubepm, int fileNumber)
{
	assert(cubepm != NULL);
	assert(cubepm->fileNames != NULL);
	assert(fileNumber >= 0 && fileNumber < cubepm->numFiles);
	assert(cubepm->fileNames[fileNumber] != NULL);
	assert(cubepm->np_local[fileNumber] >= 0);

	size_t fileSize = cubepm->np_local[fileNumber];
	fileSize += CUBEPM_HEADER_SIZE;

	xfile_createFileWithSize(cubepm->fileNames[fileNumber], fileSize);
}

extern void
cubepm_writeHeaderToFile(cubepm_t cubepm, int fileNumber)
{
	assert(cubepm != NULL);
	assert(cubepm->fileNames != NULL);
	assert(fileNumber >= 0 && fileNumber < cubepm->numFiles);
	assert(cubepm->fileNames[fileNumber] != NULL);

	bool wasOpened = (cubepm->f != NULL) ? true : false;

	cubepm_open(cubepm, CUBEPM_MODE_WRITE, fileNumber);
	xfwrite(cubepm->np_local + fileNumber, sizeof(int32_t), 1, cubepm->f);
	xfwrite(&(cubepm->a), sizeof(float), 1, cubepm->f);
	xfwrite(&(cubepm->t), sizeof(float), 1, cubepm->f);
	xfwrite(&(cubepm->tau), sizeof(float), 1, cubepm->f);
	xfwrite(&(cubepm->nts), sizeof(int32_t), 1, cubepm->f);
	xfwrite(&(cubepm->dt_f_acc), sizeof(float), 1, cubepm->f);
	xfwrite(&(cubepm->dt_pp_acc), sizeof(float), 1, cubepm->f);
	xfwrite(&(cubepm->dt_c_acc), sizeof(float), 1, cubepm->f);
	xfwrite(&(cubepm->cur_checkpoint), sizeof(int32_t), 1, cubepm->f);
	xfwrite(&(cubepm->cur_projection), sizeof(int32_t), 1, cubepm->f);
	xfwrite(&(cubepm->cur_halofind), sizeof(int32_t), 1, cubepm->f);
	xfwrite(&(cubepm->mass_p), sizeof(float), 1, cubepm->f);

	if (!wasOpened)
		cubepm_close(cubepm);
}

extern uint64_t
cubepm_readFromFile(cubepm_t cubepm,
                    int      fileNumber,
                    uint64_t pSkip,
                    uint64_t pRead,
                    stai_t   *data)
{
	assert(cubepm != NULL);
	assert(fileNumber >= 0 && fileNumber < cubepm->numFiles);
	assert(pSkip + pRead <= (uint64_t)cubepm_getNPLocal(cubepm, fileNumber));
	assert(data != NULL);

	uint64_t tmpRtn;
	bool     wasOpened = (cubepm->f == NULL) ? false : true;

	cubepm_open(cubepm, CUBEPM_MODE_READ, fileNumber);

	tmpRtn = local_actOnSingleFile(cubepm, pSkip, pRead, data);

	if (!wasOpened)
		cubepm_close(cubepm);

	return tmpRtn;
}

extern uint64_t
cubepm_read(cubepm_t cubepm, uint64_t pSkip, uint64_t pRead, stai_t *data)
{
	assert(cubepm != NULL);
	assert(pSkip + pRead <= cubepm_calcTotalNumParts(cubepm));
	assert(data != NULL);

	return local_actOnFileSet(cubepm, pSkip, pRead, data,
	                          CUBEPM_MODE_READ);
}

extern uint64_t
cubepm_writeToFile(cubepm_t cubepm,
                   int      fileNumber,
                   uint64_t pSkip,
                   uint64_t pWrite,
                   stai_t   *data)
{
	assert(cubepm != NULL);
	assert(fileNumber >= 0 && fileNumber < cubepm->numFiles);
	assert(pSkip + pWrite <= (uint64_t)cubepm_getNPLocal(cubepm, fileNumber));
	assert(data != NULL);

	uint64_t tmpRtn;
	bool     wasOpened = (cubepm->f == NULL) ? false : true;

	cubepm_open(cubepm, CUBEPM_MODE_WRITE, fileNumber);

	tmpRtn = local_actOnSingleFile(cubepm, pSkip, pWrite, data);

	if (!wasOpened)
		cubepm_close(cubepm);

	return tmpRtn;
}

extern uint64_t
cubepm_write(cubepm_t cubepm, uint64_t pSkip, uint64_t pWrite, stai_t *data)
{
	assert(cubepm != NULL);
	assert(pSkip + pWrite <= cubepm_calcTotalNumParts(cubepm));
	assert(data != NULL);

	return local_actOnFileSet(cubepm, pSkip, pWrite, data,
	                          CUBEPM_MODE_WRITE);
}

extern void
cubepm_prettyPrint(const cubepm_t cubepm, const char *prefix, FILE *f)
{
	assert(f != NULL);

	const char *actPref = (prefix == NULL) ? "" : prefix;

	fprintf(f, "%s\n%sGeneral Information:\n", actPref, actPref);
	fprintf(f, "%s  Number of files         :  %i\n",
	        actPref, cubepm->numFiles);
	fprintf(f, "%s  File Endianess          :  %s\n",
	        actPref, endian_toString(cubepm->fileEndianess));
	fprintf(f, "%s  Last opened             :  %i\n",
	        actPref, cubepm->lastOpened);
	fprintf(f, "%s  Offset of last file     :  (%g,%g,%g)\n",
	        actPref, cubepm->fileCoordOffset[0],
	        cubepm->fileCoordOffset[1], cubepm->fileCoordOffset[2]);
	fprintf(f, "%s  Total numer of particles:  %" PRIu64 "\n",
	        actPref, cubepm_calcTotalNumParts(cubepm));

	fprintf(f, "%s\n%sGeneral Header Data:\n", actPref, actPref);
	fprintf(f, "%s  a             :  %g (z = %g)\n", actPref,
	        cubepm->a, 1. / (cubepm->a) - 1.);
	fprintf(f, "%s  t             :  %g\n", actPref, cubepm->t);
	fprintf(f, "%s  tau           :  %g\n", actPref, cubepm->tau);
	fprintf(f, "%s  nts           :  %i\n", actPref, cubepm->nts);
	fprintf(f, "%s  dt_f_acc      :  %g\n", actPref, cubepm->dt_f_acc);
	fprintf(f, "%s  dt_pp_acc     :  %g\n", actPref, cubepm->dt_pp_acc);
	fprintf(f, "%s  dt_c_acc      :  %g\n", actPref, cubepm->dt_c_acc);
	fprintf(f, "%s  cur_checkpoint:  %i\n", actPref, cubepm->cur_checkpoint);
	fprintf(f, "%s  cur_projection:  %i\n", actPref, cubepm->cur_projection);
	fprintf(f, "%s  cur_halofind  :  %i\n", actPref, cubepm->cur_halofind);
	fprintf(f, "%s  mass_p        :  %g\n", actPref, cubepm->mass_p);
	fprintf(f, "%s  omega0        :  %g\n", actPref, cubepm->omega0);
	fprintf(f, "%s  lambda0       :  %g\n", actPref, cubepm->lambda0);
	fprintf(f, "%s  boxsize       :  %g Mpc/h\n", actPref, cubepm->boxsize);
	fprintf(f, "%s  nodesDim      :  %i\n", actPref, cubepm->nodesDim);
	fprintf(f, "%s  ngrid         :  %i\n", actPref, cubepm->ngrid);

	fprintf(f, "%s\n%sConversion Factors:\n", actPref, actPref);
	fprintf(f, "%s  position to Mpc/h           :  %g\n", actPref,
	        cubepm_calcFactorFilePositionToMpch(cubepm));
	fprintf(f, "%s  velocity to km/s            :  %g\n", actPref,
	        cubepm_calcFactorFileVelocityToKms(cubepm));
	fprintf(f, "%s  mass of particles in M_sun/h:  %g\n\n", actPref,
	        cubepm_calcFactorParticleMassInMsunh(cubepm));

	for (int i = 0; i < cubepm->numFiles; i++) {
		double offset[3];
		local_calculateFileCoordOffset(i, cubepm->nodesDim, cubepm->ngrid,
		                               offset);
		fprintf(f, "%s%s:\n", actPref, cubepm->fileNames[i]);
		fprintf(f, "%s  np_local:  %i\n", actPref, cubepm->np_local[i]);
		fprintf(f, "%s  offset  :  (%g,%g,%g)\n", actPref,
		        offset[0], offset[1], offset[2]);
	}
} /* cubepm_prettyPrint */

extern double
cubepm_calcFactorFilePositionToMpch(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->boxsize / ((double)(cubepm->ngrid));
}

extern double
cubepm_calcFactorFileVelocityToKms(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return cubepm->boxsize * 1.5 * sqrt(cubepm->omega0) * 100.
	       / ((double)(cubepm->ngrid) * cubepm->a);
}

extern double
cubepm_calcFactorParticleMassInMsunh(const cubepm_t cubepm)
{
	assert(cubepm != NULL);

	return POW3(cubepm->boxsize) / (POW3((double)(cubepm->ngrid)))
	       * cubepm->omega0 * RHO_CRIT_0 * cubepm->mass_p;
}

extern uint64_t
cubepm_calcTotalNumParts(const cubepm_t cubepm)
{
	assert(cubepm != NULL);
	uint64_t totalNumParts = UINT64_C(0);

	for (int i = 0; i < cubepm->numFiles; i++) {
		if (cubepm->np_local[i] != CUBEPM_NUMBER_NONE)
			totalNumParts += cubepm->np_local[i];
	}

	return totalNumParts;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_nullRestOfStructure(cubepm_t cubepm)
{
	assert(cubepm->numFiles > 0);

	cubepm->fileEndianess = endian_getSystemEndianess();
	cubepm->f             = NULL;
	cubepm->mode          = CUBEPM_MODE_READ;
	cubepm->lastOpened    = CUBEPM_NUMBER_NONE;
	for (int i = 0; i < 3; i++)
		cubepm->fileCoordOffset[i] = NAN;
	for (int i = 0; i < cubepm->numFiles; i++)
		cubepm->np_local[i] = CUBEPM_NUMBER_NONE;
	cubepm->a              = NAN;
	cubepm->t              = NAN;
	cubepm->tau            = NAN;
	cubepm->nts            = CUBEPM_NUMBER_NONE;
	cubepm->dt_f_acc       = NAN;
	cubepm->dt_pp_acc      = NAN;
	cubepm->dt_c_acc       = NAN;
	cubepm->cur_checkpoint = CUBEPM_NUMBER_NONE;
	cubepm->cur_projection = CUBEPM_NUMBER_NONE;
	cubepm->cur_halofind   = CUBEPM_NUMBER_NONE;
	cubepm->mass_p         = NAN;
	cubepm->omega0         = NAN;
	cubepm->lambda0        = NAN;
	cubepm->boxsize        = NAN;
}

static char **
local_constructFileNames(const char *path, const char *stem, int numFiles)
{
	char **fNames;
	char *start;
	char ending[CUBEPM_MAX_FILEDIGITS + 4 + 1]; // +4 for .dat +1 for \0

	assert(path != NULL && path[0] != '\0');
	assert(stem != NULL);
	assert(numFiles >= 0);

	fNames = xmalloc(sizeof(char *) * numFiles);

	if (path[strlen(path) - 1] != '/') {
		char *tmp = xstrmerge(path, "/");
		start = xstrmerge(tmp, stem);
		xfree(tmp);
	} else {
		start = xstrmerge(path, stem);
	}
	for (int i = 0; i < numFiles; i++) {
		sprintf(ending, "%i.dat", i);
		fNames[i] = xstrmerge(start, ending);
	}
	xfree(start);

	return fNames;
}

static void
local_byteswapHeaderValues(cubepm_t cubepm)
{
	for (int i = 0; i < cubepm->numFiles; i++)
		byteswap(cubepm->np_local + i, sizeof(int32_t));
	byteswap(&(cubepm->a), sizeof(float));
	byteswap(&(cubepm->t), sizeof(float));
	byteswap(&(cubepm->tau), sizeof(float));
	byteswap(&(cubepm->nts), sizeof(int32_t));
	byteswap(&(cubepm->dt_f_acc), sizeof(float));
	byteswap(&(cubepm->dt_pp_acc), sizeof(float));
	byteswap(&(cubepm->dt_c_acc), sizeof(float));
	byteswap(&(cubepm->cur_checkpoint), sizeof(int32_t));
	byteswap(&(cubepm->cur_projection), sizeof(int32_t));
	byteswap(&(cubepm->cur_halofind), sizeof(int32_t));
	byteswap(&(cubepm->mass_p), sizeof(float));
}

static void
local_calculateFileCoordOffset(int    fileNumber,
                               int    nodesDim,
                               int    ngrid,
                               double *offset)
{
	double delta;
	int    pos[3];
	int    nodesSqr = nodesDim * nodesDim;

	pos[2]      = fileNumber / nodesSqr;
	fileNumber -= pos[2] * nodesSqr;
	pos[1]      = fileNumber / nodesDim;
	fileNumber -= pos[1] * nodesDim;
	pos[0]      = fileNumber;

	delta       = ngrid / ((double)nodesDim);
	offset[0]   = pos[0] * delta;
	offset[1]   = pos[1] * delta;
	offset[2]   = pos[2] * delta;
}

static void
local_seekToParticle(cubepm_t cubepm, uint64_t particleNumber)
{
	assert(cubepm != NULL);
	assert(cubepm->f != NULL);

	fseek(cubepm->f,
	      CUBEPM_HEADER_SIZE + CUBEPM_PARTICLE_SIZE * particleNumber,
	      SEEK_SET);
}

static void
local_readParticle(FILE  *f,
                   float *partData,
                   bool  doByteswap)
{
	xfread(partData, sizeof(float), 6, f);

	if (doByteswap) {
		for (int i = 0; i < 6; i++)
			byteswap(partData + i, sizeof(float));
	}
}

static void
local_writeParticle(FILE        *f,
                    const float *partData,
                    bool        doByteswap)
{
	for (int i = 0; i < 6; i++) {
		if (isnan(partData[i]))
			xfseek(f, sizeof(float), SEEK_CUR);
		else {
			float val = partData[i];
			if (doByteswap)
				byteswap(&val, sizeof(float));
			xfwrite(&val, sizeof(float), 1, f);
		}
	}
}

static void
local_copyParticleDataToStais(const float *partData,
                              uint64_t    pos,
                              stai_t      *data)
{
	for (int i = 0; i < 6; i++) {
		if (data[i] != NULL) {
			if (stai_getSizeOfElementInBytes(data[i]) == sizeof(float)) {
				stai_setElement(data[i], pos, partData + i);
			} else if (stai_getSizeOfElementInBytes(data[i])
			           == sizeof(double)) {
				double d = (double)(partData[i]);
				stai_setElement(data[i], pos, &d);
			} else {
				diediedie(EXIT_FAILURE);
			}
		}
	}
}

static void
local_fillParticleDataFromStais(float *partData, uint64_t pos, stai_t *data)
{
	for (int i = 0; i < 6; i++) {
		if (data[i] != NULL) {
			if (stai_getSizeOfElementInBytes(data[i]) == sizeof(float)) {
				stai_getElement(data[i], pos, partData + i);
			} else if (stai_getSizeOfElementInBytes(data[i])
			           == sizeof(double)) {
				double d;
				stai_getElement(data[i], pos, &d);
				partData[i] = (float)d;
			}
		} else {
			partData[i] = NAN;
		}
	}
}

inline static void
local_rebaseStaisData(stai_t *data, int64_t offsetElements)
{
	if (data[0] != NULL)
		stai_rebase(data[0], offsetElements);
	if (data[1] != NULL)
		stai_rebase(data[1], offsetElements);
	if (data[2] != NULL)
		stai_rebase(data[2], offsetElements);
	if (data[3] != NULL)
		stai_rebase(data[3], offsetElements);
	if (data[4] != NULL)
		stai_rebase(data[4], offsetElements);
	if (data[5] != NULL)
		stai_rebase(data[5], offsetElements);
}

static void
local_calcFirstLastFile(const cubepm_t cubepm,
                        uint64_t       pSkip,
                        uint64_t       pAct,
                        int            *firstFile,
                        int            *lastFile)
{
	int curFile = 0;

	assert(pSkip + pAct <= cubepm_calcTotalNumParts(cubepm));

	while (pSkip > (uint64_t)cubepm_getNPLocal(cubepm, curFile)) {
		pSkip -= cubepm_getNPLocal(cubepm, curFile);
		curFile++;
	}
	*firstFile = curFile;
	if ((uint64_t)cubepm_getNPLocal(cubepm, curFile) >= pSkip + pAct) {
		*lastFile = curFile;
		return;
	} else {
		pAct -= ((uint64_t)cubepm_getNPLocal(cubepm, curFile) - pSkip);
		curFile++;
	}
	while (pAct > (uint64_t)cubepm_getNPLocal(cubepm, curFile)) {
		pAct -= cubepm_getNPLocal(cubepm, curFile);
		curFile++;
	}
	*lastFile = curFile;
}

static void
local_calcSkipActFile(const cubepm_t cubepm,
                      uint64_t       pSkip,
                      uint64_t       pAct,
                      int            fileNumber,
                      uint64_t       *pSkipFile,
                      uint64_t       *pActFile)
{
	uint64_t partsInFile = (uint64_t)cubepm_getNPLocal(cubepm, fileNumber);

	assert(pSkip <= partsInFile);

	*pSkipFile = pSkip;
	*pActFile  = partsInFile - *pSkipFile;
	*pActFile  = (*pActFile > pAct) ? pAct : *pActFile;
}

inline static void
local_applyOffsetToParticle(float *partData, const double *offset)
{
	for (int i = 0; i < 3; i++)
		partData[i] += (float)offset[i];
}

inline static void
local_unapplyOffsetToParticle(float *partData, const double *offset)
{
	for (int i = 0; i < 3; i++)
		partData[i] -= (float)offset[i];
}

inline static void
local_applyPeriodicityToParticle(float *partData, int ngrid)
{
	for (int i = 0; i < 3; i++)
		partData[i] = fmodf(partData[i] + (float)ngrid, (float)ngrid);
}

static uint64_t
local_actOnFileSet(cubepm_t     cubepm,
                   uint64_t     pSkip,
                   uint64_t     pAct,
                   stai_t       *data,
                   cubepmMode_t mode)
{
	uint64_t numPartsActTotal = UINT64_C(0);
	int      firstFile, lastFile;

	local_calcFirstLastFile(cubepm, pSkip, pAct, &firstFile, &lastFile);
	for (int i = 0; i < firstFile; i++)
		pSkip -= (uint64_t)cubepm_getNPLocal(cubepm, i);

	for (int i = firstFile; i <= lastFile; i++) {
		uint64_t pSkipFile, pActFile, actualAct;

		cubepm_open(cubepm, mode, i);
		local_calcSkipActFile(cubepm, pSkip, pAct, i,
		                      &pSkipFile, &pActFile);
		actualAct = local_actOnSingleFile(cubepm, pSkip, pAct, data);
		cubepm_close(cubepm);

		local_rebaseStaisData(data, actualAct);
		numPartsActTotal += actualAct;
		pSkip            -= pSkipFile;
		pAct             -= pActFile;
	}
	local_rebaseStaisData(data, -((int64_t)numPartsActTotal));

	return numPartsActTotal;
}

static uint64_t
local_actOnSingleFile(cubepm_t cubepm,
                      uint64_t pSkip,
                      uint64_t pAct,
                      stai_t   *data)
{
	bool  doByteswap = false;
	float partData[6];

	if (endian_getSystemEndianess() != cubepm_getFileEndianess(cubepm))
		doByteswap = true;

	local_seekToParticle(cubepm, pSkip);

	if (cubepm_getMode(cubepm) == CUBEPM_MODE_READ) {
		int ngrid = cubepm_getNGrid(cubepm);
		for (uint64_t i = UINT64_C(0); i < pAct; i++) {
			local_readParticle(cubepm->f, partData, doByteswap);
			local_applyOffsetToParticle(partData, cubepm->fileCoordOffset);
			local_applyPeriodicityToParticle(partData, ngrid);
			local_copyParticleDataToStais(partData, i, data);
		}
	} else if (cubepm_getMode(cubepm) == CUBEPM_MODE_WRITE) {
		for (uint64_t i = UINT64_C(0); i < pAct; i++) {
			local_fillParticleDataFromStais(partData, i, data);
			local_unapplyOffsetToParticle(partData, cubepm->fileCoordOffset);
			local_writeParticle(cubepm->f, partData, doByteswap);
		}
	} else {
		diediedie(EXIT_FAILURE);
	}

	return pAct;
}
