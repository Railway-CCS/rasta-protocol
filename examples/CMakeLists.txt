if(BUILD_LOCAL_EXAMPLES)
    # scip_example on localhost
    add_executable(scip_example_local
            localhost/c/scip.c)
    target_link_libraries(scip_example_local rasta)

    # scils_example on localhost
    add_executable(scils_example_local
            localhost/c/scils.c)
    target_link_libraries(scils_example_local rasta)

    # rasta_example_new on localhost
    add_executable(rasta_example_local
            localhost/c/rasta.c)
    target_link_libraries(rasta_example_local rasta)

    # Copy RaSTA configs to build directory
    configure_file(../config/rasta_server_local.cfg ../rasta_server_local.cfg COPYONLY)
    configure_file(../config/rasta_client1_local.cfg ../rasta_client1_local.cfg COPYONLY)
    configure_file(../config/rasta_client2_local.cfg ../rasta_client2_local.cfg COPYONLY)
endif(BUILD_LOCAL_EXAMPLES)

if(BUILD_REMOTE_EXAMPLES)
    # scip_example
    add_executable(scip_example
            scip/c/main.c)
    target_link_libraries(scip_example rasta)

    # scils_example
    add_executable(scils_example
            scils/c/main.c)
    target_link_libraries(scils_example rasta)

    # rasta_example_new
    add_executable(rasta_example
            rasta/c/main.c)
    target_link_libraries(rasta_example rasta)

    # Copy RaSTA configs to build directory
    configure_file(../config/rasta_server.cfg ../rasta_server.cfg COPYONLY)
    configure_file(../config/rasta_client1.cfg ../rasta_client1.cfg COPYONLY)
    configure_file(../config/rasta_client2.cfg ../rasta_client2.cfg COPYONLY)

    if (EXAMPLE_IP_OVERRIDE)
        add_compile_definitions(EXAMPLE_IP_OVERRIDE)
    endif(EXAMPLE_IP_OVERRIDE)
endif(BUILD_REMOTE_EXAMPLES)
