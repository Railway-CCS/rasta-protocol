# rasta_example_new
add_executable(rasta_example c/main.c)
set_target_properties(rasta_example PROPERTIES ${DEFAULT_PROJECT_OPTIONS})
target_compile_options(rasta_example PRIVATE ${DEFAULT_COMPILE_OPTIONS})
target_link_libraries(rasta_example rasta)
