cmake_minimum_required(VERSION 3.6.2)
project(rasta C)

set(CMAKE_C_STANDARD 11)

SET(USE_OPENSSL false CACHE BOOL "Use OpenSSL MD4 implementation")

option(BUILD_LOCAL_EXAMPLES "Build the RaSTA/SCI examples the run on localhost" ON)
option(BUILD_REMOTE_EXAMPLES "Build the RaSTA/SCI examples that communicate in a network" ON)
option(EXAMPLE_IP_OVERRIDE "Use IPs from environment variables in RaSTA/SCI examples" OFF)

include(GNUInstallDirs)

include_directories(
        src/rasta/headers
        src/sci/headers)

set(EXECUTABLE_OUTPUT_PATH bin/exe/examples)
set(LIBRARY_OUTPUT_PATH bin/lib)

# Shared object for RaSTA and SCI protocols
add_library(rasta SHARED
        # RaSTA headers
        src/rasta/headers/config.h
        src/rasta/headers/dictionary.h
        src/rasta/headers/event_system.h
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
        # SCI headers
        src/sci/headers/hashmap.h
        src/sci/headers/sci.h
        src/sci/headers/sci_telegram_factory.h
        src/sci/headers/scils.h
        src/sci/headers/scils_telegram_factory.h
        src/sci/headers/scip.h
        src/sci/headers/scip_telegram_factory.h
        # RaSTA sources
        src/rasta/c/config.c
        src/rasta/c/dictionary.c
        src/rasta/c/event_system.c
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
target_link_libraries(rasta rt)

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

	add_executable(event_system_example_local
                src/examples_localhost/c/event_test.c)
        target_link_libraries(event_system_example_local rasta)
	
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

