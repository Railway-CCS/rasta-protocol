cmake_minimum_required(VERSION 3.12)
project(rasta
    LANGUAGES C
    DESCRIPTION "A C implementation of the RaSTA protocol stack")

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

#
# Compiler settings and options
#

include(cmake/CompileOptions.cmake)

set(EXECUTABLE_OUTPUT_PATH bin/exe/examples)
set(LIBRARY_OUTPUT_PATH bin/lib)

add_subdirectory(src)

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
    add_subdirectory(test)
endif(BUILD_TESTING)

add_subdirectory(examples)
