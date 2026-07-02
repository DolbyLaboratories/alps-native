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
    -Wl,--wrap=mp4d_read_u16
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
