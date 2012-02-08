// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRAFIC_H
#define GRAFIC_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/grafic.h
 * @ingroup libutilFilesGrafic
 * @brief  This file provides the interface of the Grafic file type.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include <stdbool.h>


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  Gives the precision of the Grafic file. */
typedef enum {
	/** @brief  The file is in single precision. */
	GRAFIC_FORMAT_FLOAT,
	/** @brief  The file is in double precision. */
	GRAFIC_FORMAT_DOUBLE
} graficFormat_t;


/*--- ADT handle --------------------------------------------------------*/

/** Defines the handle for the grafic structure. */
typedef struct grafic_struct *grafic_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new Grafic object.
 *
 * @return  Returns a new (uninitialized) Grafic object.
 */
extern grafic_t
grafic_new(void);


/**
 * @brief  Creates a new Grafic object from a Grafic file.
 *
 * @return  Returns a new Grafic object describing the underlying file.
 */
extern grafic_t
grafic_newFromFile(const char *fileName);


/**
 * @brief  Deletes a Grafic file object and frees the associated memory.
 *
 * @param[in,out]  *grafic
 *                     A pointer to the external variable holding the
 *                     reference to the Grafic object.  After deletion, the
 *                     external variable will be set to @c NULL.  Passing
 *                     @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
grafic_del(grafic_t *grafic);


/** @} */

/**
 * @name  Getter
 *
 * @{
 */

/**
 * @brief  Retrieves the file name of the grafic file.
 *
 * @param[in]  grafic
 *                The object that should be queried, passing @c NULL is
 *                undfined.
 *
 * @return  Returns a reference to the internal variable holding the file
 *          name.  Must only be used read-only.
 */
extern const char *
grafic_getFileName(const grafic_t grafic);


/**
 * @brief  Retrieves the grid size of the Grafic file.
 *
 * @param[in]   grafic
 *                 The object that should be queried, passing @c NULL is
 *                 undfined.
 * @param[out]  *np
 *                 Array of at least three elements that will receive the
 *                 size of the Grafic file.
 *
 * @return  Returns nothing,
 */
extern void
grafic_getSize(const grafic_t grafic, uint32_t *np);


/**
 * @brief  Retrieves the cell spacing in @c Mpc.
 *
 * @param[in]  grafic
 *                The object that should be queried, passing @c NULL is
 *                undfined.
 *
 * @return  Returns the cell spacing.
 */
extern float
grafic_getDx(const grafic_t grafic);


/**
 * @brief  Retrieves the offset of the grid.
 *
 * @param[in]   grafic
 *                 The object that should be queried, passing @c NULL is
 *                 undfined.
 * @param[out]  *xoff
 *                 Array of at least three elements that will receive the
 *                 offset.
 *
 * @return  Returns nothing.
 */
extern void
grafic_getXoff(const grafic_t grafic, float *xoff);


/**
 * @brief  Retrieves the expansion of the file.
 *
 * @param[in]  grafic
 *                The object that should be queried, passing @c NULL is
 *                undfined.
 *
 * @return   Returns the expansion paramter @c a.
 */
extern float
grafic_getAstart(const grafic_t grafic);


/**
 * @brief  Retrieves the matter density.
 *
 * @param[in]  grafic
 *                The object that should be queried, passing @c NULL is
 *                undfined.
 *
 * @return  Returns the matter density in units of the critical density.
 */
extern float
grafic_getOmegam(const grafic_t grafic);


/**
 * @brief  Retrieves the dark energy density.
 *
 * @param[in]  grafic
 *                The object that should be queried, passing @c NULL is
 *                undfined.
 *
 * @return  Returns the density of the dark energy in units of the critical
 *          energy.
 */
extern float
grafic_getOmegav(const grafic_t grafic);


/**
 * @brief  Retrieves the Hubble parameter.
 *
 * @param[in]  grafic
 *                The object that should be queried, passing @c NULL is
 *                undfined.
 *
 * @return  The Hubble parameter in units of @c km/s/Mpc.
 */
extern float
grafic_getH0(const grafic_t grafic);


/**
 * @brief  Retrieves the random seed of the white noise file.
 *
 * @param[in]  grafic
 *                The object that should be queried, passing @c NULL is
 *                undfined.
 *
 * @return  Returns the random seed.
 */
extern int
grafic_getIseed(const grafic_t grafic);


/**
 * @brief  Checks if the file is a white noise file.
 *
 * @param[in]  grafic
 *                The object that should be queried, passing @c NULL is
 *                undfined.
 *
 * @return  Returns @c true if the underlying file is a white noise file and
 *          @c false otherwise.
 */
extern bool
grafic_getIsWhiteNoise(const grafic_t grafic);


/** @} */

/**
 * @name  Setter
 *
 * @{
 */

/**
 * @brief  Sets the file name of the grafic file.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      *fileName
 *                    The file name.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setFileName(grafic_t grafic, const char *fileName);


/**
 * @brief  Sets the size of the grafic file.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      *np
 *                    An array of three elements giving the extent in each
 *                    direction.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setSize(grafic_t grafic, uint32_t *np);


/**
 * @brief  Sets the cell spacing.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      dx
 *                    The cell spacing in @c Mpc.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setDx(grafic_t grafic, float dx);


/**
 * @brief  Sets the offset of the file.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      *xoff
 *                    The offset of the file.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setXoff(grafic_t grafic, float *xoff);


/**
 * @brief  Sets the expansion of this file.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      astart
 *                    The expansion parameter.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setAstart(grafic_t grafic, float astart);


/**
 * @brief  Sets matter density.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      omegam
 *                    The matter density in units of the critical density.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setOmegam(grafic_t grafic, float omegam);


/**
 * @brief  Sets the density of the dark energy.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      omegav
 *                    The densiy of the dark energy in units of the critical
 *                    density.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setOmegav(grafic_t grafic, float omegav);


/**
 * @brief  Sets the HUbble parameter.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      h0
 *                    The Hubble parameter in units of @c km/s/Mpc.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setH0(grafic_t grafic, float h0);


/**
 * @brief  Sets the random seed of the Grafic file.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      iseed
 *                    The random noise seed.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setIseed(grafic_t grafic, int iseed);


/**
 * @brief  Sets whether the file is a white noise file or not.
 *
 * @param[in,out]  grafic
 *                    The file object to work with.
 * @param[in]      isWhiteNoise
 *                    If @c true, then the file is a white noise file, if
 *                    @c false, then it is not.
 *
 * @return  Returns nothing.
 */
extern void
grafic_setIsWhiteNoise(grafic_t grafic, bool isWhiteNoise);


/** @} */

/**
 * @name  IO
 *
 * @{
 */

/**
 * @brief  Creates an empty file of the correct size.
 *
 * @param[in]  grafic
 *                The grafic object for which to create the file.
 *
 * @return  Returns nothing.
 */
extern void
grafic_makeEmptyFile(const grafic_t grafic);


/**
 * @brief  Reads the complete grafic file into a prepared array.
 *
 * The function will deal with endianess and precision adjustments.
 *
 * @param[in]   grafic
 *                 The file object to work with.
 * @param[out]  *data
 *                 The array into which to write.
 * @param[in]   dataFormat
 *                 The format of the data array.
 * @param[in]   numComponents
 *                 The number of components in the data array.
 *
 * @return  Returns nothing.
 */
extern void
grafic_read(const grafic_t grafic,
            void           *data,
            graficFormat_t dataFormat,
            int            numComponents);

/**
 * @brief  Writes the complete Grafic file using the provided data array.
 *
 * The function will deal automatically with endianess and precision
 * adjustments.
 *
 * @param[in]  grafic
 *                The file object to work with.
 * @param[in]  *data
 *                The data array from which to read.
 * @param[in]  dataFormat
 *                The format of the data array.
 * @param[in]  numComponents
 *                The number of components in the data array.
 *
 * @return  Returns nothing.
 */
extern void
grafic_write(const grafic_t grafic,
             const void     *data,
             graficFormat_t dataFormat,
             int            numComponents);

/**
 * @brief  Reads a portion of the data in the file.
 *
 * @param[in]   grafic
 *                 The file object to work with.
 * @param[out]  data
 *                 The array into which to write.
 * @param[in]   dataFormat
 *                 The format of the data array.
 * @param[in]   numComponents
 *                 The number of components in the data array.
 * @param[in]   *idxLo
 *                 The lower left corner of the window.
 * @param[in]   *dims
 *                 The size of the window.
 *
 * @return  Returns nothing.
 */
extern void
grafic_readWindowed(const grafic_t           grafic,
                    void *restrict           data,
                    graficFormat_t           dataFormat,
                    int                      numComponents,
                    const uint32_t *restrict idxLo,
                    const uint32_t *restrict dims);

/**
 * @brief  Writes a selection into the file.
 *
 * @param[in]  grafic
 *                The file object to work with.
 * @param[in]  data
 *                The array to write.
 * @param[in]  dataFormat
 *                The format of the data array.
 * @param[in]  numComponents
 *                The number of components in the data array.
 * @param[in]  *idxLo
 *                The lower left corner of the window.
 * @param[in]  *dims
 *                The size of the window.
 *
 * @return  Returns nothing.
 */
extern void
grafic_writeWindowed(const grafic_t           grafic,
                     const void *restrict     data,
                     graficFormat_t           dataFormat,
                     int                      numComponents,
                     const uint32_t *restrict idxLo,
                     const uint32_t *restrict dims);

/**
 * @brief  Reads a slab from the file.
 *
 * @param[in]   grafic
 *                 The file object to work with.
 * @param[out]  data
 *                 The array into which to write.
 * @param[in]   dataFormat
 *                 The format of the data array.
 * @param[in]   numComponents
 *                 The number of components in the data array.
 * @param[in]   slabNum
 *                 The number of the slab.
 *
 * @return  Returns nothing.
 */
extern void
grafic_readSlab(grafic_t       grafic,
                void           *data,
                graficFormat_t dataFormat,
                int            numComponents,
                int            slabNum);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilFilesGrafic Grafic
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to Grafic files.
 *
 */


#endif
