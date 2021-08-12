# HowTo - CUnit tests

## Running the tests
In Order to run all registered CUnit tests you build the project using Gradle (see [HowTo Gradle](md_doc/gradle.md)).  
Note that there will only be an output about the unit tests if at least one test failed, so if the build is successful, all tests were successful too.

## Creating a new Unit test
### Creating the test code
All unit test files are placed in `src/rastaTest/`. To be more specific, the source files (.c) in the subdirectory `c/` and the header files (.h) in `headers/`. You have to create a header file although you might not need it for your unit tests. It is needed to register the tests later on.
Let's create a simple unit test. We start by creating a header file `mytest.h` in `src/rastaTest/headers/` with the following content:
```
#include <CUnit/Basic.h>

#ifndef LST_SIMULATOR_MYTESTS_H
#define LST_SIMULATOR_MYTESTS_H

void my_unit_test();

#endif //LST_SIMULATOR_MYTESTS_H
```

This defines a prototype for a test function called `my_unit_test`. Note that we included the CUnit library in the first line. `CUnit/Basic.h` contains all the basic assert functions you will need to write your actual test. The next step is to implement our prototype test-function in a C source file `src/rastaTest/c/mytest.c`:
```
#include <mytest.h>

void my_unit_test(){
    // your actual unit test goes here
    // in this case we will just use a dummy assert which is always true
    CU_ASSERT_EQUAL(1, 1);
}
```

Congratulations, your just created your unit test, but in order to run it automatically on build we need to register it.

### Registering the tests
By registering a unit test it will be added to the list of tests that are executed when the project is built using gradle.
Registering a unit test is pretty simple, just open the file `src/rastaTest/c/registerTests.c` and you'll see a lot of other unit tests that are already registered here. In order to register your own test you need to include your header file and call `CU_add_test` in the `gradle_cunit_register` function. `CU_add_test` takes 3 arguments.  
The first one is the suite to be used, `pSuiteMath` should be fine for almost all applications, but you could easily implement your own suite, see the CUnit documentation for details.  
The second parameter is the name of your test, we are using the name of the test function as the test name, but you can name it whatever you want.  
The last parameter is a function that is associated with the test. Here you have to pass your previosly created test function.  

```
#include "registerTests.h"

// INCLUDE TESTS
#include "mytest.h"
// ... a lot of other test headers and suite init

void gradle_cunit_register() {
    // ... initialization of suite and other tests
    CU_add_test(pSuiteMath, "my_unit_test", my_unit_test);
}
```
If you created more than one test function you need to call `CU_add_test` for each of those functions.  
That's it, your tests should now be included in the build process.

For further information have a look at the [CUnit documentation](http://cunit.sourceforge.net/doc/)