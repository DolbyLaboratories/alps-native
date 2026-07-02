########################################################################################################################
# Copyright (C) 2024-2026 by Dolby International AB.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
# following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
#    disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
#    products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
########################################################################################################################

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
