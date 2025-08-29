/***********************************************************************************************************************
 * Copyright (C) 2024 by Dolby International AB.
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
 * \brief Implementation of the ISOBMFF demuxer's API. See \ref alps_mp4dmx.h
 */

#include "dlb_alps_native/alps_mp4dmx/alps_mp4dmx.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "mp4d_buffer.h"
#include "mp4d_demux.h"
#include "mp4d_nav.h"
#include "mp4d_trackreader.h"

#include "dlb_alps_native/utils/utils.h"

/**
 * \brief Evaluates \p expr to \ref mp4d_error_t and checks its value. If it is an error it sets \ref ret variable to
 * corresponding \ref alps_ret value if possible or \p ret_val if not and then goes to \ref bail label. \ref ret needs
 * to be declared before using this macro.
 * \param[in] expr an expression that returns value of \ref mp4d_error_t type
 * \param[in] ret_val the value to set \ref ret variable to if \ref expr does not have corresponding \ref alps_ret value.
 */
#define CHECK_MP4D_ERR_AND_BAIL(expr, ret_val)              \
    {                                                       \
        mp4d_error_t mp4d_err_local = (expr);               \
        if (mp4d_err_local == MP4D_E_WRONG_ARGUMENT)        \
        {                                                   \
            ret = ALPS_RET_E_INVALID_ARG;                   \
            goto bail;                                      \
        }                                                   \
        else if (mp4d_err_local == MP4D_E_BUFFER_TOO_SMALL) \
        {                                                   \
            ret = ALPS_RET_E_BUFF_TOO_SMALL;                \
            goto bail;                                      \
        }                                                   \
        else if (mp4d_err_local == MP4D_E_NEXT_SEGMENT)     \
        {                                                   \
            ret = ALPS_RET_E_NEXT_SEGMENT;                  \
            goto bail;                                      \
        }                                                   \
        else if (mp4d_err_local != MP4D_NO_ERROR)           \
        {                                                   \
            ret = (ret_val);                                \
            goto bail;                                      \
        }                                                   \
    }

/**
 * \brief Checks if the \ref ptr is NULL, and if not, frees the memory address at
 * \ref ptr, and sets ptr to NULL
 * \param[in] ptr the pointer to check and free
 */
#define  CHECK_FOR_NULL_AND_FREE(ptr) \
    if (ptr != 0) {                   \
        free(ptr);                    \
        ptr = NULL;                    \
    }                                 \

/**
 * @brief Demuxer's context holding all the relevant data required for parsing.
 */
struct alps_mp4dmx_t
{
    mp4d_demuxer_ptr_t        mp4d_demuxer;        /**> pointer to the actual MP4 demuxer used internally */
    mp4d_trackreader_ptr_t    mp4d_track_reader;   /**> pointer to the object that reads MP4 tracks */
    unsigned char            *segment_buf;         /**> buffer that holds current MP4 segment's data */
    size_t                    segment_buf_pos;     /**> number of bytes in the segment's buffer already parsed */
    mp4d_movie_info_t         movie_info;          /**> MP4 movie info parsed from moov box */
    mp4d_stream_info_t        ac4_track_info;      /**> stream info from first AC-4 track parsed from moov box */
    alps_mp4dmx_preselection *preselections;       /**> array of preselections */
    uint32_t                  preselections_count; /**> number of items in preselections array */
};

/**
 * @brief Struct representing ISOBMFF FullBox.
 */
typedef struct  alps_mp4dmx_full_box_t
{
    mp4d_atom_t box; /**> the underlying Box structure */
    uint8_t version; /**> box's version value */
} alps_mp4dmx_full_box;

/**
 * @brief Struct representing GroupsListBox.
 */
typedef struct  alps_mp4dmx_grpl_t
{
    mp4d_atom_t box;                         /**> the underlying Box structure */
    uint32_t preselections_count;            /**> number of PreselectionGroupBoxes inside this box */
    alps_mp4dmx_preselection *preselections; /**> array of preselections created from the PreselectionGroupBoxes */
} alps_mp4dmx_grpl;

/**
 * @brief Struct representing MetaBox.
 */
typedef struct  alps_mp4dmx_meta_t
{
    alps_mp4dmx_full_box full_box; /**> underlying FullBox structure */
    alps_mp4dmx_grpl grpl;         /**> GroupsListBox inside this MetaBox */
} alps_mp4dmx_meta;

/**
 * @brief Structure representing MovieBox
 */
typedef struct  alps_mp4dmx_moov_t
{
    mp4d_atom_t box;       /**> underlying Box structure */
    alps_mp4dmx_meta meta; /**> MetaBox inside this MovieBox */
} alps_mp4dmx_moov;

alps_ret alps_mp4dmx_query_mem(
    size_t *mem_size)
{
    alps_ret ret = ALPS_RET_OK;
    uint64_t static_mem_size = 0;
    uint64_t dynamic_mem_size = 0;

    CHECK_EXPR_AND_BAIL(!mem_size, ALPS_RET_E_INVALID_ARG);

    CHECK_MP4D_ERR_AND_BAIL(mp4d_demuxer_query_mem(&static_mem_size, &dynamic_mem_size), ALPS_RET_E_UNDEFINED);
    *mem_size = sizeof(alps_mp4dmx) + static_mem_size + dynamic_mem_size;

    CHECK_MP4D_ERR_AND_BAIL(mp4d_trackreader_query_mem(&static_mem_size, &dynamic_mem_size), ALPS_RET_E_UNDEFINED);
    *mem_size += static_mem_size + dynamic_mem_size;

bail:
    return ret;
}

alps_ret alps_mp4dmx_init(
    alps_mp4dmx **demuxer,
    void         *mem)
{
    alps_ret ret = ALPS_RET_OK;
    uint64_t static_mem_size = 0;
    uint64_t dynamic_mem_size = 0;
    unsigned char *mem_bytes = mem;
    void *static_mem;
    void *dynamic_mem;

    CHECK_EXPR_AND_BAIL(!demuxer || !mem, ALPS_RET_E_INVALID_ARG);

    *demuxer = (alps_mp4dmx*)mem_bytes;
    mem_bytes += sizeof(**demuxer);

    memset(*demuxer, 0, sizeof(**demuxer));

    /* initialize mp4d_demuxer_ptr_t */
    CHECK_MP4D_ERR_AND_BAIL(mp4d_demuxer_query_mem(&static_mem_size, &dynamic_mem_size), ALPS_RET_E_UNDEFINED);
    static_mem = mem_bytes;
    mem_bytes += static_mem_size;
    dynamic_mem = mem_bytes;
    mem_bytes += dynamic_mem_size;
    CHECK_MP4D_ERR_AND_BAIL(mp4d_demuxer_init(&(*demuxer)->mp4d_demuxer, static_mem, dynamic_mem), ALPS_RET_E_UNDEFINED);

    /* initialize mp4d_trackreader_ptr_t */
    CHECK_MP4D_ERR_AND_BAIL(mp4d_trackreader_query_mem(&static_mem_size, &dynamic_mem_size), ALPS_RET_E_UNDEFINED);
    static_mem = mem_bytes;
    mem_bytes += static_mem_size;
    dynamic_mem = mem_bytes;
    mem_bytes += dynamic_mem_size;
    CHECK_MP4D_ERR_AND_BAIL(mp4d_trackreader_init(&(*demuxer)->mp4d_track_reader, static_mem, dynamic_mem), ALPS_RET_E_UNDEFINED);
    /* set fake track encryption data - without it mp4d_trackreader_next_sample() will return MP4D_E_WRONG_ARGUMENT
       when there's senc box present;
       the track encryption data should be set with values read from tenc box but mp4d_parse_tenc() in mp4d_demux.c is
       outdated and cannot properly read modern tenc boxes;
       the fake data might render data read from senc box invalid but we're not using it anyway */
    CHECK_MP4D_ERR_AND_BAIL(mp4d_trackreader_set_tenc((*demuxer)->mp4d_track_reader, 0, 8, (uint8_t*)"some_fake_key_id"), ALPS_RET_E_UNDEFINED);

bail:
    return ret;
}

static void alps_mp4dmx_free_preselection(alps_mp4dmx_preselection *preselection);

void alps_mp4dmx_destroy(
    alps_mp4dmx *dmx
)
{
    uint32_t i;

    if (!dmx)
    {
        return;
    }

    for (i = 0; i < dmx->preselections_count; i++)
    {
        alps_mp4dmx_free_preselection(&dmx->preselections[i]);
    }

    free(dmx->preselections);
    dmx->preselections = 0;
}

/**
 * @brief Parses members of a FullBox
 * @param[in] atom pointer to a FullBox structure to be parsed
 * @return
 * - \ref ALPS_RET_E_INVALID_ARG if \p atom is 0 or it has not been properly initialized with \ref mp4d_parse_atom_header
 * - \ref ALPS_RET_E_BUFF_TOO_SMALL if the size of the underlying buffer is too small to contain FullBox members
 * - \ref ALPS_RET_OK if operation is successful
 */
static alps_ret alps_mp4dmx_parse_full_box(
    alps_mp4dmx_full_box *atom
)
{
    alps_ret ret = ALPS_RET_OK;
    mp4d_buffer_t atom_buf;

    CHECK_EXPR_AND_BAIL(!atom || !atom->box.p_data, ALPS_RET_E_INVALID_ARG);

    atom_buf = mp4d_atom_to_buffer((mp4d_atom_t*)atom);

    /* skip version */
    atom->version = mp4d_read_u8(&atom_buf);
    CHECK_EXPR_AND_BAIL(atom->box.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);

    atom->box.flags = mp4d_read_u24(&atom_buf);
    CHECK_EXPR_AND_BAIL(atom->box.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);

    atom->box.p_data = atom_buf.p_data;
    atom->box.size = atom_buf.size;

bail:
    return ret;
}

/**
 * @brief Calculates the length of utf8string
 * @param[in] buf bit buffer containing the string; beginning of the buffer is the beginning of the string
 * @param[out] length calculated length of the string
 * @return \ref ALPS_RET_E_BUFF_TOO_SMALL if the buffer is too short to hold the string data, \ref ALPS_RET_OK if
 * everything's all right
 */
static alps_ret alps_mp4dmx_mp4d_string_length(
    mp4d_buffer_t *buf,
    size_t *length
)
{
    size_t i;

    *length = 0;

    if (buf->size == 0 || buf->size == (uint64_t)-1)
    {
        return ALPS_RET_E_BUFF_TOO_SMALL;
    }

    for (i = 0; buf->size - i != 0 && buf->p_data[i] != 0; i++);
    if (buf->size - i == 0 && buf->p_data[i] != 0)
    {
        return ALPS_RET_E_BUFF_TOO_SMALL;
    }

    *length = i + 1; /* plus one for terminating null char */

    return ALPS_RET_OK;
}

/**
 * @brief Reads an utf8string.
 * @param[in] buf bit buffer containing the string; beginning of the buffer is the beginning of the string
 * @param[in] string buffer to read the string to
 * @return \ref ALPS_RET_E_BUFF_TOO_SMALL if the buffer is too short to hold the string data, \ref ALPS_RET_OK if
 * everything's all right
 */
static alps_ret alps_mp4dmx_mp4d_read_string(
    mp4d_buffer_t *buf,
    char         **string
)
{
    alps_ret ret = ALPS_RET_OK;
    size_t length;

    CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_string_length(buf, &length));
    if (length > 0)
    {
        *string = malloc(length);
        memcpy(*string, buf->p_data, length);
        mp4d_skip_bytes(buf, length); /* not checking buf size; it was checked when finding length */
    }

bail:
    return ret;
}

/**
 * @brief Skips number of bytes equal to the length of an ut8string present in the buffer
 * @param[in] buf bit buffer containing the string; beginning of the buffer is the beginning of the string
 * @return \ref ALPS_RET_E_BUFF_TOO_SMALL if the buffer is too short to hold the string data, \ref ALPS_RET_OK if
 * everything's all right
 */
static alps_ret alps_mp4dmx_mp4d_skip_string(
    mp4d_buffer_t *buf
)
{
    alps_ret ret = ALPS_RET_OK;
    size_t length;

    CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_string_length(buf, &length));
    mp4d_skip_bytes(buf, length); /* not checking buf size; it was checked when finding length */

bail:
    return ret;
}

/**
 * @brief Releases memory allocated for a kind struct
 * @param[in] kind pointer to the kind struct
 */
static void alps_mp4dmx_free_kind(
    alps_mp4dmx_prsl_kind *kind
)
{
    CHECK_FOR_NULL_AND_FREE(kind->scheme_uri);
    CHECK_FOR_NULL_AND_FREE(kind->value);
}
static void alps_mp4dmx_free_labl(
    alps_mp4dmx_prsl_labl *label
)
{
    CHECK_FOR_NULL_AND_FREE(label->label);
    CHECK_FOR_NULL_AND_FREE(label->language);
}

/**
 * @brief Releases memory allocated for a preselection struct
 * @param[in] preselection pointer to the preselection struct
 */
static void alps_mp4dmx_free_preselection(
    alps_mp4dmx_preselection *preselection
)
{

    unsigned int i;
    CHECK_FOR_NULL_AND_FREE(preselection->extended_language);
    free(preselection->entity_ids);
    for (i = 0; preselection->kinds != NULL && i < preselection->kinds_count; i++)
    {
        alps_mp4dmx_free_kind(&preselection->kinds[i]);
    }

    free(preselection->kinds);

    for (i = 0; preselection->labels != NULL && i < preselection->labels_count; i++)
    {
        alps_mp4dmx_free_labl(&preselection->labels[i]);
    }

    free(preselection->labels);

    memset(preselection, 0, sizeof(*preselection));
    preselection->preselection_tag = -1;
}

/**
 * @brief Constructs a preselection struct from the given atom
 * @param[in] prsl prsl atom to parse
 * @param[out] preselection pointer to the preselection struct to fill
 * @return \ref ALPS_RET_E_BUFF_TOO_SMALL when given buffer's size is too small to hold the whole atom, \ref ALPS_RET_E_PARSE
 * when the atom is malformed, \ref ALPS_RET_OK when everything's all right
 */
static alps_ret alps_mp4dmx_create_preselection(
    mp4d_atom_t *prsl,
    alps_mp4dmx_preselection *preselection
)
{
    alps_ret ret = ALPS_RET_OK;
    mp4d_error_t mp4d_err = MP4D_NO_ERROR;
    mp4d_buffer_t buf;
    uint32_t flags;
    int preselection_tag_present;
    int selection_priority_present;
    int interleaving_tag_present;
    uint32_t i;
    mp4d_atom_t atom;

    /* releases memory when preselection instance is reused */
    alps_mp4dmx_free_preselection(preselection);

    buf = mp4d_atom_to_buffer(prsl);
    mp4d_skip_bytes(&buf, 1); /* skip version */
    CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);

    flags = mp4d_read_u24(&buf);
    CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);
    preselection_tag_present = flags & 0x1000;
    selection_priority_present = flags & 0x2000;
    interleaving_tag_present = flags & 0x4000;

    mp4d_skip_bytes(&buf, 4); /* skip group_id */
    CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);

    preselection->num_entities_in_group = mp4d_read_u32(&buf);
    CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);
    preselection->entity_ids = malloc(preselection->num_entities_in_group * sizeof(preselection->entity_ids[0]));
    for (i = 0; i < preselection->num_entities_in_group; i++)
    {
        preselection->entity_ids[i] = mp4d_read_u32(&buf);
        CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);
    }

    if (preselection_tag_present)
    {
        char *prsl_tag_string = 0;
        CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_read_string(&buf, &prsl_tag_string));
        if (prsl_tag_string)
        {
            preselection->preselection_tag = atoi(prsl_tag_string);
            free(prsl_tag_string);
        }
    }

    if (selection_priority_present)
    {
        preselection->selection_priority = mp4d_read_u8(&buf);
        CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);
    }

    if (interleaving_tag_present)
    {
        CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_skip_string(&buf)); /* skip interleaving_tag */
        CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);
    }

    /* align atom pointer with processed buffer to be able to find child atoms */
    prsl->p_data = buf.p_data;
    prsl->size = buf.size;

    /* find and parse ExtendedLanguageBox */
    CHECK_MP4D_ERR_AND_BAIL(mp4d_find_atom(prsl, "elng", 0, &atom), ALPS_RET_E_PARSE);
    buf = mp4d_atom_to_buffer(&atom);
    mp4d_skip_bytes(&buf, 4); /* skip version and flags */
    CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);
    CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_read_string(&buf, &preselection->extended_language));


    // Count the LabelBoxes
    i = 0;
    while (!(mp4d_err = mp4d_find_atom(prsl, "labl", i, &atom)))
    {
        i++;
    }

    if ((mp4d_err != MP4D_NO_ERROR && mp4d_err != MP4D_E_ATOM_UNKNOWN) || i == 0)
    {
        CHECK_MP4D_ERR_AND_BAIL(mp4d_err, ALPS_RET_E_PARSE);
    }

    preselection->labels = malloc(sizeof(alps_mp4dmx_prsl_labl) * i);
    preselection->labels_count = i;
    for (i = 0; i < preselection->labels_count; i++)
    {
        memset(&preselection->labels[i], 0,sizeof(alps_mp4dmx_prsl_labl ));
        mp4d_find_atom(prsl, "labl", i, &atom);
        buf = mp4d_atom_to_buffer(&atom);
        mp4d_skip_bytes(&buf, 1); /* skip version */
        CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);

        preselection->labels[i].is_group_label = mp4d_read_u24(&buf) & 0x1; /* read is_group_label from flags */
        CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);

        preselection->labels[i].label_id = mp4d_read_u16(&buf);
        CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);

        CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_read_string(&buf, &preselection->labels[i].language));
        CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_read_string(&buf, &preselection->labels[i].label));
    }

    /* count the KindBoxes */
    i = 0;
    while (!(mp4d_err = mp4d_find_atom(prsl, "kind", i, &atom)))
    {
        i++;
    }

    // KindBox with Dialogue Enhancement level is optional so it's okay if none is found
    if (mp4d_err != MP4D_NO_ERROR && mp4d_err != MP4D_E_ATOM_UNKNOWN)
    {
        CHECK_MP4D_ERR_AND_BAIL(mp4d_err, ALPS_RET_E_PARSE);
    }

    if (i > 0)
    {
        preselection->kinds = malloc(i * sizeof(alps_mp4dmx_prsl_kind));
        preselection->kinds_count = i;
        for (i = 0; i < preselection->kinds_count; i++)
        {
            memset(&preselection->kinds[i], 0,sizeof(alps_mp4dmx_prsl_kind ));
            mp4d_find_atom(prsl, "kind", i, &atom);
            buf = mp4d_atom_to_buffer(&atom);
            CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);

            mp4d_skip_bytes(&buf, 4); /* skip version and flags */
            CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_read_string(&buf, &preselection->kinds[i].scheme_uri));
            CHECK_RET_AND_BAIL(alps_mp4dmx_mp4d_read_string(&buf, &preselection->kinds[i].value));
        }
    }


    /* find AudioRenderingIndicationBox */
    mp4d_err = mp4d_find_atom(prsl, "ardi", 0, &atom);
    // AudioRenderingIndicationBox is optional
    if (mp4d_err == MP4D_NO_ERROR )
    {
        buf = mp4d_atom_to_buffer(&atom);
        mp4d_skip_bytes(&buf, 4); /* skip version and flags */
        CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);
        preselection->audio_rendering_indication = mp4d_read_u8(&buf);
        CHECK_EXPR_AND_BAIL(buf.size == (uint64_t)-1, ALPS_RET_E_BUFF_TOO_SMALL);
    }
    // AudioRenderingIndicationBox is optional
    else if (mp4d_err != MP4D_E_ATOM_UNKNOWN)
    {
        preselection->audio_rendering_indication = (uint8_t)-1;
        CHECK_MP4D_ERR_AND_BAIL(mp4d_err, ALPS_RET_E_PARSE);
    }

bail:
    if (ret != ALPS_RET_OK)
    {
        alps_mp4dmx_free_preselection(preselection);
    }

    return ret;
}

/**
 * @brief Releases memory allocated for a grpl struct
 * @param[in] grpl pointer to the grpl struct
 */
static void alps_mp4dmx_free_grpl(
    alps_mp4dmx_grpl *grpl
)
{
    uint32_t i;

    for (i = 0; i < grpl->preselections_count; i++)
    {
        alps_mp4dmx_free_preselection(&grpl->preselections[i]);
    }

    free(grpl->preselections);
    grpl->preselections = 0;
}

/**
 * @brief Constructs a grpl struct by parsing the atom it contains
 * @param[in] atom pointer to the grpl struct that contains the grpl atom
 * @return \ref ALPS_RET_E_BUFF_TOO_SMALL when given buffer's size is too small to hold the whole atom, \ref ALPS_RET_E_PARSE
 * when the atom is malformed, \ref ALPS_RET_OK when everything's all right
 */
static alps_ret alps_mp4dmx_parse_grpl(
    alps_mp4dmx_grpl *atom
)
{
    alps_ret ret = ALPS_RET_OK;
    mp4d_error_t mp4d_err = MP4D_NO_ERROR;
    uint32_t preselections_count = 0;
    mp4d_atom_t prsl;
    uint32_t i;

    while ((mp4d_err = mp4d_find_atom((mp4d_atom_t*)atom, "prsl", preselections_count, &prsl)) == 0)
    {
        preselections_count++;
    }

    /* mp4d_find_atom() returns MP4D_E_ATOM_UNKNOWN when no more child atoms can be found
       and other error when there's not enough data in the buffer */
    if (mp4d_err != MP4D_E_ATOM_UNKNOWN)
    {
        BAIL(ALPS_RET_E_BUFF_TOO_SMALL);
    }

    /* realloc memory only when there's more preselections than before */
    if (preselections_count > atom->preselections_count)
    {
        alps_mp4dmx_free_grpl(atom);
        atom->preselections = malloc(preselections_count * sizeof(atom->preselections[0]));
        memset(atom->preselections, 0, preselections_count * sizeof(atom->preselections[0]));
    }

    atom->preselections_count = preselections_count;
    for (i = 0; i < atom->preselections_count; i++)
    {
        mp4d_atom_t prsl;
        /* return value was checked in the first loop so can be safely ignored here */
        mp4d_find_atom((mp4d_atom_t*)atom, "prsl", i, &prsl);
        CHECK_RET_AND_BAIL(alps_mp4dmx_create_preselection(&prsl, &atom->preselections[i]));
    }

bail:
    if (ret != ALPS_RET_OK)
    {
        alps_mp4dmx_free_grpl(atom);
    }

    return ret;
}

/**
 * @brief Constructs a meta struct by parsing meta atom it contains
 * @param[in] atom pointer to the meta struct that contains the meta atom
 * @return \ref ALPS_RET_E_BUFF_TOO_SMALL when given buffer's size is too small to hold the whole atom, \ref ALPS_RET_E_PARSE
 * when the atom is malformed, \ref ALPS_RET_OK when everything's all right
 */
static alps_ret alps_mp4dmx_parse_meta(
    alps_mp4dmx_meta *atom
)
{
    alps_ret ret = ALPS_RET_OK;

    CHECK_RET_AND_BAIL(alps_mp4dmx_parse_full_box((alps_mp4dmx_full_box *)atom));

    CHECK_MP4D_ERR_AND_BAIL(mp4d_find_atom((mp4d_atom_t*)atom, "grpl", 0, (mp4d_atom_t*)&atom->grpl), ALPS_RET_E_PARSE);

    CHECK_RET_AND_BAIL(alps_mp4dmx_parse_grpl(&atom->grpl));

bail:
    return ret;
}

alps_ret alps_mp4dmx_process_buffer(
    alps_mp4dmx   *dmx,
    unsigned char *buffer,
    size_t         size)
{
    alps_ret ret = ALPS_RET_OK;
    mp4d_atom_t atom = {0};
    uint64_t box_size = 0;
    uint32_t i;

    CHECK_EXPR_AND_BAIL(!dmx || !buffer, ALPS_RET_E_INVALID_ARG);

    dmx->segment_buf = buffer;
    dmx->segment_buf_pos = 0;

    /* find the moov or moof box */
    MP4D_FOURCC_ASSIGN(atom.type, "----");
    while (dmx->segment_buf_pos < size)
    {
        CHECK_MP4D_ERR_AND_BAIL(mp4d_parse_atom_header(buffer + dmx->segment_buf_pos, size - dmx->segment_buf_pos, &atom), ALPS_RET_E_PARSE);

        if (MP4D_FOURCC_EQ(atom.type, "moov") || MP4D_FOURCC_EQ(atom.type, "moof"))
        {
            CHECK_MP4D_ERR_AND_BAIL(
                mp4d_demuxer_parse(dmx->mp4d_demuxer, buffer + dmx->segment_buf_pos, size - dmx->segment_buf_pos, 1, 0, &box_size),
                ALPS_RET_E_PARSE);

            if (MP4D_FOURCC_EQ(atom.type, "moov"))
            {
                /* get movie and ac-4 track info */
                CHECK_MP4D_ERR_AND_BAIL(mp4d_demuxer_get_movie_info(dmx->mp4d_demuxer, &dmx->movie_info), ALPS_RET_E_PARSE);
                for (i = 0, dmx->ac4_track_info.track_id = 0; i < dmx->movie_info.num_streams; i++, dmx->ac4_track_info.track_id = 0)
                {
                    CHECK_MP4D_ERR_AND_BAIL(mp4d_demuxer_get_stream_info(dmx->mp4d_demuxer, i, &dmx->ac4_track_info), ALPS_RET_E_PARSE);
                    if (MP4D_FOURCC_EQ(dmx->ac4_track_info.codec, "ac-4"))
                    {
                        break;
                    }
                }
            }

            /* check if ac-4 track is known */
            CHECK_EXPR_AND_BAIL(dmx->ac4_track_info.track_id == 0, ALPS_RET_E_NO_AC4_TRACK);

            CHECK_MP4D_ERR_AND_BAIL(
                mp4d_trackreader_init_segment(
                    dmx->mp4d_track_reader,
                    dmx->mp4d_demuxer,
                    dmx->ac4_track_info.track_id,
                    dmx->movie_info.time_scale,
                    dmx->ac4_track_info.time_scale, 0),
                ALPS_RET_E_PARSE);
        }
        else if (MP4D_FOURCC_EQ(atom.type, "meta"))
        {
            alps_mp4dmx_meta meta;

            /* get list of preselections from meta box */
            memset(&meta, 0 , sizeof(meta));
            memcpy(&meta, &atom, sizeof(atom));
            CHECK_RET_AND_BAIL(alps_mp4dmx_parse_meta(&meta));
            dmx->preselections_count = meta.grpl.preselections_count;
            dmx->preselections = meta.grpl.preselections;
        }

        dmx->segment_buf_pos += atom.header + atom.size;
    }

bail:
    return ret;
}

alps_ret alps_mp4dmx_next_sample(
    alps_mp4dmx    *dmx,
    unsigned char **data,
    size_t         *size)
{
    alps_ret ret = ALPS_RET_OK;
    mp4d_sampleref_t sample;

    CHECK_EXPR_AND_BAIL(!dmx || !data || !size, ALPS_RET_E_INVALID_ARG);
    CHECK_EXPR_AND_BAIL(!dmx->segment_buf, ALPS_RET_E_PARSE);

    CHECK_MP4D_ERR_AND_BAIL(mp4d_trackreader_next_sample(dmx->mp4d_track_reader, &sample), ALPS_RET_E_PARSE);

    *data = dmx->segment_buf + sample.pos;
    *size = sample.size;

bail:
    return ret;
}

alps_ret alps_mp4dmx_get_preselections(
    alps_mp4dmx        *dmx,
    alps_mp4dmx_preselection **preselections,
    size_t             *count)
{
    alps_ret ret = ALPS_RET_OK;

    CHECK_EXPR_AND_BAIL(!dmx || !preselections || !count, ALPS_RET_E_INVALID_ARG);

    *preselections = dmx->preselections;
    *count = (size_t)dmx->preselections_count;

bail:
    return ret;
}
