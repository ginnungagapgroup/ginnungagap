// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef ART_H
#define ART_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/art.h
 * @ingroup libutilFilesART
 * @brief  This file provides the interface to ART files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "artHeader.h"
#include <stdint.h>
#include <stdio.h>
#include "stai.h"


/*--- Exported defines --------------------------------------------------*/

/**
 * @brief  Used to indicate to art_new() that the default path prefix
 *         should be used.
 *
 * @sa #ART_DEFAULT_PATH_STRING
 */
#define ART_USE_DEFAULT_PATH NULL

/**
 * @brief  Used to indicate to art_new() that the default file suffix
 *         should be used.
 *
 * @sa #ART_DEFAULT_SUFFIX_STRING
 */
#define ART_USE_DEFAULT_SUFFIX NULL


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  Gives the mode with which the files are opened. */
typedef enum {
	/**
	 * @brief  This is for writing, corresponds to "ab".
	 *
	 * This should be used to append to an existing file.  The file will
	 * be created if it does not exist.
	 */
	ART_MODE_WRITE,
	/** @brief  This is for writing, corresponds to "rb".  */
	ART_MODE_READ
} artMode_t;


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for an ART object.
 */
typedef struct art_struct *art_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new ART file object.
 *
 * ART files are described by two different file types: the PMcrd.DAT
 * file that contains the header information, and the PMcrsX.DAT file(s)
 * (with X=0..numFiles) that contain the actual particle data.  This
 * function assumes that the files have names according to the following
 * scheme:  '@<Path@>PMcr[d,s%i]@<Suffix@>'.
 *
 * @param[in]  *pathToFiles
 *                The path (absolute or relative) to the directory in
 *                which the ART files are stored.  This must be either a
 *                properly terminated string or #ART_USE_DEFAULT_PATH
 *                which assumes that the files are local.  The path
 *                should have a trailing '/', though if it does not, one
 *                is appended.  Note that passing #ART_USE_DEFAULT_PATH
 *                is equivalent to passing @c NULL, but you should be
 *                using the former to make the code more readable.
 * @param[in]  *fileNameSuffix
 *                The suffix of the files.  This should include
 *                everything after the PMcrd or PMcrsX and be a properly
 *                terminated string.  Passing @c NULL assumes that the
 *                suffix is '.DAT', but you should be using
 *                #ART_USE_DEFAULT_SUFFIX instead of @c NULL to make the
 *                code more readable.
 * @param[in]  numFiles
 *                This gives the number of files the PMcrs-files are
 *                split over.  This must be a non-zero positive integer
 *                less than #ART_MAX_NUMFILES.
 *
 * @return  Returns a properly set up ART file object.
 */
extern art_t
art_new(const char *pathToFiles, const char *fileNameSuffix, int numFiles);


/**
 * @brief  Deletes an ART file object.
 *
 * If there is still a file opened, it will be closed before deletion of
 * the object.
 *
 * @param[in,out]  *art
 *                    Pointer to an external variable holding the
 *                    reference to the ART object.  After deletion the
 *                    external variable will be set to @c NULL.
 *
 * @return  Returns nothing.
 */
extern void
art_del(art_t *art);

/** @} */

/**
 * @name Setter
 *
 * @{
 */

/**
 * @brief  Sets whether nrowc should be limited in calculating the page
 *         size.
 *
 * This will trigger a re-calculation of the page size and number of
 * particle and all these things if a header is already available.  If
 * no header is available, the numbers are not touched, but the
 * truncation mode is taken into account once the numbers are calculated
 * (by adding a header).
 *
 * @param[in,out]  art
 *                    The ART object to set the truncation for.  Passing
 *                    @c NULL is undefined.
 * @param[in]      value
 *                    If this is @c true, then @c nrowc will be limited
 *                    to #ART_MAX_NROWC in the calculation of the page
 *                    size, otherwise the @c nrowc value from the file
 *                    is used.
 *
 * @return  Returns nothing.
 */
extern void
art_setTruncateNrowc(art_t art, bool value);

/** @} */

/**
 * @name Getter
 *
 * @{
 */

/**
 * @brief  Returns the number of data files in this set.
 *
 * @param[in]  art
 *                The ART file object to query.
 *
 * @return  Returns the number of data files (PMcrsX.DAT) in this ART
 *          file object.
 */
extern int
art_getNumFiles(const art_t art);

/**
 * @brief  Retrieves the trunctation mode of @c nrwoc.
 *
 * @param[in]  art
 *                The ART file object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns @c true if @c nrowc should be truncated in the
 *          calculation of the page size and @c false, if the value from
 *          the header should be used as-is.
 */
extern bool
art_getTruncateNrowc(const art_t art);

/**
 * @brief  Gets the name of the header file associated with this file.
 *
 * @param[in]  art
 *                The ART file object to query.
 *
 * @return  Returns the name of the header file associated with this
 *          file.  The memory region is private.
 */
extern const char *
art_getHeaderFileName(const art_t art);

/**
 * @brief  Retrieves the name of a given data file.
 *
 * @param[in]  art
 *                The ART file object to query.
 * @param[in]  numFile
 *                The number of file whose name should be retrieved.
 *
 * @return  Returns the name of the requested data file.  The string
 *          points to a private memory region and must only be used
 *          read-only.
 */
extern const char *
art_getDataFileName(const art_t art, int numFile);

/**
 * @brief  Retrieves a handle to the header associated with the file.
 *
 * @param[in]  art
 *                The ART file object to query.
 *
 * @return  Returns a handle of the header object associated with this
 *          file.  This is the private handle of the file object.
 */
extern artHeader_t
art_getHeaderHandle(const art_t art);


/** @} */

/**
 * @name Using
 *
 * @{
 */


/**
 * @brief  Attaches an ART header object to an ART file object.
 *
 * If there already was an header object, then it will be replaced by
 * the new one.
 *
 * @param[in,out]  art
 *                    The ART file object that should receive the
 *                    header.  Passing @c NULL is undefined.
 * @param[in]      header
 *                    The new header.  The calling function is not
 *                    responsible for this object anymore.  Deallocation
 *                    will be done by the ART object.  Passing @c NULL
 *                    is undefined.
 *
 * @return  Returns nothing.
 */
extern void
art_attachHeader(art_t art, artHeader_t header);


/**
 * @brief  Reads a header from the file set.
 *
 * This is a convenience wrapper around art_attachHeader().
 *
 * @param[in,out]  art
 *                    The ART file object for which to read the header
 *                    from the file.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
art_attachHeaderFromFile(art_t art);

/**
 * @brief  Prints a quick summary of the file to a stream.
 *
 * @param[in]   art
 *                 The ART file object to pretty print.  Passing @c NULL
 *                 is undefined.
 * @param[in]   *prefix
 *                 A string that should appear at the beginning of each
 *                 output line.  This must be a proper string or @c NULL
 *                 which is treated as an empty string.
 * @param[out]  *f
 *                 The stream to pretty print to.  This must be a valid
 *                 stream for writing.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
art_prettyPrint(const art_t art, const char *prefix, FILE *f);


/** @} */

/**
 * @name Dealing with Files
 *
 * @{
 */

/**
 * @brief  This will fopen() the according PMcrsX.DAT file.
 *
 * If a different file, or the same with a different mode, is already
 * opened, then this will close that one and open the requested one
 * instead.  If the same file is opened again, with the same mode, then
 * the file is simply rewind and the file pointer positioned at the
 * beginning.  Repeated calls are thus perfectly legal.
 *
 * When in writing mode, the user @b must ensure that only task is
 * opening a given file, i.e. it is erroneous to open the same file
 * (within the set) more than once.  In other words: at most
 * art_struct::numFiles may be opened for writing and the access to a
 * single file must be exclusive.
 *
 * @param[in,out]  art
 *                    The ART file object to use to open a PMcrsX.DAT
 *                    file.  Passing @c NULL is undefined.
 * @param[in]      mode
 *                    The mode with which to open the file.
 * @param[in]      numFile
 *                    The number of the file to open.
 *
 * @return  Returns nothing.
 */
extern void
art_open(art_t art, artMode_t mode, int numFile);


/**
 * @brief  This will create an empty file with the right size in which
 *         all seeks are guaranteed to succeed.
 *
 * @param[in]  art
 *                The ART object.  This is required to obtain the
 *                correct file name and the correct size of the file.
 *                Passing @c NULL is undefined.
 * @param[in]  numFile
 *                The file that should be created.  This must be a valid
 *                file number of the ART object, otherwise the behaviour
 *                is undefined.
 *
 * @return  Returns nothing.
 */
extern void
art_createEmptyFile(const art_t art, int numFile);


/**
 * @brief  Will close the currently opened PMcrsX.DAT file.
 *
 * If no file is currently opened, this function will do nothing.
 *
 * @param[in,out]  art
 *                    The ART file object to use to close the file.
 *                    Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
art_close(art_t art);


/** @} */

/**
 * @name Output
 *
 * Note that no unit conversion whatsoever is done, the data arrays have
 * to be in the correct units for an ART file.  The only things that are
 * done to the data are down-casting to float precision and/or endianess
 * conversion (both only if required):  ART files always store the data
 * values in single precision and the file endianess is given by the
 * endianess of the header.
 *
 * The write functions have the capability to only write a small
 * fraction of the file (and also only for a subset of components),
 * however, for this to work, the file @b must already contain the whole
 * page on disk, otherwise the seeking in the file will fail (see
 * art_createEmptyFile()).
 *
 * @{
 */

/**
 * @brief  Writes a page to the file.
 *
 * After the completion of the function, the file pointer will be
 * positioned at the end of the page that has just been written.
 *
 * @param[in,out]  art
 *                    The ART file object.  It is required that the file
 *                    is opened for writing.  Passing @c NULL is
 *                    undefined.
 * @param[in]      pageNumber
 *                    The page number that should be written.
 * @param[in]      pSkip
 *                    The number of particles to skip at the beginning
 *                    of the page.  This must be less or equal to the
 *                    number of particles in the page.
 * @param[in]      pWrite
 *                    The number of particles to write.  The sum of
 *                    @c pWrite and @c pSkip must be less or equal to
 *                    the number of particles in the page.
 * @param[in]      *data
 *                    The must be an array of at least 6 stais (it may
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
 *                    be written to the file and to be skipped over.
 *
 * @returns  Returns the number of particles written to the file.  This
 *           should be equal to @c pWrite.
 *
 * @sa  art_open(), art_createEmptyFile(), art_close(), art_write(),
 *      art_readFromPage()
 */
extern uint64_t
art_writeToPage(art_t    art,
                int      pageNumber,
                uint64_t pSkip,
                uint64_t pWrite,
                stai_t   *data);


/**
 * @brief  Writes a set of particles to a file.
 *
 * @param[in,out]  art
 *                    The ART file object.  If the file is not opened,
 *                    or not opened in the correct mode, it will be
 *                    opened.  If any file (not necessarily this one),
 *                    then the file that is written to will not be
 *                    closed before returning.  Passing @c NULL is
 *                    undefined.
 * @param[in]      fileNumber
 *                    The number of the file to open.  This must be
 *                    valid file number.
 * @param[in]      pSkip
 *                    The number of particles to skip over before
 *                    starting to write.  This must be less or equal to
 *                    the number of particles in the file.
 * @param[in]      pWrite
 *                    The number of particles to write.  The sum of
 *                    @c pSkip and @c pWrite must be less or equal to
 *                    the number of particles in the file.
 * @param[in]      *data
 *                    See art_writeToPage() for a description of this
 *                    parameter.
 *
 * @return  Returns the number of particles written.  This should be
 *          equal to @c pWrite.
 *
 * @sa  art_open(), art_createEmptyFile(), art_close(),
 *      art_readFromFile()
 */
extern uint64_t
art_writeToFile(art_t    art,
                int      fileNumber,
                uint64_t pSkip,
                uint64_t pWrite,
                stai_t   *data);


/**
 * @brief  Writes a set of particles to a file set.
 *
 * @param[in,out]  art
 *                    The ART file object that should be used for
 *                    writing.  It is required that a header is
 *                    attached.  Passing @c NULL is undefined.
 * @param[in]      pSkip
 *                    The number of particles to skip.
 * @param[in]      pWrite
 *                    The number of particles to write.
 * @param[in]      *data
 *                    See art_writeToPage() for a description of this
 *                    parameter.
 *
 * @return  Returns the number of particles written.  This should be
 *          equal to @c pWrite.
 *
 * @sa  art_createEmptyFile(), art_read()
 */
extern uint64_t
art_write(art_t    art,
          uint64_t pSkip,
          uint64_t pWrite,
          stai_t   *data);


/** @} */

/**
 * @name Input
 *
 * Note that no unit conversion whatsoever is done, the data arrays will
 * be filled with the values in the file.  The only things that are done
 * to the data are casting to agree with the in-application storage
 * model and endianess correction to the system endianess.
 *
 * @{
 */


/**
 * @brief  Reads a page from the file.
 *
 * This will read a subset of particles in a page for the currently
 * opened ART file.  As such, it is the inverse of
 * art_writeToPage().  It is required that a file has already been
 * opened for reading with art_open().
 *
 * @param[in,out]  art
 *                    The ART file to read from.  The file is required
 *                    to be opened for reading (it may be positioned
 *                    anywhere in the file though).  Passing @c NULL is
 *                    undefined.
 * @param[in]      pageNumber
 *                    The page number to read.  This must be less or
 *                    equal to the total number of pages in the file.
 * @param[in]      pSkip
 *                    The number of particles to skip over at the
 *                    beginning of the page.  This must be less or equal
 *                    to the number of particles in the page.
 * @param[in]      pRead
 *                    The number of particles to read from the file.
 *                    The sum of @c pRead and @c pSkip must be less or
 *                    equal to the total number of particles in the
 *                    file.
 * @param[in,out]  *data
 *                    An array of at least 6 stais that describe the
 *                    arrays where the particle data should be written
 *                    to.  See art_writeToPage() for a more detailed
 *                    description of this array.  Any of the data arrays
 *                    may be @c NULL, indicating that the according data
 *                    should not be read.
 *
 * @return  Return the number of bytes read from the file.  This should
 *          be equal to @c pRead.
 *
 * @sa  art_open(), art_close(), art_WriteToPage().
 */
extern uint64_t
art_readFromPage(art_t    art,
                 int      pageNumber,
                 uint64_t pSkip,
                 uint64_t pRead,
                 stai_t   *data);


/**
 * @brief  Reads a subset of particles from a specific data file.
 *
 * This is the inverse of art_writeToFile().
 *
 * @param[in,out]  art
 *                    The ART object from which to read.  If any file
 *                    was already opened, it will be ensured that the
 *                    correct one is opened and will be left opened when
 *                    returning.  If no file was opened the function
 *                    will also return the ART object with all files
 *                    closed.  Passing @c NULL is undefined.
 * @param[in]      fileNumber
 *                    The file number from which to read.
 * @param[in]      pSkip
 *                    The number of particles to skip.
 * @param[in]      pRead
 *                    The number of particles to read.
 * @param[in,out]  *data
 *                    See art_readFromPage() for more details on the
 *                    parameter.
 *
 * @return  Returns the number of particles read.  This should be equal
 *          to @c pRead.
 *
 * @sa  art_open(), art_close(), art_write(), art_writeToFile(),
 *      art_WriteToPage(), art_read(), art_readFromPage()
 */
extern uint64_t
art_readFromFile(art_t    art,
                 int      fileNumber,
                 uint64_t pSkip,
                 uint64_t pRead,
                 stai_t   *data);


/**
 * @brief  Read a subset of particles from the whole file set.
 *
 * This is the inverse pf art_write().
 *
 * @param[in,out]  art
 *                    The ART file object from which to read.
 * @param[in]      pSkip
 *                    The number of particles to skip.
 * @param[in]      pRead
 *                    The number of particles to read.
 * @param[in,out]  *data
 *                    See art_readFromPage() for more details on the
 *                    parameter.
 *
 * @return  Returns the number of particles read.  This should be equal
 *          to  @c pRead.
 *
 * @sa  art_open(), art_close(), art_write(), art_writeToFile(),
 *      art_WriteToPage(), art_readFromFile(), art_readFromPage()
 */
extern uint64_t
art_read(art_t    art,
         uint64_t pSkip,
         uint64_t pRead,
         stai_t   *data);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilFilesART ART
 * @ingroup libutilFiles
 * @brief Provides low-level interfaces to ART files.
 *
 * The IO functionality is split in three groups: functions to open and
 * close file (art_open() and art_close()), functions to write data
 * (art_writeToPage(), art_writeToFile(), and art_write()), and
 * functions to read data (art_readFromPage(), art_readFromFile(), and
 * art_read()).
 *
 * For each write function a corresponding read function exists.  The
 * read/write functions are acting on three different layers within
 * the ART file set.  The three layers are along the line of the logical
 * structure of ART files:  A single file consists of a collection of
 * pages, and a file set consists of a collection of files.
 *
 * The highest level is the whole file set, the collection of multiple
 * data files.  This is characterised by the number of files and the
 * total number of particles.  On this level, art_write() (art_read())
 * provides a way to write (read) subset of particles to (from) the
 * file-set.  Those functions will transparently access different files
 * and different page within those to work.  They will also deal with
 * opening and closing files, so no calls to art_open() or art_close()
 * are needed if only these functions are used.
 *
 * The next lower level is the file level, this is characterised by the
 * number of particles and the number of pages in the file.  Here
 * operations are performed on one single file of the whole file set
 * forming the ART object and art_writeToFile() (art_readFromFile()) are
 * used to write (read) data to (from) a specific file.  Generally, the
 * file to work with is specified by its file number, however, it is
 * possible to prevent the functions from closing files, so that
 * multiple calls working on the same file do not entail multiple
 * closings and re-openings of the file.
 *
 * On the lowest level, a single page within a file, art_writeToPage()
 * (art_readFromPage()) are used to write (read) data into (from) a page
 * within a specific file.  Those functions require the correct file to
 * be opened (with the correct mode), it is, however, irrelevant where
 * the files are positioned.
 *
 * When reading are writing to/from files, the smallest object that is
 * use is a page (actually a subset of a page) and this smallest object
 * forms the smallest allowed actual IO operation.  This entails, that,
 * if the external memory is not in the on-disk format of ART files, a
 * buffer needs to be allocated.  In the worst case, this will be of the
 * size of one sixth of page (each page contains the information for six
 * components and the smallest IO is actually on a subset of one
 * component), which is 4MB (as the largest allowed page contains one
 * million particles).  If only a small fraction of a page is
 * read/written, the buffer is accordingly smaller.
 *
 */


#endif
