cmake_minimum_required(VERSION 3.15)

project(dlb_alps_native
        VERSION 0.1.0.0
        LANGUAGES C)

option(BUILD_SHARED_LIBS "Build all modules as shared libraries instead of static" ON)
option(BUILD_SHARED_ALPS_LIB "Build dlb_alps_native as a shared library instead of static")
option(BUILD_CLI "Build CLI" ON)
option(BUILD_UNIT_TESTS "Build unit tests" ON)
option(BUILD_DOCUMENTATION "Build documentation" ON)

if (BUILD_UNIT_TESTS)
    message("Building static libraries so the mocking using --wrap linker option works")
    set(BUILD_SHARED_LIBS OFF)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(project_compiler_options INTERFACE)
target_compile_options(project_compiler_options INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "'git submodule update --init --recursive' failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()

add_subdirectory(src)
add_subdirectory(third-party)

if (BUILD_CLI)
    add_subdirectory(cli)
endif()

if (BUILD_UNIT_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

if (BUILD_DOCUMENTATION)
    add_subdirectory(docs)
endif()
