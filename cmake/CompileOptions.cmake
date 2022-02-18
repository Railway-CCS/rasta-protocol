#
# Project options
#

set(DEFAULT_PROJECT_OPTIONS
    C_STANDARD                11
    LINKER_LANGUAGE           "C"
    POSITION_INDEPENDENT_CODE ON
)


#
# Include directories
#

set(DEFAULT_INCLUDE_DIRECTORIES)


#
# Libraries
#

set(DEFAULT_LIBRARIES)


#
# Compile definitions
#

set(DEFAULT_COMPILE_DEFINITIONS)


#
# Compile options
#

set(DEFAULT_COMPILE_OPTIONS)

# GCC and Clang compiler options
if ("${CMAKE_C_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        PRIVATE
            -Wall
            -Wextra
            -Wunused
            -Werror

            -Wignored-qualifiers
            -Wmissing-braces
            -Wreturn-type
            -Wswitch
            -Wswitch-default
            -Wuninitialized
            -Wmissing-field-initializers

            $<$<C_COMPILER_ID:GNU>:
                -Wmaybe-uninitialized

                $<$<VERSION_GREATER:$<C_COMPILER_VERSION>,4.8>:
                    -Wpedantic

                    -Wreturn-local-addr
                >
            >

            $<$<C_COMPILER_ID:Clang>:
                -Wpedantic

                # -Wreturn-stack-address # gives false positives
            >
    )
endif ()


#
# Linker options
#

set(DEFAULT_LINKER_OPTIONS)
