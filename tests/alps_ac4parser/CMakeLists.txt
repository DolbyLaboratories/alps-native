add_executable(alps_ac4parser_tests alps_ac4parser_tests.c)

target_link_options(alps_ac4parser_tests PRIVATE
    -Wl,--wrap=BinarySource_create
    # -Wl,--wrap=bam_binary_sink_create
    -Wl,--wrap=F_raw_ac4_frame_toc_only_create
    -Wl,--wrap=F_raw_ac4_frame_toc_only_parse
)

target_link_libraries(alps_ac4parser_tests
    project_compiler_options
    cmocka::cmocka
    alps_ac4parser
    bam_ac4parser
)

add_test(alps_ac4parser alps_ac4parser_tests)