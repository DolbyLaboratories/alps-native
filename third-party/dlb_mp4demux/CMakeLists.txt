########################################################################################################################
# Copyright (C) 2024 by Dolby International AB.
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