/***********************************************************************************************************************
 * Copyright (C) 2024-2026 by Dolby International AB.
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
 * \brief API of the binary sink used in parsing the AC-4 TOC.
 *
 * The sink creates a list of presentations present in its frame buffer and allows to set "presentation_level" field
 * for them.<br>
 * Before the presentations can be accessed they need to be parsed by the BAM-generated AC-4 TOC parser. Create an
 * instance of the sink with \ref bam_binary_sink_create with the same buffer as given to the source and pass it to
 * \ref F_raw_ac4_frame_toc_only_parse.
 */

#ifndef BAM_BINARY_SINK_H
#define BAM_BINARY_SINK_H

#include "bam.h"

#include "dlb_alps_native/utils/types.h"

typedef struct bam_binary_sink_t bam_binary_sink;

/**
 * \brief Allocates and initializes a \ref bam_binary_sink instance for the given buffer.
 * \param[in] buffer memory buffer that holds AC_4 frame
 * \param[in] size_in_bytes size of the frame buffer in bytes
 * \return \ref NULL if \p buffer or \p size_in_bytes are zero or the allocation fails, otherwise a pointer of
 * \ref BAMSink type to the created \ref bam_binary_sink
 */
BAMSink *bam_binary_sink_create(
    unsigned char *buffer,
    size_t size_in_bytes);

/**
 * \brief Returns a pointer and size of the presentations list from the parsed AC-4 TOC.
 * \param[in] sink the instance of the sink
 * \param[out] presentations pointer to the array of presentations
 * \param[out] presentation_count number of the presentations in the returned list
 * \return \ref ALPS_RET_E_INVALID_ARG if any of the arguments is 0, \ref ALPS_RET_OK otherwise
 */
alps_ret bam_binary_sink_get_presentations(
    bam_binary_sink *sink,
    alps_presentation **presentations,
    size_t *presentation_count
);

/**
 * \brief Sets the "presentation_level" TOC field for the given presentation in the frame buffer of the sink.
 * \param[in] sink the instance of the sink
 * \param[out] presentation_index index of the presentation to be updated in the list returned by \ref bam_binary_sink_get_presentations
 * \param[out] presentation_level value to be set for the "presentation" level TOC field
 * \return \ref ALPS_RET_E_INVALID_ARG if:
 * - \p sink is 0,
 * - \p presentation_index is greater or equal than number of presentations in the list returned by \ref bam_binary_sink_get_presentations,
 * - \p presentation_level is greater than 7,
 * .
 * \ref ALPS_RET_OK otherwise
 */
alps_ret bam_binary_sink_set_presentation_level(
    bam_binary_sink *sink,
    size_t presentation_index,
    unsigned int presentation_level
);

#endif /* BAM_BINARY_SINK_H */
