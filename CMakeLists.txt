cmake_minimum_required(VERSION 3.12)
project(rasta
    LANGUAGES C
    DESCRIPTION "A C implementation of the RaSTA protocol stack")

set(CMAKE_C_STANDARD 11)

SET(USE_OPENSSL false CACHE BOOL "Use OpenSSL MD4 implementation")

option(BUILD_DOCUMENTATION "Generate documentation" OFF)
option(BUILD_LOCAL_EXAMPLES "Build the RaSTA/SCI examples the run on localhost" ON)
option(BUILD_REMOTE_EXAMPLES "Build the RaSTA/SCI examples that communicate in a network" ON)
option(EXAMPLE_IP_OVERRIDE "Use IPs from environment variables in RaSTA/SCI examples" OFF)
option(ENABLE_CODE_COVERAGE "Provide command to generate code coverage report" OFF)
option(ENABLE_STATIC_ANALYSIS "Run cppcheck along with the compiler" OFF)

if(ENABLE_STATIC_ANALYSIS)
    set(CMAKE_C_CPPCHECK "cppcheck" "--enable=performance,information")
endif(ENABLE_STATIC_ANALYSIS)

set(VALGRIND_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all")

include(CTest)
include(GNUInstallDirs)

add_compile_options(-Wall)

if(ENABLE_CODE_COVERAGE)
    link_libraries(-lgcov)
    add_compile_options(-fprofile-arcs -ftest-coverage)

    find_program(LCOV lcov)
    find_program(GENHTML genhtml)
    if(EXISTS ${LCOV} AND EXISTS ${GENHTML})
        add_custom_target(coverage_report
            COMMAND "${LCOV}" "--base-directory" "${CMAKE_SOURCE_DIR}"
                              "--no-external"
                              "--directory" "."
                              "--capture"
                              "--output-file" "app.info"
            COMMAND "${GENHTML}" "app.info" "-o" "ccHTML"
            COMMENT "Generate HTML files from preprocessed code coverage data"
            VERBATIM)
    endif()
endif(ENABLE_CODE_COVERAGE)

include_directories(
        src/rasta/headers
        src/sci/headers)

set(EXECUTABLE_OUTPUT_PATH bin/exe/examples)
set(LIBRARY_OUTPUT_PATH bin/lib)

# RaSTA headers
set(RASTA_HDRS
        src/rasta/headers/config.h
        src/rasta/headers/dictionary.h
        src/rasta/headers/fifo.h
        src/rasta/headers/logging.h
        src/rasta/headers/rasta_new.h
        src/rasta/headers/rasta_red_multiplexer.h
        src/rasta/headers/rastacrc.h
        src/rasta/headers/rastadeferqueue.h
        src/rasta/headers/rastafactory.h
        src/rasta/headers/rastahandle.h
        src/rasta/headers/rastalist.h
        src/rasta/headers/rastamd4.h
        src/rasta/headers/rastamodule.h
        src/rasta/headers/rastaredundancy_new.h
        src/rasta/headers/rastautil.h
        src/rasta/headers/rmemory.h
        src/rasta/headers/udp.h
        src/rasta/headers/rastablake2.h
        src/rasta/headers/rastasiphash24.h
        src/rasta/headers/rastahashing.h
    )

# SCI headers
set(SCI_HDRS
        src/sci/headers/hashmap.h
        src/sci/headers/sci.h
        src/sci/headers/sci_telegram_factory.h
        src/sci/headers/scils.h
        src/sci/headers/scils_telegram_factory.h
        src/sci/headers/scip.h
        src/sci/headers/scip_telegram_factory.h
    )

# Shared object for RaSTA and SCI protocols
add_library(rasta SHARED
        # RaSTA sources
        src/rasta/c/config.c
        src/rasta/c/dictionary.c
        src/rasta/c/fifo.c
        src/rasta/c/logging.c
        src/rasta/c/rasta_new.c
        src/rasta/c/rasta_red_multiplexer.c
        src/rasta/c/rastacrc.c
        src/rasta/c/rastadeferqueue.c
        src/rasta/c/rastafactory.c
        src/rasta/c/rastahandle.c
        src/rasta/c/rastalist.c
        src/rasta/c/rastamd4.c
        src/rasta/c/rastamodule.c
        src/rasta/c/rastaredundancy_new.c
        src/rasta/c/rastautil.c
        src/rasta/c/rmemory.c
        src/rasta/c/udp.c
        src/sci/c/hashmap.c
        src/rasta/c/rastablake2.c
        src/rasta/c/rastasiphash24.c
        src/rasta/c/rastahashing.c
        # SCI sources
        src/sci/c/sci.c
        src/sci/c/sci_telegram_factory.c
        src/sci/c/scils.c
        src/sci/c/scils_telegram_factory.c
        src/sci/c/scip.c
        src/sci/c/scip_telegram_factory.c
        )
# Link system libraries for librasta
target_link_libraries(rasta rt pthread)

set_property(
    TARGET rasta
    PROPERTY PUBLIC_HEADER
        ${RASTA_HDRS}
        ${SCI_HDRS})

# if USE_OPENSSL parameter is passed to cmake or if architecture is ARM -> use openssl md4 implementation
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${USE_OPENSSL})
    message("Using OpenSSL MD4 implementation (only standard IV)")

    # define flag to use openssl in rastamd4
    target_compile_definitions(rasta PUBLIC USE_OPENSSL)
    # link libcrypto
    target_link_libraries(rasta crypto)
else()
    message("Using rasta-c MD4 implementation")
endif()

# Installation for library
install(TARGETS rasta
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Enable optimization for RaSTA
target_compile_options(rasta PRIVATE -O3)

if(BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED dot)

    set(DOXYGEN_PROJECT_NAME "Librasta & Libsci")
    set(DOXYGEN_OUTPUT_DIRECTORY "doc")
    set(DOXYGEN_CREATE_SUBDIRS YES)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_EXTRACT_ANON_NSPACES YES)
    set(DOXYGEN_CASE_SENSE_NAMES NO)
    set(DOXYGEN_WARN_NO_PARAMDOC YES)
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
    set(DOXYGEN_INLINE_SOURCES YES)
    set(DOXYGEN_GENERATE_MAN YES)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_HIDE_UNDOC_RELATIONS NO)
    if(DOXYGEN_FOUND)
        doxygen_add_docs(
            documentation
            src/rasta/headers
            src/sci/headers
            README.md
            md_doc/
            ALL
            COMMENT "Generate documentation")
    endif(DOXYGEN_FOUND)
endif(BUILD_DOCUMENTATION)

if(BUILD_TESTING)
    include(FindPkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(CUnit IMPORTED_TARGET cunit)
    endif(PKG_CONFIG_FOUND)

    add_executable(rastaTest
        src/rastaTest/headers/blake2test.h
        src/rastaTest/headers/configtest.h
        src/rastaTest/headers/dictionarytest.h
        src/rastaTest/headers/fifotest.h
        src/rastaTest/headers/rastacrcTest.h
        src/rastaTest/headers/rastadeferqueueTest.h
        src/rastaTest/headers/rastafactoryTest.h
        src/rastaTest/headers/rastalisttest.h
        src/rastaTest/headers/rastamd4Test.h
        src/rastaTest/headers/rastamoduleTest.h
        src/rastaTest/headers/siphash24test.h
        src/rastaTest/c/blake2test.c
        src/rastaTest/c/configtest.c
        src/rastaTest/c/dictionarytest.c
        src/rastaTest/c/fifotest.c
        src/rastaTest/c/rastacrcTest.c
        src/rastaTest/c/rastadeferqueueTest.c
        src/rastaTest/c/rastafactoryTest.c
        src/rastaTest/c/rastalisttest.c
        src/rastaTest/c/rastamd4Test.c
        src/rastaTest/c/rastamoduleTest.c
        src/rastaTest/c/registerTests.c
        src/rastaTest/c/siphash24test.c)
    target_include_directories(rastaTest PRIVATE src/rastaTest/headers)
    target_link_libraries(rastaTest rasta PkgConfig::CUnit)

    add_test(NAME test_rastaTest
             COMMAND rastaTest)

    add_executable(sciTest
        src/sciTest/headers/scilsTests.h
        src/sciTest/headers/scipTests.h
        src/sciTest/headers/sciTests.h
        src/sciTest/c/registerTests.c
        src/sciTest/c/scilsTests.c
        src/sciTest/c/scipTests.c
        src/sciTest/c/sciTests.c)
    target_include_directories(sciTest PRIVATE src/sciTest/headers)
    target_link_libraries(sciTest rasta PkgConfig::CUnit)

    add_test(NAME test_sciTest
             COMMAND sciTest)
endif(BUILD_TESTING)

if(BUILD_LOCAL_EXAMPLES)
	# scip_example on localhost
	add_executable(scip_example_local
		    src/examples_localhost/c/scip.c)
	target_link_libraries(scip_example_local rasta)


	# scils_example on localhost
	add_executable(scils_example_local
		    src/examples_localhost/c/scils.c)
	target_link_libraries(scils_example_local rasta)

	# rasta_example_new on localhost
	add_executable(rasta_example_local
		    src/examples_localhost/c/rasta.c)
	target_link_libraries(rasta_example_local rasta)
	
	# Copy RaSTA configs to build directory
	configure_file(config/rasta_server_local.cfg rasta_server_local.cfg COPYONLY)
	configure_file(config/rasta_client1_local.cfg rasta_client1_local.cfg COPYONLY)
	configure_file(config/rasta_client2_local.cfg rasta_client2_local.cfg COPYONLY)
endif(BUILD_LOCAL_EXAMPLES)

if(BUILD_REMOTE_EXAMPLES)
	# scip_example
	add_executable(scip_example
		    src/scip_example/c/main.c)
	target_link_libraries(scip_example rasta)


	# scils_example
	add_executable(scils_example
		    src/scils_example/c/main.c)
	target_link_libraries(scils_example rasta)

	# rasta_example_new
	add_executable(rasta_example
		    src/rasta_example_new/c/main.c)
	target_link_libraries(rasta_example rasta)
	
	# Copy RaSTA configs to build directory
	configure_file(config/rasta_server.cfg rasta_server.cfg COPYONLY)
	configure_file(config/rasta_client1.cfg rasta_client1.cfg COPYONLY)
	configure_file(config/rasta_client2.cfg rasta_client2.cfg COPYONLY)

	if (EXAMPLE_IP_OVERRIDE)
		add_compile_definitions(EXAMPLE_IP_OVERRIDE)
	endif(EXAMPLE_IP_OVERRIDE)
endif(BUILD_REMOTE_EXAMPLES)

