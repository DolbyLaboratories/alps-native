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
 * \brief Implementation of the API for setting active presentation in AC-4 frames.
 */

#include "dlb_alps_native/alps_ac4parser/alps_ac4parser.h"

#include "bam_binary_source.h"
#include "bam_binary_sink.h"
#include "raw_ac4_frame_toc_only.h"

#include "dlb_alps_native/utils/utils.h"

#define PRESENTATION_LEVEL_DISABLED (7)

alps_ret alps_ac4parser_set_active_presentation_id(
    unsigned char *frame_buf,
    size_t size_in_bytes,
    int active_presentation_id)
{
    alps_ret ret = ALPS_RET_OK;
    BAMSource *src = NULL;
    BAMSink *sink = NULL;
    BAMSyntax *syntax =  NULL;
    alps_presentation *presentations = NULL;
    size_t presentations_count = 0;
    size_t i;
    int active_presentation_found = 0;

    CHECK_EXPR_AND_BAIL(!frame_buf || !size_in_bytes , ALPS_RET_E_INVALID_ARG);
    /* if active_presentation_id is invalid just don't do anything */
    CHECK_EXPR_AND_BAIL(active_presentation_id == ALPS_INVALID_PRES_ID, ALPS_RET_OK);

    src = BinarySource_create(frame_buf, size_in_bytes);
    sink = bam_binary_sink_create(frame_buf, size_in_bytes);
    syntax =  F_raw_ac4_frame_toc_only_create();

    CHECK_EXPR_AND_BAIL(BAM_OK != F_raw_ac4_frame_toc_only_parse(syntax, src, sink), ALPS_RET_E_PARSE);

    CHECK_RET_AND_BAIL(bam_binary_sink_get_presentations((bam_binary_sink*)sink, &presentations, &presentations_count));

    /* check if given active_presentation_id can be found in the presentations list */
    active_presentation_found = 0;
    for (i = 0; i < presentations_count; i++)
    {
        if (presentations[i].id == active_presentation_id)
        {
            active_presentation_found = 1;
        }
    }

    /* don't change the buffer if presentation with given id is not present in the AC-4 TOC */
    CHECK_EXPR_AND_BAIL(!active_presentation_found, ALPS_RET_E_PRES_ID_NOT_FOUND);

    for (i = 0; i < presentations_count; i++)
    {
        if (presentations[i].id != active_presentation_id)
        {
            CHECK_RET_AND_BAIL(bam_binary_sink_set_presentation_level((bam_binary_sink*)sink, i, PRESENTATION_LEVEL_DISABLED));
        }
    }

bail:
    if (src)
    {
        src->destroy(src);
    }

    if (sink)
    {
        sink->destroy(sink);
    }

    if (syntax)
    {
        syntax->destroy(syntax);
    }

    return ret;
}
