/***********************************************************************************************************************
 * Copyright (C) 2024-2025 by Dolby International AB.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 *    disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *    following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************************************************************/

/**
 * \file
 * \brief API for setting active presentation in AC-4 frames.
 */

#ifndef DLB_ALPS_NATIVE_ALPS_AC4PARSER_ALPS_AC4PARSER_H
#define DLB_ALPS_NATIVE_ALPS_AC4PARSER_ALPS_AC4PARSER_H

#include <stddef.h>

#include "dlb_alps_native/utils/types.h"

/**
 * \brief Sets active presentation in given AC-4 frame.
 *
 * Changes the value of the presentation_level field in TOC of given AC_4 frame to 7 for all presentations which ID is
 * NOT equal to given ID. Presentations with presentation_level 7 are not supported by the decoder so the only
 * supported presentation is the selected one. The change is done in-place.
 *
 * \param[in,out] frame_buf memory buffer holding the AC-4 frame
 * \param[in] size_in_bytes size in bytes of the memory buffer
 * \param[in] active_presentation_id ID of the presentation that is to be set active
 * \return \ref ALPS_RET_OK if presentation was set successfully or following errors:
 * - \ref ALPS_RET_E_INVALID_ARG if \p frame_buf or \p size_in_bytes are 0
 * - \ref ALPS_RET_E_PARSE if there are problems with parsing AC_4 TOC
 * - \ref ALPS_RET_E_PRES_ID_NOT_FOUND if the given presentation ID was not found in the TOC
 */
alps_ret alps_ac4parser_set_active_presentation_id(
    unsigned char *frame_buf,
    size_t size_in_bytes,
    int active_presentation_id);

#endif /* DLB_ALPS_NATIVE_ALPS_AC4PARSER_ALPS_AC4PARSER_H */
