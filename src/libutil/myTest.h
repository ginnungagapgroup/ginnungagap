// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MYTEST_H
#define MYTEST_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/myTest.h
 * @ingroup libutilMiscMyTest
 * @brief  This file provides the code test infrastructure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Callback types ----------------------------------------------------*/

/**
 * @brief  The signature of the function that is implementing the actual
 *         test.
 *
 * The function is required to take no arguments and simply return a
 * boolean value indicating the success or failure of the implemented
 * test.
 */
typedef bool (*myTest_testFunc_t)(void);


/*--- Exported defines --------------------------------------------------*/

/**
 * @brief  A utility macro that evaluates the result of myTest_runtest()
 *         taking into account previous executions.
 *
 * The main thing here is that provided boolean variable can only be
 * changed to true (indicating a failure), i.e. if the value of the
 * provided boolean variable was false, it can be changed to true, but
 * if it was true it will stay on true.
 *
 * This is done to keep track of the global status of the tests,  We
 * don't want to stop after the first test that failed but keep running
 * the tests, but in the end the test suite failed if just one single
 * test failed.
 *
 * @param  func
 *           The function to pass as an argument to myTest_runtest().
 * @param  hasFailed
 *           The name of a boolean variable that will be assigned the
 *           result.
 */
#define MYTEST_RUN(func, hasFailed) \
    if (!(myTest_runtest(func))) {  \
		hasFailed = true;           \
	} else {                        \
		if (!hasFailed)             \
			hasFailed = false;      \
	}


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  This function runs the test functions and evaluates the
 *         return (globally, if MPI is used).
 *
 * This basically only wraps around the actual function that implements
 * the test (passed as the argument).  It will however also verify that
 * the test function suceeded on all MPI tasks, should MPI be used.
 *
 * Additionally, it will announce failure of success on stdout (only for
 * the first task in the MPI case).
 *
 * @param[in]  functionToTest
 *               This is the actual function that implements the test.
 *
 * @return  Returns true if (on all tasks) the test function returned
 *          true.  If it failed (on at least on task), false is
 *          returned.
 */
extern bool
myTest_runtest(myTest_testFunc_t functionToTest);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilMiscMyTest A Test Framework
 * @ingroup libutilMisc
 * @brief Provides a simple test framework.
 *
 */

#endif
