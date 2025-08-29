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

add_executable(dlb_alps_native_tests dlb_alps_native_tests.c)

target_link_options(dlb_alps_native_tests PRIVATE
    -Wl,--wrap=alps_mp4dmx_query_mem
    -Wl,--wrap=alps_mp4dmx_init
    -Wl,--wrap=alps_mp4dmx_destroy
    -Wl,--wrap=alps_mp4dmx_process_buffer
    -Wl,--wrap=alps_mp4dmx_get_preselections
    -Wl,--wrap=alps_mp4dmx_next_sample
    -Wl,--wrap=alps_ac4parser_set_active_presentation_id
)

get_target_property(MP4DMX_INCLUDE alps_mp4dmx INCLUDE_DIRECTORIES)

target_include_directories(dlb_alps_native_tests PRIVATE
    MP4DMX_INCLUDE
)

target_link_libraries(dlb_alps_native_tests
    project_compiler_options
    cmocka::cmocka
    dlb_alps_native
)

add_test(dlb_alps_native dlb_alps_native_tests)
