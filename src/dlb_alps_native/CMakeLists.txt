# generate version header
find_package(Git)
add_custom_target(alps_native_version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
                     -D DST=${CMAKE_CURRENT_BINARY_DIR}/src/version.h
                     -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
                     -D GIT_ROOT=${CMAKE_SOURCE_DIR}
                     -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateVersionHeader.cmake
)

if (BUILD_SHARED_ALPS_LIB OR (NOT DEFINED BUILD_SHARED_ALPS_LIB AND BUILD_SHARED_LIBS))
    set(ALPS_LIB_TYPE SHARED)
else ()
    set(ALPS_LIB_TYPE STATIC)
endif ()

add_library(dlb_alps_native ${ALPS_LIB_TYPE}
    src/dlb_alps_native.c
)

target_include_directories(dlb_alps_native
    PUBLIC include
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
)

target_link_libraries(dlb_alps_native
    project_compiler_options
    utils
    alps_mp4dmx
    alps_ac4parser
)

add_dependencies(dlb_alps_native
    alps_native_version
)