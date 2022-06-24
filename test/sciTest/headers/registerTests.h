#ifndef LST_SIMULATOR_REGISTERTESTS_H
#define LST_SIMULATOR_REGISTERTESTS_H

#include <CUnit/Basic.h>

// include tests
#include "sciTests.h"
#include "scipTests.h"
#include "scilsTests.h"
/*
 * Called by the Gradle CUnit launcher to register all CUnit tests.
 */
void gradle_cunit_register();

#endif //LST_SIMULATOR_REGISTERTESTS_H
