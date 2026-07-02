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
 * \brief Implementation of the API for setting active AC-4 presentation from application layer.
 */

#include "dlb_alps_native/dlb_alps_native.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dlb_alps_native/alps_mp4dmx/alps_mp4dmx.h"
#include "dlb_alps_native/alps_ac4parser/alps_ac4parser.h"

#include "dlb_alps_native/utils/types.h"
#include "dlb_alps_native/utils/utils.h"

#include "version.h"

/**
 * @brief Holds values calculated and required for operation
 */
struct alps_ctx_t
{
    alps_mp4dmx             *mp4dmx;                 /**< pointer to the MP-4 demuxer's context */
    int                      active_presentation_id; /**< ID of the active presentation */
    alps_presentation       *presentations;          /**< array of presentation parsed from a ISOBMFF segment */
    size_t                   presentations_count;    /**< number of items in the presentations aray */
    presentations_changed_cb presentations_cb;       /**< pointer to the callback called when presentations array changes */
    callback_ctx             presentations_cb_ctx;   /**< pointer to the data passed to the above callback */
};

static void alps_free_presentations_array( alps_presentation **presentations, size_t presentations_count);
static alps_ret alps_get_presentations_from_isobmff_segment(alps_ctx *ctx, int *presentations_changed);

char *alps_version(void)
{
    static const char version[] = ALPS_NATIVE_VERSION;
    return (char*)version;
}

alps_ret alps_query_mem(
    size_t *mem_size)
{
    alps_ret ret = ALPS_RET_OK;
    size_t mp4dmx_mem_size = 0;

    CHECK_EXPR_AND_BAIL(!mem_size, ALPS_RET_E_INVALID_ARG);

    CHECK_RET_AND_BAIL(alps_mp4dmx_query_mem(&mp4dmx_mem_size));

    *mem_size = sizeof(alps_ctx) + mp4dmx_mem_size;

bail:
    return ret;
}

alps_ret alps_init(
    alps_ctx **ctx,
    void      *mem)
{
    alps_ret ret = ALPS_RET_OK;

    CHECK_EXPR_AND_BAIL(!ctx || !mem, ALPS_RET_E_INVALID_ARG);

    *ctx = mem;

    CHECK_RET_AND_BAIL(alps_mp4dmx_init(&(*ctx)->mp4dmx, (unsigned char*)mem + sizeof(alps_ctx)));

    (*ctx)->active_presentation_id = ALPS_INVALID_PRES_ID;
    (*ctx)->presentations = NULL;
    (*ctx)->presentations_count = 0;
    (*ctx)->presentations_cb = NULL;
    (*ctx)->presentations_cb_ctx = NULL;

bail:
    return ret;
}

void alps_destroy(
    alps_ctx *ctx
)
{
    if (ctx != 0)
    {
        alps_mp4dmx_destroy(ctx->mp4dmx);
        alps_free_presentations_array(&ctx->presentations, ctx->presentations_count);
    }
}

void alps_set_presentations_changed_callback(
    alps_ctx                *ctx,
    presentations_changed_cb function,
    callback_ctx             data)
{
    ctx->presentations_cb = function;
    ctx->presentations_cb_ctx = data;
}

callback_ctx alps_get_presentations_changed_callback_context(
    alps_ctx *ctx
)
{
    return ctx->presentations_cb_ctx;
}

alps_ret alps_process_isobmff_segment(
    alps_ctx      *ctx,
    unsigned char *buffer,
    size_t         size)
{
    alps_ret ret = ALPS_RET_OK;
    int presentations_changed = 0;
    unsigned char *sample_data = NULL;
    size_t sample_size = 0;

    CHECK_EXPR_AND_BAIL(!ctx || !buffer, ALPS_RET_E_INVALID_ARG);

    CHECK_RET_AND_BAIL(alps_mp4dmx_process_buffer(ctx->mp4dmx, buffer, size));

    CHECK_RET_AND_BAIL(alps_get_presentations_from_isobmff_segment(ctx, &presentations_changed));
    if (presentations_changed && ctx->presentations_cb)
    {
        ctx->presentations_cb(ctx->presentations_cb_ctx);
    }

    while ((ret = alps_mp4dmx_next_sample(ctx->mp4dmx, &sample_data, &sample_size)) == ALPS_RET_OK)
    {
        CHECK_RET_AND_BAIL(alps_ac4parser_set_active_presentation_id(sample_data, sample_size, ctx->active_presentation_id));
    }

    if (ret == ALPS_RET_E_NEXT_SEGMENT)
    {
        /* all samples in the segment were processed and that's okay */
        ret = ALPS_RET_OK;
    }

bail:
    return ret;
}

alps_ret alps_get_presentations(
    alps_ctx           *ctx,
    alps_presentation **presentations,
    size_t             *count
)
{
    alps_ret ret = ALPS_RET_OK;

    CHECK_EXPR_AND_BAIL(!ctx || !presentations || !count, ALPS_RET_E_INVALID_ARG);

    *presentations = ctx->presentations;
    *count = ctx->presentations_count;

bail:
    return ret;
}

alps_ret alps_get_active_presentation_id(
    alps_ctx *ctx,
    int      *presentation_id)
{
    alps_ret ret = ALPS_RET_OK;

    CHECK_EXPR_AND_BAIL(!ctx || !presentation_id, ALPS_RET_E_INVALID_ARG);

    *presentation_id = ctx->active_presentation_id;

bail:
    return ret;
}

alps_ret alps_set_active_presentation_id(
    alps_ctx *ctx,
    int       presentation_id)
{
    alps_ret ret = ALPS_RET_OK;

    CHECK_EXPR_AND_BAIL(!ctx, ALPS_RET_E_INVALID_ARG);

    ctx->active_presentation_id = presentation_id;

bail:
    return ret;
}

/* PRIVATE METHODS */

/**
 * @brief Releases memory allocated for a kinds array
 * @param[in] kinds pointer to the kinds array
 * @param[in] kinds_count number of elements in the array
 */
static void alps_free_presentation_kinds_array(
    alps_presentation_kind **kinds,
    size_t kinds_count
)
{
    unsigned int i;
    for (i = 0; *kinds != NULL && i < kinds_count; i++)
    {
        free((*kinds)[i].scheme_uri);
        free((*kinds)[i].value);
    }

    free(*kinds);
    *kinds = NULL;
}

/**
 * @brief Releases memory allocated for a labels array
 * @param[in] labels pointer to the labels array
 * @param[in] labels_count number of elements in the array
 */
static void alps_free_presentation_labels_array(
    alps_presentation_label **labels,
    size_t labels_count
)
{
    unsigned int i;
    for (i = 0; *labels != NULL && i < labels_count; i++)
    {
        free((*labels)[i].label);
        free((*labels)[i].language);
    }

    free(*labels);
    *labels = NULL;
}

/**
 * @brief Releases memory allocated for presentation struct.
 * @param[in] presentation pointer to the presentation struct
 */
static void alps_free_presentation(
    alps_presentation *presentation)
{

    free(presentation->extended_language);
    alps_free_presentation_labels_array( &presentation->labels, presentation->labels_count);
    alps_free_presentation_kinds_array(&presentation->kinds, presentation->kinds_count);
}

/**
 * @brief Releases memory allocated for a presentations array.
 * @param[in] presentations pointer to the presentations array
 * @param[in] presentations_count number of elements in the array
 */
static void alps_free_presentations_array(
    alps_presentation **presentations,
    size_t presentations_count)
{
    size_t i;

    for (i = 0; i < presentations_count; i++)
    {
        alps_free_presentation(&(*presentations)[i]);
    }

    free(*presentations);
    *presentations = NULL;
}

/**
 * @brief Constructs presentations array from preselections parsed from ISOBMFF segment.
 * @param[in] ctx pointer to the library context
 * @param[out] presentations_changed set to <c>true</c> if presentations array changed and to <c>false</c> otherwise
 * @return \ref ALPS_RET_E_INVALID_ARG if \p ctx or \p presentations_changed is 0, \ref ALPS_RET_OK otherwise
 */
static alps_ret alps_get_presentations_from_isobmff_segment(
    alps_ctx *ctx,
    int *presentations_changed)
{
    alps_ret ret = ALPS_RET_OK;
    alps_mp4dmx_preselection *preselections = NULL;
    size_t preselections_count = 0;

    CHECK_EXPR_AND_BAIL(ctx == NULL || presentations_changed == NULL, ALPS_RET_E_INVALID_ARG);

    *presentations_changed = 0;

    /* get preselections from prsl boxes */
    CHECK_RET_AND_BAIL(alps_mp4dmx_get_preselections(ctx->mp4dmx, &preselections, &preselections_count));
    if (preselections != NULL)
    {
        size_t i, k;

        if (preselections_count != ctx->presentations_count)
        {
            *presentations_changed = 1;
        }

        /* reallocate memory for the presentations array if needed */
        if (preselections_count != ctx->presentations_count)
        {
            alps_free_presentations_array(&ctx->presentations, ctx->presentations_count);
            ctx->presentations = realloc(ctx->presentations, preselections_count * sizeof(*ctx->presentations));
            memset(ctx->presentations, 0, preselections_count * sizeof(*ctx->presentations));
            ctx->presentations_count = preselections_count;
        }

        for (i = 0; i < ctx->presentations_count; i++)
        {
            alps_presentation *pres = &ctx->presentations[i];
            alps_mp4dmx_preselection *prsl = &preselections[i];

            if (pres->id != prsl->preselection_tag)
            {
                *presentations_changed = 1;
            }

            pres->id = prsl->preselection_tag;
            pres->audio_rendering_indication = prsl->audio_rendering_indication;
            pres->selection_priority = prsl->selection_priority;
            pres->dialog_gain = prsl->dialog_gain;
            pres->dialog_gain_present = prsl->dialog_gain_present;


            /* reallocate labels array if needed */
            if (prsl->labels && (!pres->labels || pres->labels_count != prsl->labels_count))
            {
                alps_free_presentation_labels_array(&pres->labels, pres->labels_count);
                pres->labels = malloc(prsl->labels_count * sizeof(*pres->labels));
                pres->labels_count = prsl->labels_count;
            }

            /* copy labels array from preselection */
            for (k = 0; k < pres->labels_count; k++)
            {
                size_t len = strlen(prsl->labels[k].label) + 1;
                pres->labels[k].label = malloc(len);
                memcpy(pres->labels[k].label, prsl->labels[k].label, len);
                len = strlen(prsl->labels[k].language) + 1;
                pres->labels[k].language = malloc(len);
                memcpy(pres->labels[k].language, prsl->labels[k].language, len);
                pres->labels[k].is_group_label = prsl->labels[k].is_group_label;
                pres->labels[k].label_id = prsl->labels[k].label_id;
            }

            /* reallocate extended_language string if needed */
            if (prsl->extended_language && (!pres->extended_language || strlen(pres->extended_language) < strlen(prsl->extended_language)))
            {
                pres->extended_language = realloc(pres->extended_language, strlen(prsl->extended_language) + 1);
                *presentations_changed = 1;
            }

            /* copy extended_language from preselection */
            if (prsl->extended_language && strcmp(pres->extended_language, prsl->extended_language))
            {
                memcpy(pres->extended_language, prsl->extended_language, strlen(prsl->extended_language) + 1);
                *presentations_changed = 1;
            }

            /* reallocate kinds array if needed */
            if (prsl->kinds && (!pres->kinds || pres->kinds_count != prsl->kinds_count))
            {
                alps_free_presentation_kinds_array(&pres->kinds, pres->kinds_count);
                pres->kinds = malloc(prsl->kinds_count * sizeof(*pres->kinds));
                pres->kinds_count = prsl->kinds_count;
            }

            /* copy kinds array from preselection */
            for (k = 0; k < pres->kinds_count; k++)
            {
                size_t len = strlen(prsl->kinds[k].scheme_uri) + 1;
                pres->kinds[k].scheme_uri = malloc(len);
                memcpy(pres->kinds[k].scheme_uri, prsl->kinds[k].scheme_uri, len);
                len = strlen(prsl->kinds[k].value) + 1;
                pres->kinds[k].value = malloc(len);
                memcpy(pres->kinds[k].value, prsl->kinds[k].value, len);
            }
        }
    }

bail:
    if (ret != ALPS_RET_OK)
    {
        alps_free_presentations_array(&ctx->presentations, ctx->presentations_count);
    }

    return ret;
}
