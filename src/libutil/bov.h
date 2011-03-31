// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BOV_H
#define BOV_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/bov.h
 * @ingroup libutilFilesBov
 * @brief  This file provides the interface of the BOV file type.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include "endian.h"


/*--- Typedefs ----------------------------------------------------------*/

/**
 * @brief Gives the type of the data stored.
 */
typedef enum {
	/** Corresponds to bytes (equivalent to char).  */
	BOV_FORMAT_BYTE,
	/** Corresponds to int. */
	BOV_FORMAT_INT,
	/** Corresponds to float. */
	BOV_FORMAT_FLOAT,
	/** Corresponds to double. */
	BOV_FORMAT_DOUBLE
} bovFormat_t;

/**
 * @brief Indicator for the centering of the data values.
 */
typedef enum {
	/** Data is centered with a zone. */
	BOV_CENTERING_ZONAL,
	/** Data is centered at the nodes. */
	BOV_CENTERING_NODAL
} bovCentering_t;


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a BOV object.
 */
typedef struct bov_struct *bov_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new empty BOV handle.
 *
 * @return  Returns a handle to an empty BOV object.
 */
extern bov_t
bov_new(void);


/**
 * @brief  Creates and initializes a BOV object by reading a .bov file.
 *
 * @param[in]  *fileName
 *                The name of the BOV file to open and read.
 *
 * @return  Returns a handle to a BOV object holding the information of
 *          the bov.
 */
extern bov_t
bov_newFromFile(const char *fileName);


/**
 * @brief  Deletes a BOV objects and frees all associated memory.
 *
 * @param[in,out]  *bov
 *                    A pointer to the variable holding the handle of
 *                    the BOV object that is to be deleted.  After
 *                    deletion the external variable is set to point to
 *                    NULL, removing access to the now invalid memory
 *                    region.
 */
extern void
bov_del(bov_t *bov);


/** @} */

/**
 * @name Getter
 *
 * @{
 */

/**
 * @brief  Retrieves the time value of the file.
 *
 * @param[in]  bov
 *                The object to query.
 *
 * @return  Returns the time.
 */
extern double
bov_getTime(const bov_t bov);

/**
 * @brief  Retrieves the full path and name of the data file the BOV
 *         describes.
 *
 * If the file name stored in the BOV with an absolute path, then just
 * the entry of the data_file field in the .bov is returned.  Otherwise
 * the path of the .bov is prepended.  This works under the assumption
 * that the data file the bov is describing, is to be searched relative
 * to where the .bov is.  I.e. if the data_file is
 * <tt>data/large.dat</tt> and the bov is opened as
 * <tt>../output/large.bov</tt>, then the string returned by this
 * function will be <tt>'../output/data/large.dat'</tt>.
 *
 * @param[in]  bov
 *                The object to query.
 *
 * @return  Returns a NULL terminated string holding the full path and
 *          name of the data files.  The calling function is responsible
 *          for freeing the memory of this string.
 */
extern char *
bov_getDataFileName(const bov_t bov);

/**
 * @brief  Retrieves the size of the data.
 *
 * @param[in]   bov
 *                 The object to query.
 * @param[out] *dataSize
 *                 This is a pointer to an array of at least 3 values
 *                 which will receive the size of the data.
 *
 * @return  Returns nothing, the data transfer is handle via the
 *          function parameter.
 */
extern void
bov_getDataSize(const bov_t bov, uint32_t *dataSize);

/**
 * @brief  Queries the format of the data.
 *
 * @param[in]  bov
 *                The object to query.
 *
 * @return  Returns data format.
 */
extern bovFormat_t
bov_getDataFormat(const bov_t bov);

/**
 * @brief  Retrieves the name of the variable in the file.
 *
 * @param[in]  bov
 *                The object to query.
 *
 * @return  Returns a NULL terminated string holding the name of the
 *          variable.  The calling function is responsible to free the
 *          string after usage.
 */
extern char *
bov_getVarName(const bov_t bov);

/**
 * @brief  Retrieves endianess of the data file.
 *
 * @param[in]  bov
 *                The object to query.
 *
 * @return  Returns the endianess.
 *
 * @sa libutil/endian.h
 */
extern endian_t
bov_getDataEndian(const bov_t bov);

/**
 * @brief  Retrieves the centering of the data.
 *
 * @param[in]  bov
 *                The object to query.
 *
 * @return  Returns the centering.
 */
extern bovCentering_t
bov_getCentering(const bov_t bov);

/**
 * @brief  Retrieves the origin of the data block.
 *
 * @param[in]   bov
 *                 The object to query.
 * @param[out]  *brickOrigin
 *                 Pointer to an array of at least 3 double values that
 *                 will receive the origin of the data.
 *
 * @return  Returns nothing.  The data transfer is handle via the
 *          function parameter.
 */
extern void
bov_getBrickOrigin(const bov_t bov, double *brickOrigin);

/**
 * @brief  Retrieves the extend of the data.
 *
 * @param[in]   bov
 *                 The object to query.
 * @param[out]  *brickSize
 *                 Pointer to an array of at least 3 double values that
 *                 will receive the extend of the data.
 *
 * @return  Returns nothing.  The data transfer is handle via the
 *          function parameter.
 */
extern void
bov_getBrickSize(const bov_t bov, double *brickSize);

/**
 * @brief  Retrieves the number of data components.
 *
 * This can be used to identify vector data, a value of 3 would indicate
 * that 3 values are stored for each grid point.
 *
 * @param[in]  bov
 *                The object to query.
 *
 * @return  Returns the number of components.
 */
extern int
bov_getDataComponents(const bov_t bov);


/** @} */

/**
 * @name Setter
 *
 * @{
 */

/**
 * @brief  Sets the time value of the file.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      time
 *                    The time to set.
 *
 * @return  Returns nothing
 */
extern void
bov_setTime(bov_t bov, const double time);

/**
 * @brief  Sets the name of the data file.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      *dataFileName
 *                    A NULL terminated string that contains the name of
 *                    the data file.  The functions makes a private copy
 *                    of the string.
 *
 * @return  Returns nothing.
 */
extern void
bov_setDataFileName(bov_t bov, const char *dataFileName);

/**
 * @brief  Sets the data size.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      *dataSize
 *                    A pointer to an array contain at least 3 values
 *                    which are stored as the data size.  Note that if
 *                    you want to store data with less than 3
 *                    dimensions, you need to give the unneeded ones a
 *                    size of 1.
 *
 * @return  Returns nothing.
 */
extern void
bov_setDataSize(bov_t bov, const uint32_t *dataSize);

/**
 * @brief  Sets the data format
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      format
 *                    The format of the data.
 *
 * @return  Returns nothing.
 */
extern void
bov_setDataFormat(bov_t bov, const bovFormat_t format);

/**
 * @brief  Sets the variable name.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      *varName
 *                    A NULL terminated string holding the name of the
 *                    variable.  The function will make a private copy
 *                    of the string.
 *
 * @return  Returns nothing.
 */
extern void
bov_setVarName(bov_t bov, const char *varName);

/**
 * @brief  Sets the data endianess.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      endian
 *                    The endianess of the data.
 *
 * @return  Returns nothing.
 *
 * @sa  libutil/endian.h
 */
extern void
bov_setDataEndian(bov_t bov, const endian_t endian);

/**
 * @brief  Sets the centering of the data.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      centering
 *                    The centering of the data.
 *
 * @return  Returns nothing.
 */
extern void
bov_setCentering(bov_t bov, const bovCentering_t centering);

/**
 * @brief  Sets the origin of the data block.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      *brickOrigin
 *                    A pointer to an array of at least 3 values holding
 *                    the origin of the data.
 *
 * @return  Returns nothing.
 */
extern void
bov_setBrickOrigin(bov_t bov, const double *brickOrigin);

/**
 * @brief  Sets the extent of the data block.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      *brickSize
 *                    A pointer to an array of at least 3 values holding
 *                    the extent of the data.
 *
 * @return  Returns nothing.
 */
extern void
bov_setBrickSize(bov_t bov, const double *brickSize);

/**
 * @brief  Sets the number of components of the data.
 *
 * @param[in,out]  bov
 *                    The object to update.
 * @param[in]      numComponents
 *                    The number of components of the data.
 *
 * @return  Returns nothing.
 */
extern void
bov_setDataComponents(bov_t bov, const int numComponents);


/** @} */


/**
 * @name Reading
 *
 * @{
 */

/**
 * @brief  Reads the full data block described by the bov.
 *
 * This will open the data file and read the full block into an array
 * provided.  Type conversion and endianess adaption will be performed.
 * The whole file is always read, if the file format is identical to the
 * in-memory format, that it will be read directly into this memory
 * location.  However, if the formats differ, then the whole file will
 * be read into a temporary buffer and then copied over to the real data
 * array.  Note that 'format' includes not only the data type but also
 * the number of components.
 *
 * @param[in]      bov
 *                    The object from which to read.
 * @param[in,out]  *data
 *                    The array into which to read.
 * @param[in]      dataFormat
 *                    The format of the data array.
 * @param[in]      numComponents
 *                    The number of components the data array supports.
 *
 * @return  Returns nothing.
 */
extern void
bov_read(bov_t       bov,
         void        *data,
         bovFormat_t dataFormat,
         int         numComponents);

/**
 * @brief  Reads a windowed sample of the data block described by the
 *         bov.
 *
 * This is used to read only a (rectangular) subsection of the full
 * data.  Type conversion and endianess conversion is performed
 * automatically.
 *
 * @param[in]      bov
 *                    The object from which to read.
 * @param[in,out]  *data
 *                    The array into which to read.
 * @param[in]      dataFormat
 *                    The format of the data array.
 * @param[in]      numComponents
 *                    The number of components the data array supports.
 * @param[in]      *idxLo
 *                    An array of at least 3 values giving the
 *                    coordinates of he lower corner of the window.
 * @param[in]      *dims
 *                    An array of at least 3 values giving the size of
 *                    the window.
 *
 * @return  Returns nothing.
 */
extern void
bov_readWindowed(bov_t       bov,
                 void        *data,
                 bovFormat_t dataFormat,
                 int         numComponents,
                 uint32_t    *idxLo,
                 uint32_t    *dims);


/** @} */

/**
 * @name Writing
 *
 * @{
 */

/**
 * @brief  Checks a bov object if it valid for writing.
 *
 * For a bov object to be valid for writing, the required values must
 * have been set, i.e. the data size, the variable name and the data
 * file name must be set.
 *
 * @param[in]  bov
 *                The BOV object to be queried.
 *
 * @return  Returns whether or not the object is valid for writing.
 */
extern bool
bov_isValidForWrite(const bov_t bov);

/**
 * @brief  Write a bov to a file.
 *
 * The BOV must be valid for writing, otherwise the function will
 * abort the program.  Being valid is not sufficient though, also the
 * name of the file to which to write must either already have been set
 * (in which case the according function parameter may be NULL) or a
 * filename needs to be provided.
 *
 * Note that this function only writes the BOV it will <b>not</b> write
 * the according data file.
 *
 * @param[in,out]  bov
 *                    The BOV object that should be written.
 * @param[in]      *bovFileName
 *                    A NULL terminated string that holds the name of
 *                    the file the bov should be written to.  
 */
extern void
bov_write(bov_t bov, const char *bovFileName);


/** @} */


#endif
