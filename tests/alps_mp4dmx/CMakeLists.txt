add_executable(alps_mp4dmx_tests alps_mp4dmx_tests.c)

target_link_options(alps_mp4dmx_tests PRIVATE
    -Wl,--wrap=mp4d_demuxer_query_mem
    -Wl,--wrap=mp4d_trackreader_query_mem
    -Wl,--wrap=mp4d_demuxer_init
    -Wl,--wrap=mp4d_trackreader_init
    -Wl,--wrap=mp4d_trackreader_set_tenc
    -Wl,--wrap=mp4d_parse_atom_header
    -Wl,--wrap=mp4d_demuxer_parse
    -Wl,--wrap=mp4d_find_atom
    -Wl,--wrap=mp4d_atom_to_buffer
    -Wl,--wrap=mp4d_read_u8
    -Wl,--wrap=mp4d_read_u24
    -Wl,--wrap=mp4d_read_u32
    -Wl,--wrap=mp4d_skip_bytes
    -Wl,--wrap=mp4d_demuxer_get_movie_info
    -Wl,--wrap=mp4d_demuxer_get_stream_info
    -Wl,--wrap=mp4d_trackreader_init_segment
    -Wl,--wrap=mp4d_trackreader_next_sample
)

target_link_libraries(alps_mp4dmx_tests
    project_compiler_options
    cmocka::cmocka
    alps_mp4dmx
)

add_test(alps_mp4dmx alps_mp4dmx_tests)