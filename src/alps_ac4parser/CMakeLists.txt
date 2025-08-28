add_library(alps_ac4parser
    src/alps_ac4parser.c
    src/bam_binary_sink.c
)

target_compile_options(alps_ac4parser PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused-parameter -Werror>
)

target_include_directories(alps_ac4parser
    PUBLIC include
    PRIVATE src
)

target_link_libraries(alps_ac4parser
    utils
    bam_ac4parser
)