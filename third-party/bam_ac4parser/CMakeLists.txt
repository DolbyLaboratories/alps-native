file(GLOB BAM_AC4PARSER_SOURCES src/*.c)

add_library(bam_ac4parser
    ${BAM_AC4PARSER_SOURCES}
)

target_compile_options(bam_ac4parser PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused-parameter -Wno-return-type -Wno-sign-compare -Werror>
)

target_compile_definitions(bam_ac4parser PUBLIC
    SYMBOL_PREFIX=dlb_alps_native
)

target_include_directories(bam_ac4parser
    PUBLIC src
)

target_link_libraries(bam_ac4parser
    m
)