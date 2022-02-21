# scils_example
add_executable(scils_example c/main.c)
set_target_properties(scils_example PROPERTIES ${DEFAULT_PROJECT_OPTIONS})
target_compile_options(scils_example PRIVATE ${DEFAULT_COMPILE_OPTIONS})
target_link_libraries(scils_example rasta)
