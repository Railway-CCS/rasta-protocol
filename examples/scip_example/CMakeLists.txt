# scip_example
add_executable(scip_example c/main.c)
set_target_properties(scip_example PROPERTIES ${DEFAULT_PROJECT_OPTIONS})
target_compile_options(scip_example PRIVATE ${DEFAULT_COMPILE_OPTIONS})
target_link_libraries(scip_example rasta)
