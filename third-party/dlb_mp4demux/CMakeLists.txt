set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/repo)

add_library(dlb_mp4demux
    ${SOURCE_DIR}/src/mp4d_nav.c
    ${SOURCE_DIR}/src/mp4d_trackreader.c
    ${SOURCE_DIR}/src/mp4d_demux.c
    ${SOURCE_DIR}/src/mp4d_box_read.c
    ${SOURCE_DIR}/src/mp4d_buffer.c
    ${SOURCE_DIR}/src/player.c
    ${SOURCE_DIR}/src/es_sink.c
    ${SOURCE_DIR}/src/stream.c
    ${SOURCE_DIR}/src/file_movie.c
    ${SOURCE_DIR}/src/md_sink.c
    ${SOURCE_DIR}/src/fragment_stream.c
    ${SOURCE_DIR}/src/file_stream.c
    ${SOURCE_DIR}/src/util.c
)

target_compile_options(dlb_mp4demux PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wno-unused-parameter -Wno-format-extra-args>
)

target_compile_definitions(dlb_mp4demux PRIVATE
    EZXML_NOMMAP=X
    _FILE_OFFSET_BITS=64
)

target_include_directories(dlb_mp4demux
    PUBLIC ${SOURCE_DIR}/include
)