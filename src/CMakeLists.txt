# Target name
set(target rasta)

include_directories(
    rasta/headers
    sci/headers)

# RaSTA headers
set(RASTA_HDRS
    rasta/headers/config.h
    rasta/headers/dictionary.h
    rasta/headers/event_system.h
    rasta/headers/fifo.h
    rasta/headers/logging.h
    rasta/headers/rasta_new.h
    rasta/headers/rasta_red_multiplexer.h
    rasta/headers/rastacrc.h
    rasta/headers/rastadeferqueue.h
    rasta/headers/rastafactory.h
    rasta/headers/rastahandle.h
    rasta/headers/rastalist.h
    rasta/headers/rastamd4.h
    rasta/headers/rastamodule.h
    rasta/headers/rastaredundancy_new.h
    rasta/headers/rastautil.h
    rasta/headers/rmemory.h
    rasta/headers/udp.h
    rasta/headers/rastablake2.h
    rasta/headers/rastasiphash24.h
    rasta/headers/rastahashing.h
)

# SCI headers
set(SCI_HDRS
    sci/headers/hashmap.h
    sci/headers/sci.h
    sci/headers/sci_telegram_factory.h
    sci/headers/scils.h
    sci/headers/scils_telegram_factory.h
    sci/headers/scip.h
    sci/headers/scip_telegram_factory.h
)

# Shared object for RaSTA and SCI protocols
add_library(${target} SHARED
    # RaSTA sources
    rasta/c/config.c
    rasta/c/dictionary.c
    rasta/c/event_system.c
    rasta/c/fifo.c
    rasta/c/logging.c
    rasta/c/rasta_new.c
    rasta/c/rasta_red_multiplexer.c
    rasta/c/rastacrc.c
    rasta/c/rastadeferqueue.c
    rasta/c/rastafactory.c
    rasta/c/rastahandle.c
    rasta/c/rastalist.c
    rasta/c/rastamd4.c
    rasta/c/rastamodule.c
    rasta/c/rastaredundancy_new.c
    rasta/c/rastautil.c
    rasta/c/rmemory.c
    rasta/c/udp.c
    sci/c/hashmap.c
    rasta/c/rastablake2.c
    rasta/c/rastasiphash24.c
    rasta/c/rastahashing.c
    # SCI sources
    sci/c/sci.c
    sci/c/sci_telegram_factory.c
    sci/c/scils.c
    sci/c/scils_telegram_factory.c
    sci/c/scip.c
    sci/c/scip_telegram_factory.c
)

# Link system libraries for librasta
target_link_libraries(${target} rt pthread)

set_property(TARGET ${target}
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

#
# Project options
#

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
)

#
# Include directories
#

target_include_directories(${target}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/rasta/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/sci/headers

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rasta/headers>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sci/headers>
)

#
# Compile options
#

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

# Installation for library
install(TARGETS ${target}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
