
include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(CUnit IMPORTED_TARGET cunit)
endif(PKG_CONFIG_FOUND)

add_executable(rastaTest
    rastaTest/headers/blake2test.h
    rastaTest/headers/configtest.h
    rastaTest/headers/dictionarytest.h
    rastaTest/headers/fifotest.h
    rastaTest/headers/rastacrcTest.h
    rastaTest/headers/rastadeferqueueTest.h
    rastaTest/headers/rastafactoryTest.h
    rastaTest/headers/rastalisttest.h
    rastaTest/headers/rastamd4Test.h
    rastaTest/headers/rastamoduleTest.h
    rastaTest/headers/registerTests.h
    rastaTest/headers/siphash24test.h
    rastaTest/c/blake2test.c
    rastaTest/c/configtest.c
    rastaTest/c/dictionarytest.c
    rastaTest/c/fifotest.c
    rastaTest/c/rastacrcTest.c
    rastaTest/c/rastadeferqueueTest.c
    rastaTest/c/rastafactoryTest.c
    rastaTest/c/rastalisttest.c
    rastaTest/c/rastamd4Test.c
    rastaTest/c/rastamoduleTest.c
    rastaTest/c/registerTests.c
    rastaTest/c/registerTests.c
    rastaTest/c/siphash24test.c)
target_include_directories(rastaTest PRIVATE rastaTest/headers)
target_link_libraries(rastaTest rasta PkgConfig::CUnit)
target_compile_definitions(rastaTest PRIVATE WITH_CMAKE)

add_test(NAME test_rastaTest
         COMMAND rastaTest)

add_executable(sciTest
    sciTest/headers/registerTests.h
    sciTest/headers/scilsTests.h
    sciTest/headers/scipTests.h
    sciTest/headers/sciTests.h
    sciTest/c/registerTests.c
    sciTest/c/scilsTests.c
    sciTest/c/scipTests.c
    sciTest/c/sciTests.c)
target_include_directories(sciTest PRIVATE sciTest/headers)
target_link_libraries(sciTest rasta PkgConfig::CUnit)
target_compile_definitions(sciTest PRIVATE WITH_CMAKE)

add_test(NAME test_sciTest
         COMMAND sciTest)
