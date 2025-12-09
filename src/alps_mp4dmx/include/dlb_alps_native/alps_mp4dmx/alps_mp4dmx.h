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
 * \brief API of the ISOBMFF demuxer.
 *
 * This demuxer is able to process an ISOBMFF segment to parse list of preselections out of it and iterate through all
 * ac-4 samples.
 */

#ifndef DLB_ALPS_NATIVE_ALPS_MP4DMX_ALPS_MP4DMX_H
#define DLB_ALPS_NATIVE_ALPS_MP4DMX_ALPS_MP4DMX_H

#include <stddef.h>
#include <sys/types.h> 
#include "dlb_alps_native/utils/types.h"


/**
 * @brief Handle for the demuxer context
 */
typedef struct alps_mp4dmx_t alps_mp4dmx;

/**
 * @brief Kind or role of the preselection as parsed from a ISOBMFF's 'kind' box.
 */
typedef struct alps_mp4dmx_prsl_kind_t
{
    char *scheme_uri; /**< the name of the kind or naming scheme for the value */
    char *value;      /**< name of the kind from the scheme defined by scheme_uri */
} alps_mp4dmx_prsl_kind;

/**
 * @brief Label or GroupLabel of the preselection as parsed from a ISOBMFF's 'labl' box.
 */
typedef struct alps_mp4dmx_prsl_labl_t
{
    u_int16_t label_id;      /** id of the label */
    char* language;          /** language of the label */
    char* label;             /** the actual label text */
    u_int8_t is_group_label; /** is this a label for the group of labels */
} alps_mp4dmx_prsl_labl;

/**
 * @brief Preselection data as parsed from ISOBMFF's 'prsl' box. In context of ALPS this describes ac-4 presentation.
 */
typedef struct alps_mp4dmx_preselection_t
{
    unsigned int num_entities_in_group; /**< number of entries in the \ref entity_ids array */
    unsigned int *entity_ids;           /**< array containing IDs of entities in the group described by the EntityToGroupBox */
    int preselection_tag;               /**< unique identifier of the preselection (ac-4 presentation ID) */
    char *extended_language;            /**< language tag denoting the language of the preselection */
    alps_mp4dmx_prsl_kind *kinds;       /**< array of kinds the preselection is of */
    unsigned int kinds_count;           /**< number of items in kinds array */
    alps_mp4dmx_prsl_labl *labels;      /**< array of the labels describing this preselection */
    unsigned int labels_count;          /**< number if items in the labels array */
    int selection_priority;             /**< selection priority of the presentation */
    int audio_rendering_indication;     /**< audio rendering indication, or 255 if not present in box */
    int dialog_gain_present;            /**< signals whether the dialog_gain value is present */
    float dialog_gain;                  /**< represents a gain value in decibel, reflecting the perceptually subjective gain of dialogue in the mix. */
                                        /**< this field is optional, the value NaN should be interpereted as missing info*/
} alps_mp4dmx_preselection;

/**
 * @brief Gives the size of the memory required by the demuxer. This memory should be allocated and freed by the user.
 * @param[out] mem_size size of the required memory in bytes
 * @return \ref ALPS_RET_E_INVALID_ARG if \p mem_size is 0; \ref ALPS_RET_OK if successful
 */
alps_ret alps_mp4dmx_query_mem(
    size_t *mem_size
);

/**
 * @brief Initializes the demuxer context.
 * @param[out] dmx_ptr pointer to the initialized demuxer context handle
 * @param[in] mem pointer to the memory the size of at least as much as returned by \ref alps_mp4dmx_query_mem
 * @return
 * - \ref ALPS_RET_E_INVALID_ARG if \p dmx_ptr or \p mem is 0
 * - \ref ALPS_RET_E_UNDEFINED if the initialization fails
 * - \ref ALPS_RET_OK if the initialization succeeds
 */
alps_ret alps_mp4dmx_init(
    alps_mp4dmx **dmx_ptr,
    void         *mem
);

/**
 * @brief Releases all the resources allocated by the demuxer.
 * @param[in] dmx the demuxer handle
 */
void alps_mp4dmx_destroy(
    alps_mp4dmx   *dmx
);

/**
 * @brief Parses given ISOBMFF segment for preselections list and initializes AC-4 sample iterator.
 * @param[in] dmx the demuxer handle
 * @param[in] buffer byte array containing the ISOBMFF segment
 * @param[in] size size of the buffer in bytes
 * @return
 * - \ref ALPS_RET_E_INVALID_ARG if \p dmx or \p buffer is 0
 * - \ref ALPS_RET_E_PARSE if the segment could not be successfully parsed
 * - \ref ALPS_RET_E_NO_AC4_TRACK if no AC_4 track could be found in the segment
 * - \ref ALPS_RET_OK if the processing succeeds
 */
alps_ret alps_mp4dmx_process_buffer(
    alps_mp4dmx   *dmx,
    unsigned char *buffer,
    size_t         size
);

/**
 * @brief Retrieves subsequent AC-4 sample from the ISOBMFF segment. \ref alps_mp4dmx_process_buffer needs to be run
 * before to initialize the iterator.
 * @param[in] dmx the demuxer handle
 * @param[out] data byte array containing retrieved sample's data
 * @param[out] size size of the sample data in bytes
 * @return
 * - \ref ALPS_RET_E_INVALID_ARG if \p dmx, \p data or \p size is 0
 * - \ref ALPS_RET_E_PARSE if the segment was not yet processed or the next sample cannot be located
 * - \ref ALPS_RET_OK if the operation succeeds
 */
alps_ret alps_mp4dmx_next_sample(
    alps_mp4dmx    *dmx,
    unsigned char **data,
    size_t         *size
);

/**
 * @brief Retrieves the list of the preselection from the ISOBMFF segment. \ref alps_mp4dmx_process_buffer needs to be
 * run before to parse out the preselection list.
 * @param[in] dmx the demuxer handle
 * @param[out] preselections array of the preselections
 * @param[out] count number of items in the preselections array
 * @return
 * - \ref ALPS_RET_E_INVALID_ARG if any of the arguments is 0
 * - \ref ALPS_RET_OK if the operation succeeds
 */
alps_ret alps_mp4dmx_get_preselections(
    alps_mp4dmx               *dmx,
    alps_mp4dmx_preselection **preselections,
    size_t                    *count
);

#endif /* DLB_ALPS_NATIVE_ALPS_MP4DMX_ALPS_MP4DMX_H */
