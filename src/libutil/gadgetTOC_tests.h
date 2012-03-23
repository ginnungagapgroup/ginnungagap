// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETTOC_TESTS_H
#define GADGETTOC_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetTOC_tests.h
 * @ingroup  libutilFilesGadgetTOCTests
 * @brief  Provides the interface to the Gadget TOC tests.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/** @brief  Tests gadgetTOC_(). */
extern bool
gadgetTOC_new_test(void);

/** @brief  Tests gadgetTOC_newFromFile(). */
extern bool
gadgetTOC_newFromFile_test(void);

/** @brief  Tests gadgetTOC_del(). */
extern bool
gadgetTOC_del_test(void);

/** @brief  Tests gadgetTOC_setFileVersion(). */
extern bool
gadgetTOC_setFileVersion_test(void);

/** @brief  Tests gadgetTOC_getFileVersion(). */
extern bool
gadgetTOC_getFileVersion_test(void);

/** @brief  Tests gadgetTOC_blockExists(). */
extern bool
gadgetTOC_blockExists_test(void);

/** @brief  Tests gadgetTOC_blockExistsByName(). */
extern bool
gadgetTOC_blockExistsByName_test(void);

/** @brief  Tests gadgetTOC_getOffsetForBlock(). */
extern bool
gadgetTOC_getOffsetForBlock_test(void);

/** @brief  Tests gadgetTOC_getSizeInBytesForBlock(). */
extern bool
gadgetTOC_getSizeInBytesForBlock_test(void);

/** @brief  Tests gadgetTOC_getNameInV2FilesForBlock(). */
extern bool
gadgetTOC_getNameInV2FilesForBlock_test(void);

/** @brief  Tests gadgetTOC_getOffsetBySeqNumber(). */
extern bool
gadgetTOC_getOffsetBySeqNumber_test(void);

/** @brief  Tests gadgetTOC_getTypeBySeqNumber(). */
extern bool
gadgetTOC_getTypeBySeqNumber_test(void);

/** @brief  Tests gadgetTOC_getSizeInBytesBySeqNumber(). */
extern bool
gadgetTOC_getSizeInBytesBySeqNumber_test(void);

/** @brief  Tests gadgetTOC_getNameBySeqNumber(). */
extern bool
gadgetTOC_getNameBySeqNumber_test(void);

/** @brief  Tests gadgetTOC_getTotalFileSize(). */
extern bool
gadgetTOC_getTotalFileSize_test(void);

/** @brief  Tests gadgetTOC_addEntryByName(). */
extern bool
gadgetTOC_addEntryByName_test(void);

/** @brief  Tests gadgetTOC_addEntryByType(). */
extern bool
gadgetTOC_addEntryByType_test(void);

/** @brief  Tests gadgetTOC_calcSizes(). */
extern bool
gadgetTOC_calcSizes_test(void);

/** @brief  gadgetTOC_calcOffset(). */
extern bool
gadgetTOC_calcOffset_test(void);

/** @brief  Tests gadgeTOC_seekToData(). */
extern bool
gadgetTOC_seekToData_test(void);

/** @brief  Tests gadgeTOC_seekToDescriptor(). */
extern bool
gadgetTOC_seekToDescriptor_test(void);

/** @brief  Tests gadgetTOC_isValid(). */
extern bool
gadgetTOC_isValid_test(void);


/*--- Doxygen group definition ------------------------------------------*/

/**
 * @defgroup libutilFilesGadgetTOCTests Tests
 * @ingroup libutilFilesGadgetTOC
 * @brief Provides tests for @ref libutilFilesGadgetTOC
 */

#endif
