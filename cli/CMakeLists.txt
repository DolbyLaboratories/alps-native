add_executable(alps main.c)

target_link_libraries(alps
    project_compiler_options
    dlb_alps_native
)