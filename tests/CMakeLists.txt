include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(CUnit IMPORTED_TARGET cunit)
endif(PKG_CONFIG_FOUND)

add_executable(rastaTest
    rasta/headers/blake2test.h
    rasta/headers/configtest.h
    rasta/headers/dictionarytest.h
    rasta/headers/fifotest.h
    rasta/headers/rastacrcTest.h
    rasta/headers/rastadeferqueueTest.h
    rasta/headers/rastafactoryTest.h
    rasta/headers/rastalisttest.h
    rasta/headers/rastamd4Test.h
    rasta/headers/rastamoduleTest.h
    rasta/headers/siphash24test.h
    rasta/c/blake2test.c
    rasta/c/configtest.c
    rasta/c/dictionarytest.c
    rasta/c/fifotest.c
    rasta/c/rastacrcTest.c
    rasta/c/rastadeferqueueTest.c
    rasta/c/rastafactoryTest.c
    rasta/c/rastalisttest.c
    rasta/c/rastamd4Test.c
    rasta/c/rastamoduleTest.c
    rasta/c/registerTests.c
    rasta/c/siphash24test.c)
target_include_directories(rastaTest PRIVATE rasta/headers)
target_link_libraries(rastaTest rasta PkgConfig::CUnit)

add_test(NAME test_rastaTest
         COMMAND rastaTest)

add_executable(sciTest
    sci/headers/scilsTests.h
    sci/headers/scipTests.h
    sci/headers/sciTests.h
    sci/c/registerTests.c
    sci/c/scilsTests.c
    sci/c/scipTests.c
    sci/c/sciTests.c)
target_include_directories(sciTest PRIVATE sci/headers)
target_link_libraries(sciTest rasta PkgConfig::CUnit)

add_test(NAME test_sciTest
         COMMAND sciTest)
