add_library(alps_mp4dmx
    src/alps_mp4dmx.c
)

target_include_directories(alps_mp4dmx
    PUBLIC include)

target_link_libraries(alps_mp4dmx
    project_compiler_options
    utils
    dlb_mp4demux
)