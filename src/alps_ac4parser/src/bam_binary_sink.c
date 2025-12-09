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
 * \brief Implementation of binary sink used in parsing the AC-4 TOC.
 */

#include "bam_binary_sink.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "dlb_bitbuf.h"
#include "dlb_bitbuf_write.h"

#include "dlb_alps_native/utils/utils.h"

/**
 * \brief Holds the offset and value of the presentation_level field.
 */
typedef struct presentation_level_t
{
    unsigned long offset; /**< offset in bits of the presentation_level field in sample buffer */
    uint8_t value;        /**< value of the presentation_level field */
} presentation_level;

/**
 * \brief Context for the binary sink.
 */
struct bam_binary_sink_t
{
    struct BAMSink      super;               /**< pointer to the parent structure this one inherits from */
    dlb_bitbuf          bitbuf;              /**< bit buffer used to track position and write to an AC-4 sample buffer */
    alps_presentation  *presentations;       /**< array of presentations */
    presentation_level *levels;              /**< array of presentation_level fields; each entry refers to an entry in presentations array with the same index */
    size_t              presentations_count; /**< number of entries in presentations and levels arrays */
    size_t              curr_pres;           /**< currently parsed presentation */
};

/**
 * \brief Just skips \p num_bits bits in the sink buffer.
 * \attention Not intended to be called directly. It is called internally by the BAM-generated AC-4 TOC parser.
 * \param[in] self instance of the \ref bam_binary_sink as returned by \ref bam_binary_sink_create
 * \param[in] num_bits size of the TOC field in bits
 * \param[in] value value of the TOC field
 * \return \ref Always returns BAM_OK.
 */
static bam_error bam_binary_sink_write_align(
    struct BAMSink *self,
    unsigned int num_bits,
    unsigned int value)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;

    dlb_bitbuf_skip(&sink->bitbuf, num_bits);

    return err;
}

/**
 * \brief Saves the value and offset of the given TOC field if it's "presentation_level".
 * Skips the \p num_bits bits in the sink buffer.
 * \attention Not intended to be called directly. It is called internally by the BAM-generated AC-4 TOC parser.
 * \param[in] self instance of the \ref bam_binary_sink as returned by \ref bam_binary_sink_create
 * \param[in] varname name of the TOC field
 * \param[in] num_bits size of the TOC field in bits
 * \param[in] val value of the TOC field
 * \return \ref Always returns BAM_OK.
 */
static bam_error bam_binary_sink_write_uint(
    struct BAMSink *self,
    const char *varname,
    unsigned int num_bits,
    BAMint val)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;

    if (!strcmp(varname, "presentation_level"))
    {
        sink->levels[sink->curr_pres].offset = dlb_bitbuf_get_abs_pos(&sink->bitbuf);
        sink->levels[sink->curr_pres].value = (uint8_t)val;
    }

    dlb_bitbuf_skip(&sink->bitbuf, num_bits);

    return err;
}

/**
 * \brief If the given TOC field is "n_presentations" and its value is higher than current presentations count
 * reallocates and initialize the memory for the list of presentations.
 * \attention Not intended to be called directly. It is called internally by the BAM-generated AC-4 TOC parser.
 * \param[in] self instance of the \ref bam_binary_sink as returned by \ref bam_binary_sink_create
 * \param[in] varname name of the TOC field
 * \param[in] val value of the TOC field
 * \return \ref Always returns BAM_OK.
 */
static bam_error bam_binary_sink_after_assign_int(
    struct BAMSink *self,
    const char *varname,
    BAMint val)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;
    size_t i;

    /* reallocate presentations array if needed */
    if (!strcmp(varname, "n_presentations"))
    {
        if ((size_t)val > sink->presentations_count)
        {
            sink->presentations = realloc(sink->presentations, val * sizeof(*sink->presentations));
            sink->levels = realloc(sink->levels, val * sizeof(*sink->levels));
        }

        memset(sink->presentations, 0, sizeof(*sink->presentations) * val);
        memset(sink->levels, 0, sizeof(*sink->levels) * val);
        sink->presentations_count = val;
        sink->curr_pres = 0;

        for (i = 0; i < sink->presentations_count; i++)
        {
            sink->presentations[i].id = ALPS_INVALID_PRES_ID;
        }
    }

    return err;
}

/**
 * \brief If the given TOC structure is "ac4_presentation_info" or "ac4_presentation_v2_info" increments the index of
 * the currently processed presentation.
 * \attention Not intended to be called directly. It is called internally by the BAM-generated AC-4 TOC parser.
 * \param[in] self instance of the \ref bam_binary_sink as returned by \ref bam_binary_sink_create
 * \param[in] fname name of the TOC structure that was parsed
 * \return \ref Always returns BAM_OK.
 */
static bam_error bam_binary_sink_after_call_void(
    struct BAMSink *self,
    const char *fname)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;

    if (!strcmp(fname, "ac4_presentation_info") || !strcmp(fname, "ac4_presentation_v2_info"))
    {
        sink->curr_pres++;
    }

    return err;
}


/**
 * \brief If the given TOC field is "presentation_id" saves the ID of the currently processed presentation.
 * \attention Not intended to be called directly. It is called internally by the BAM-generated AC-4 TOC parser.
 * \param[in] self instance of the \ref bam_binary_sink as returned by \ref bam_binary_sink_create
 * \param[in] fname name of the parser function used to parse more complex fields
 * \param[in] return_var_name name of the TOC field parsed by the \p fname function
 * \param[in] return_value value of the TOC field parsed by the \p fname function
 * \return \ref Always returns BAM_OK.
 */
static bam_error bam_binary_sink_after_call_int(
    struct BAMSink *self,
    const char *fname,
    const char *return_var_name,
    BAMint return_value)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;

    if (!strcmp(return_var_name, "presentation_id"))
    {
        sink->presentations[sink->curr_pres].id = return_value;
    }

    return err;
}

/**
 * \brief Releases the memory allocated by the \ref bam_binary_sink_create.
 * \attention Not intended to be called directly. It is called internally by the BAM-generated AC-4 TOC parser.
 * \param[in] self instance of the \ref bam_binary_sink as returned by \ref bam_binary_sink_create
 */
static void bam_binary_sink_destroy(
    BAMSink *self)
{
    bam_binary_sink *sink = (bam_binary_sink*)self;

    if (sink)
    {
        free(sink->presentations);
        free(sink->levels);
    }

    free(self);
}

BAMSink *bam_binary_sink_create(
    unsigned char *buffer,
    size_t size_in_bytes)
{
    bam_binary_sink *sink;

    if (buffer == NULL || size_in_bytes == 0)
    {
        return NULL;
    }

    sink = malloc(sizeof(*sink));
    if (sink == NULL)
    {
        return NULL;
    }

    BAMSink_init(&sink->super);

    sink->super.write_align = bam_binary_sink_write_align;
    sink->super.write_uint = bam_binary_sink_write_uint;
    sink->super.after_assign_int = bam_binary_sink_after_assign_int;
    sink->super.after_call_void = bam_binary_sink_after_call_void;
    sink->super.after_call_int = bam_binary_sink_after_call_int;
    sink->super.destroy = bam_binary_sink_destroy;

    dlb_bitbuf_init(&sink->bitbuf, buffer, 8 * size_in_bytes);
    sink->presentations = NULL;
    sink->levels = NULL;
    sink->presentations_count = 0;
    sink->curr_pres = 0;

    return &sink->super;
}

alps_ret bam_binary_sink_get_presentations(
    bam_binary_sink *sink,
    alps_presentation **presentations,
    size_t *presentation_count)
{
    alps_ret ret = ALPS_RET_OK;

    CHECK_EXPR_AND_BAIL(!sink || !presentations || !presentation_count, ALPS_RET_E_INVALID_ARG);

    *presentations = sink->presentations;
    *presentation_count = sink->presentations_count;

bail:
    return ret;
}

alps_ret bam_binary_sink_set_presentation_level(
    bam_binary_sink *sink,
    size_t presentation_index,
    unsigned int presentation_level)
{
    alps_ret ret = ALPS_RET_OK;
    size_t current_position;

    CHECK_EXPR_AND_BAIL(!sink || presentation_index >= sink->presentations_count || presentation_level > 7, ALPS_RET_E_INVALID_ARG);

    current_position = dlb_bitbuf_get_abs_pos(&sink->bitbuf);

    dlb_bitbuf_set_abs_pos(&sink->bitbuf, sink->levels[presentation_index].offset);
    dlb_bitbuf_write(&sink->bitbuf, presentation_level, 3);

    dlb_bitbuf_set_abs_pos(&sink->bitbuf, current_position);

bail:
    return ret;
}
