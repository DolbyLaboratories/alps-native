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

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmocka.h>

#include "mp4d_nav.h"
#include "mp4d_types.h"

#include "dlb_alps_native/alps_mp4dmx/alps_mp4dmx.h"

#define SIZE_OF_ALPS_MP4DMX_STRUCT (120)
#define MP4D_DMX_STATIC_MEM_SIZE (1)
#define MP4D_DMX_DYNAMIC_MEM_SIZE (2)
#define MP4D_TR_STATIC_MEM_SIZE (4)
#define MP4D_TR_DYNAMIC_MEM_SIZE (8)
#define TOTAL_MEM_SIZE (SIZE_OF_ALPS_MP4DMX_STRUCT + MP4D_DMX_STATIC_MEM_SIZE + MP4D_DMX_DYNAMIC_MEM_SIZE + MP4D_TR_STATIC_MEM_SIZE + MP4D_TR_DYNAMIC_MEM_SIZE)

#define FAKE_DATA_PTR ((void*)0x1234)
#define BUF_SIZE_TOO_SMALL ((uint64_t)-1)
#define INVALID_PRESELECTION_TAG (-1)

#define TAG_PRESENT_FLAG (0x1000)
#define PRIO_PRESENT_FLAG (0x2000)

#define MP4D_RET_ERR 111

/* FUNCTIONS' MOCKS */

int __wrap_mp4d_demuxer_query_mem(
    uint64_t *static_mem_size,
    uint64_t *dynamic_mem_size)
{
    *static_mem_size = mock();
    *dynamic_mem_size = mock();

    return mock();
}

int __wrap_mp4d_trackreader_query_mem(
    uint64_t *static_mem_size,
    uint64_t *dynamic_mem_size)
{
    *static_mem_size = mock();
    *dynamic_mem_size = mock();

    return mock();
}

int __wrap_mp4d_demuxer_init(
    void *dmx,
    void *static_mem,
    void *dynamic_mem)
{
    (void)dmx;
    (void)static_mem;
    (void)dynamic_mem;

    return mock();
}

int __wrap_mp4d_trackreader_init(
    void *tr,
    void *static_mem,
    void *dynamic_mem
)
{
    (void)tr;
    (void)static_mem;
    (void)dynamic_mem;

    return mock();
}

int __wrap_mp4d_trackreader_set_tenc(void *p_tr, uint32_t default_algorithmID, uint8_t default_iv_size, uint8_t* default_kid)
{
    (void)p_tr;
    (void)default_algorithmID;
    (void)default_iv_size;
    (void)default_kid;

    return mock();
}

int __wrap_mp4d_parse_atom_header(
    const unsigned char *buffer,
    uint64_t size,
    mp4d_atom_t *atom)
{
    static const uint32_t header_size = 8;
    char *type;

    (void)buffer;
    (void)size;

    atom->header = header_size;
    atom->size = mock_type(uint64_t) - header_size;
    atom->p_data = FAKE_DATA_PTR;
    type = mock_ptr_type(char*);
    MP4D_FOURCC_ASSIGN(atom->type, type);

    return mock();
}

int __wrap_mp4d_demuxer_parse(
    void *dmux,
    const unsigned char *buffer,
    uint64_t size,
    int is_eof,
    uint64_t ref_offs,
    uint64_t *box_size_out)
{
    (void)dmux;
    (void)buffer;
    (void)size;
    (void)is_eof;
    (void)ref_offs;
    (void)box_size_out;

    return mock();
}

mp4d_error_t __wrap_mp4d_find_atom(
    mp4d_atom_t *atom,
    const char *type,
    uint32_t occurence,
    mp4d_atom_t *child)
{
    (void)atom;
    (void)type;
    (void)occurence;

    assert_non_null(child);
    child->p_data = mock_ptr_type(unsigned char *);

    return mock_type(mp4d_error_t);
}

mp4d_buffer_t __wrap_mp4d_atom_to_buffer(const mp4d_atom_t * p_atom)
{
    (void)p_atom;

    return *mock_ptr_type(mp4d_buffer_t*);
}

uint8_t __wrap_mp4d_read_u8(mp4d_buffer_t *p)
{
    if (mock())
    {
        assert_non_null(p);
        p->size = mock_type(uint64_t);
    }

    return mock_type(uint8_t);
}

uint8_t __wrap_mp4d_read_u16(mp4d_buffer_t *p)
{
    if (mock())
    {
        assert_non_null(p);
        p->size = mock_type(uint64_t);
    }

    return mock_type(uint16_t);
}

uint32_t __wrap_mp4d_read_u24(mp4d_buffer_t *p)
{
    if (mock())
    {
        assert_non_null(p);
        p->size = mock_type(uint64_t);
    }

    return mock_type(uint32_t);
}

uint32_t __wrap_mp4d_read_u32(mp4d_buffer_t *p)
{
    if (mock())
    {
        assert_non_null(p);
        p->size = mock_type(uint64_t);
    }

    return mock_type(uint32_t);
}

void __wrap_mp4d_skip_bytes(mp4d_buffer_t * p, uint64_t size)
{
    (void)p;
    (void)size;
}

int __wrap_mp4d_demuxer_get_movie_info(void *p_demuxer, mp4d_movie_info_t *p_movie_info)
{
    (void)p_demuxer;

    p_movie_info->num_streams = mock();

    return mock();
}

int __wrap_mp4d_demuxer_get_stream_info(void *p_demuxer, uint32_t stream_num, mp4d_stream_info_t *p_stream_info)
{
    char *codec;
    (void)p_demuxer;
    (void)stream_num;

    p_stream_info->track_id = mock();
    codec = mock_ptr_type(char *);
    MP4D_FOURCC_ASSIGN(p_stream_info->codec, codec);

    return mock();
}

int __wrap_mp4d_trackreader_init_segment(
    void *trackreader_ptr,
    void *demuxer_ptr,
    uint32_t track_ID,
    uint32_t movie_time_scale,
    uint32_t media_time_scale,
    const uint64_t *abs_time_offs)
{
    (void)trackreader_ptr;
    (void)demuxer_ptr;
    (void)track_ID;
    (void)movie_time_scale;
    (void)media_time_scale;
    (void)abs_time_offs;

    return mock();
}

int __wrap_mp4d_trackreader_next_sample(void *trackreader_ptr, mp4d_sampleref_t *sample_ptr_out)
{
    (void)trackreader_ptr;
    (void)sample_ptr_out;

    assert_non_null(sample_ptr_out);
    sample_ptr_out->pos = mock_type(uint64_t);
    sample_ptr_out->size = mock_type(uint32_t);

    return mock();
}

/* HELPER FUNCTIONS */

static alps_mp4dmx* init_demuxer(unsigned char *mem)
{
    alps_ret ret = ALPS_RET_OK;
    alps_mp4dmx *dmx = NULL;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_init, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_set_tenc, MP4D_NO_ERROR);

    ret = alps_mp4dmx_init(&dmx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    return dmx;
}

static void set_segment_buf(alps_mp4dmx *dmx, unsigned char *segment_buf)
{
    const int MEMBER_OFFSET_SEGMENT_BUF = 16;
    *((unsigned char**)((char*)dmx + MEMBER_OFFSET_SEGMENT_BUF)) = segment_buf;
}

static void set_segment_buf_pos(alps_mp4dmx *dmx, size_t segment_buf_pos)
{
    const int MEMBER_OFFSET_SEGMENT_BUF_POS = 24;
    *((size_t*)((char*)dmx + MEMBER_OFFSET_SEGMENT_BUF_POS)) = segment_buf_pos;
}

static void will_return_parse_atom_header(uint64_t atom_size, char *type, int ret_val)
{
    will_return(__wrap_mp4d_parse_atom_header, atom_size);
    will_return(__wrap_mp4d_parse_atom_header, type);
    will_return(__wrap_mp4d_parse_atom_header, ret_val);
}

static void will_return_find_atom(unsigned char *data, int ret_val)
{
    will_return(__wrap_mp4d_find_atom, data);
    will_return(__wrap_mp4d_find_atom, ret_val);
}

static void will_return_mp4d_read_u16_change_buf_size(uint64_t size, uint16_t ret_val)
{
    will_return(__wrap_mp4d_read_u16, 1);
    will_return(__wrap_mp4d_read_u16, size);
    will_return(__wrap_mp4d_read_u16, ret_val);
}

static void will_return_mp4d_read_u24_change_buf_size(uint64_t size, uint32_t ret_val)
{
    will_return(__wrap_mp4d_read_u24, 1);
    will_return(__wrap_mp4d_read_u24, size);
    will_return(__wrap_mp4d_read_u24, ret_val);
}

static void will_return_mp4d_read_u32_change_buf_size(uint64_t size, uint32_t ret_val)
{
    will_return(__wrap_mp4d_read_u32, 1);
    will_return(__wrap_mp4d_read_u32, size);
    will_return(__wrap_mp4d_read_u32, ret_val);
}

static void will_return_mp4d_read_u8(uint8_t ret_val)
{
    will_return(__wrap_mp4d_read_u8, 0);
    will_return(__wrap_mp4d_read_u8, ret_val);
}

static void will_return_mp4d_read_u16(uint16_t ret_val)
{
    will_return(__wrap_mp4d_read_u16, 0);
    will_return(__wrap_mp4d_read_u16, ret_val);
}

static void will_return_mp4d_read_u24(uint32_t ret_val)
{
    will_return(__wrap_mp4d_read_u24, 0);
    will_return(__wrap_mp4d_read_u24, ret_val);
}

static void will_return_mp4d_read_u32(uint32_t ret_val)
{
    will_return(__wrap_mp4d_read_u32, 0);
    will_return(__wrap_mp4d_read_u32, ret_val);
}

static void will_return_parse_full_box(mp4d_buffer_t *buf, uint8_t read_u8_ret, uint32_t read_u24_ret)
{
    will_return(__wrap_mp4d_atom_to_buffer, buf);
    will_return_mp4d_read_u8(read_u8_ret);
    will_return_mp4d_read_u24(read_u24_ret);
}

static void will_return_create_preselection(unsigned int flags, mp4d_buffer_t atom_bufs[3])
{
    /* mocks for whole alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs[0]);
    /* parse flags */
    will_return_mp4d_read_u24(flags);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32(1);
    /* find elng box; data and size do not matter */
    if (flags & 0x2000) {
        // Selection priority
        will_return_mp4d_read_u8(1);
    }
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert elng box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs[1]);
    /* find labl box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert labl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs[2]);
    /* parse is_group_label */
    will_return_mp4d_read_u24(0x0);
    /* parse label_id */
    will_return_mp4d_read_u16(0x1);
    /* no kind box found */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);

    /* ARDI box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_atom_to_buffer, "");
    /* audio rendering indication */
    will_return_mp4d_read_u8(1);
}

static void will_return_mp4d_demuxer_get_movie_info(uint32_t num_streams, int ret_value)
{
    will_return(__wrap_mp4d_demuxer_get_movie_info, num_streams);
    will_return(__wrap_mp4d_demuxer_get_movie_info, ret_value);
}

static void will_return_mp4d_demuxer_get_stream_info(uint32_t track_id, const char *codec, int ret_value)
{
    will_return(__wrap_mp4d_demuxer_get_stream_info, track_id);
    will_return(__wrap_mp4d_demuxer_get_stream_info, codec);
    will_return(__wrap_mp4d_demuxer_get_stream_info, ret_value);
}

static void will_return_mp4d_trackreader_next_sample(uint64_t position, uint32_t size, int ret_value)
{
    will_return(__wrap_mp4d_trackreader_next_sample, position);
    will_return(__wrap_mp4d_trackreader_next_sample, size);
    will_return(__wrap_mp4d_trackreader_next_sample, ret_value);
}

/* TESTS */

static void query_mem__all_good__ok(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    size_t mem_size = 0;

    (void)state;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_NO_ERROR);

    ret = alps_mp4dmx_query_mem(&mem_size);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_int_equal(mem_size, TOTAL_MEM_SIZE);
}

static void query_mem__null_ptr__invalid_arg(void **state)
{
    alps_ret ret = ALPS_RET_OK;

    (void)state;

    ret = alps_mp4dmx_query_mem(NULL);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
}

static void query_mem__mp4d_demuxer_error__undefined_error(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    size_t mem_size = 0;

    (void)state;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_RET_ERR);

    ret = alps_mp4dmx_query_mem(&mem_size);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void query_mem__mp4d_trackreader_error__undefined_error(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    size_t mem_size = 0;

    (void)state;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_RET_ERR);

    ret = alps_mp4dmx_query_mem(&mem_size);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void init__all_good__ok(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];

    (void)state;

    init_demuxer(mem);
}

static void init__dmx_query_mem_error__undefined_error(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    alps_mp4dmx *dmx = NULL;
    unsigned char mem[TOTAL_MEM_SIZE];

    (void)state;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_RET_ERR);

    ret = alps_mp4dmx_init(&dmx, mem);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void init__dmx_init_error__undefined_error(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    alps_mp4dmx *dmx = NULL;
    unsigned char mem[TOTAL_MEM_SIZE];

    (void)state;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_init, MP4D_RET_ERR);

    ret = alps_mp4dmx_init(&dmx, mem);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void init__tr_query_mem_error__undefined_error(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    alps_mp4dmx *dmx = NULL;
    unsigned char mem[TOTAL_MEM_SIZE];

    (void)state;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_init, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_RET_ERR);

    ret = alps_mp4dmx_init(&dmx, mem);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void init__tr_init_error__undefined_error(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    alps_mp4dmx *dmx = NULL;
    unsigned char mem[TOTAL_MEM_SIZE];

    (void)state;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_init, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init, MP4D_RET_ERR);

    ret = alps_mp4dmx_init(&dmx, mem);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void init__tr_set_tenc_error__undefined_error(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    alps_mp4dmx *dmx = NULL;
    unsigned char mem[TOTAL_MEM_SIZE];

    (void)state;

    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_DMX_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_demuxer_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_init, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_STATIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_TR_DYNAMIC_MEM_SIZE);
    will_return(__wrap_mp4d_trackreader_query_mem, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_set_tenc, MP4D_RET_ERR);

    ret = alps_mp4dmx_init(&dmx, mem);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void destroy__all_good__ok(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;

    (void)state;

    dmx = init_demuxer(mem);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__dmx_not_initialized__invalid_arg(void **state)
{
    alps_ret ret = ALPS_RET_OK;
    const size_t buf_size = 2;
    unsigned char buffer[buf_size];

    (void)state;

    ret = alps_mp4dmx_process_buffer(NULL, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
}

static void process_buffer__buffer_null__invalid_arg(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret = ALPS_RET_OK;

    (void)state;

    dmx = init_demuxer(mem);

    ret = alps_mp4dmx_process_buffer(dmx, NULL, 0);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__moov_present__segment_parsed(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];

    (void)state;

    dmx = init_demuxer(mem);

    /* find the moov box */
    will_return_parse_atom_header(buf_size, "moov", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);
    /* get movie and ac-4 track info */
    will_return_mp4d_demuxer_get_movie_info(2, MP4D_NO_ERROR);
    will_return_mp4d_demuxer_get_stream_info(1, "hevc", MP4D_NO_ERROR);
    will_return_mp4d_demuxer_get_stream_info(2, "ac-4", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init_segment, MP4D_NO_ERROR);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__parse_moov_and_moof__fragment_parsed(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];

    (void)state;

    dmx = init_demuxer(mem);

    /* find the moov box */
    will_return_parse_atom_header(buf_size, "moov", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);
    /* get movie and ac-4 track info */
    will_return_mp4d_demuxer_get_movie_info(2, MP4D_NO_ERROR);
    will_return_mp4d_demuxer_get_stream_info(1, "hevc", MP4D_NO_ERROR);
    will_return_mp4d_demuxer_get_stream_info(2, "ac-4", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init_segment, MP4D_NO_ERROR);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    /* find the moof box */
    will_return_parse_atom_header(buf_size, "moof", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init_segment, MP4D_NO_ERROR);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__moov_moof_and_meta_present__preselections_available(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    mp4d_buffer_t atom_bufs_pl[3] = {
        {(unsigned char *)"1", 2, NULL},
        {(unsigned char *)"pl", 3, NULL},
        {(unsigned char *)"Polish", 7, NULL}
    };
    mp4d_buffer_t atom_bufs_en[3] = {
        {(unsigned char *)"2", 2, NULL},
        {(unsigned char *)"en", 3, NULL},
        {(unsigned char *)"English", 8, NULL}
    };
    mp4d_buffer_t atom_bufs_es[3] = {
        {(unsigned char *)"3", 2, NULL},
        {(unsigned char *)"es", 3, NULL},
        {(unsigned char *)"Spanish", 8, NULL}
    };
    alps_ret ret;
    alps_mp4dmx_preselection *parsed_preselections = NULL;
    size_t parsed_preselections_count = 0;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the moov box */
    will_return_parse_atom_header(buf_size, "moov", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);
    /* get movie and ac-4 track info */
    will_return_mp4d_demuxer_get_movie_info(2, MP4D_NO_ERROR);
    will_return_mp4d_demuxer_get_stream_info(1, "hevc", MP4D_NO_ERROR);
    will_return_mp4d_demuxer_get_stream_info(2, "ac-4", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init_segment, MP4D_NO_ERROR);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    /* find the moof box */
    will_return_parse_atom_header(buf_size, "moof", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init_segment, MP4D_NO_ERROR);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_create_preselection(TAG_PRESENT_FLAG | PRIO_PRESENT_FLAG, atom_bufs_pl);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_create_preselection(TAG_PRESENT_FLAG, atom_bufs_en);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_create_preselection(TAG_PRESENT_FLAG | PRIO_PRESENT_FLAG, atom_bufs_es);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_mp4dmx_get_preselections(dmx, &parsed_preselections, &parsed_preselections_count);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_int_equal(parsed_preselections_count, 3);
    assert_int_equal(parsed_preselections[0].preselection_tag, atoi((char*)atom_bufs_pl[0].p_data));
    assert_string_equal(parsed_preselections[0].extended_language, (char*)atom_bufs_pl[1].p_data);
    assert_string_equal(parsed_preselections[0].labels[0].label, (char*)atom_bufs_pl[2].p_data);
    assert_int_equal(parsed_preselections[1].preselection_tag, atoi((char*)atom_bufs_en[0].p_data));
    assert_string_equal(parsed_preselections[1].extended_language, (char*)atom_bufs_en[1].p_data);
    assert_string_equal(parsed_preselections[1].labels[0].label, (char*)atom_bufs_en[2].p_data);
    assert_int_equal(parsed_preselections[2].preselection_tag, atoi((char*)atom_bufs_es[0].p_data));
    assert_string_equal(parsed_preselections[2].extended_language, (char*)atom_bufs_es[1].p_data);
    assert_string_equal(parsed_preselections[2].labels[0].label, (char*)atom_bufs_es[2].p_data);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__only_ftyp_box_found__ok_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];

    (void)state;

    dmx = init_demuxer(mem);

    /* find the moov box */
    will_return_parse_atom_header(buf_size, "ftyp", MP4D_NO_ERROR);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__moof_present_but_no_track_id__no_movie_info_error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the moof box */
    will_return_parse_atom_header(buf_size, "moof", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_NO_AC4_TRACK);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__no_prsl_boxes__preselections_unavailable(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    alps_ret ret;
    alps_mp4dmx_preselection *parsed_preselections = NULL;
    size_t parsed_preselections_count = 0;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* there are no prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_mp4dmx_get_preselections(dmx, &parsed_preselections, &parsed_preselections_count);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_int_equal(parsed_preselections_count, 0);
    assert_ptr_equal(parsed_preselections, NULL);

    alps_mp4dmx_destroy(dmx);
}

typedef struct return_values_t
{
    mp4d_error_t mock;
    alps_ret expected;
} return_values;

static void process_buffer__error_finding_moov__error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    unsigned long i;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    return_values ret_values[] = {
        {MP4D_E_WRONG_ARGUMENT, ALPS_RET_E_INVALID_ARG},
        {MP4D_E_BUFFER_TOO_SMALL, ALPS_RET_E_BUFF_TOO_SMALL},
        {MP4D_E_INVALID_ATOM, ALPS_RET_E_PARSE}
    };

    (void)state;

    dmx = init_demuxer(mem);

    for (i = 0; i < sizeof(ret_values) / sizeof(ret_values[0]); i++)
    {
        will_return_parse_atom_header(buf_size, "moov", ret_values[i].mock);
        ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
        assert_int_equal(ret, ret_values[i].expected);
    }

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__mp4d_error_parsing_buffer__error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    unsigned long i;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    return_values ret_values[] = {
        {MP4D_E_WRONG_ARGUMENT, ALPS_RET_E_INVALID_ARG},
        {MP4D_E_BUFFER_TOO_SMALL, ALPS_RET_E_BUFF_TOO_SMALL}
    };

    (void)state;

    dmx = init_demuxer(mem);

    for (i = 0; i < sizeof(ret_values) / sizeof(ret_values[0]); i++)
    {
        /* find the moof box */
        will_return_parse_atom_header(buf_size, "moof", MP4D_NO_ERROR);

        /* wrong argument */
        will_return(__wrap_mp4d_demuxer_parse, ret_values[i].mock);
        ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
        assert_int_equal(ret, ret_values[i].expected);
    }

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__error_counting_prsl__buff_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_INVALID_ATOM);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__prsl_buf_contains_no_data__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    mp4d_buffer_t too_small_buf = {NULL, (uint64_t)-1, NULL};
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &too_small_buf);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__prsl_buf_contains_just_version__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_buf);
    /* parse flags */
    will_return_mp4d_read_u24_change_buf_size(BUF_SIZE_TOO_SMALL, 0x0);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__prsl_buf_size_up_to_flags__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_buf);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32_change_buf_size(BUF_SIZE_TOO_SMALL, 1);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__prsl_buf_size_up_to_num_entities__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_buf);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32_change_buf_size(BUF_SIZE_TOO_SMALL, 1);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__prsl_buf_size_up_to_entity_ids__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_buf);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32_change_buf_size(BUF_SIZE_TOO_SMALL, 1);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__prsl_buf_size_up_to_prsl_tag__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_buf);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32(1);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__cannot_find_elng_box__parse_error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    mp4d_buffer_t atom_bufs_pl[3] = {
        {(unsigned char *)"1", 2, NULL},
        {(unsigned char *)"pl", 3, NULL},
        {(unsigned char *)"Polish", 7, NULL}
    };
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[0]);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32(1);
    /* find elng box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_INVALID_ATOM);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_PARSE);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__elng_box_too_small__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    mp4d_buffer_t atom_bufs_pl[3] = {
        {(unsigned char *)"1", 2, NULL},
        {(unsigned char *)"pl", 3, NULL},
        {(unsigned char *)"Polish", 7, NULL}
    };
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[0]);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32(1);
    /* find elng box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert elng box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_buf);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__cannot_find_labl_box__parse_error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    mp4d_buffer_t atom_bufs_pl[3] = {
        {(unsigned char *)"1", 2, NULL},
        {(unsigned char *)"pl", 3, NULL},
        {(unsigned char *)"Polish", 7, NULL}
    };
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[0]);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32(1);
    /* find elng box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert elng box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[1]);
    /* find labl box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_INVALID_ATOM);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_PARSE);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__labl_box_too_small__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    mp4d_buffer_t atom_bufs_pl[3] = {
        {(unsigned char *)"1", 2, NULL},
        {(unsigned char *)"pl", 3, NULL},
        {(unsigned char *)"Polish", 7, NULL}
    };
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[0]);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32(1);
    /* find elng box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert elng box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[1]);
    /* find labl box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert labl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[2]);
    /* parse is_group_label */
    will_return_mp4d_read_u24_change_buf_size(BUF_SIZE_TOO_SMALL, 0);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__labl_buf_size_up_to_label__buf_too_small_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    mp4d_buffer_t atom_bufs_pl[3] = {
        {(unsigned char *)"1", 2, NULL},
        {(unsigned char *)"pl", 3, NULL},
        {(unsigned char *)"Polish", 7, NULL}
    };
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[0]);
    /* parse flags */
    will_return_mp4d_read_u24(TAG_PRESENT_FLAG);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32(1);
    /* find elng box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert elng box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[1]);
    /* find labl box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert labl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_buf);
    /* parse is_group_label */
    will_return_mp4d_read_u24(0);
    will_return_mp4d_read_u16_change_buf_size(BUF_SIZE_TOO_SMALL, 0);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__no_prsl_tag__preselection_tag_not_valid(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];
    mp4d_buffer_t atom_buf = {0};
    mp4d_buffer_t atom_bufs_pl[3] = {
        {(unsigned char *)"1", 2, NULL},
        {(unsigned char *)"pl", 3, NULL},
        {(unsigned char *)"pl\0Polish", 10, NULL}
    };
    alps_ret ret;
    alps_mp4dmx_preselection *parsed_preselections = NULL;
    size_t parsed_preselections_count = 0;

    (void)state;

    dmx = init_demuxer(mem);

    /* find the meta box */
    will_return_parse_atom_header(buf_size, "meta", MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_meta() */
    will_return_parse_full_box(&atom_buf, 0, 0);
    /* find grpl box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_parse_grpl() */
    /* find all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    /* iterate over all prsl boxes */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* alps_mp4dmx_create_preselection() */
    /* convert prsl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[0]);
    /* parse flags */
    will_return_mp4d_read_u24(0x0);
    /* parse num_entities_in_group */
    will_return_mp4d_read_u32(1);
    /* parse all entity_ids */
    will_return_mp4d_read_u32(1);
    /* find elng box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert elng box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[1]);
    /* find labl box; data and size do not matter */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_E_ATOM_UNKNOWN);
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    /* convert labl box to buffer */
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[2]);
    /* parse is_group_label */
    will_return_mp4d_read_u24(0);
    /* parse label_id */
    will_return_mp4d_read_u16(1);
    /* no kind box found */
    will_return_find_atom(NULL, MP4D_E_ATOM_UNKNOWN);
    /* ARDI box */
    will_return_find_atom(FAKE_DATA_PTR, MP4D_NO_ERROR);
    will_return(__wrap_mp4d_atom_to_buffer, &atom_bufs_pl[0]);
    /* audio rendering indication */
    will_return_mp4d_read_u8(1);
    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_mp4dmx_get_preselections(dmx, &parsed_preselections, &parsed_preselections_count);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_int_equal(parsed_preselections_count, 1);
    assert_int_equal(parsed_preselections[0].preselection_tag, INVALID_PRESELECTION_TAG);
    assert_string_equal(parsed_preselections[0].extended_language, (char*)atom_bufs_pl[1].p_data);
    assert_string_equal(parsed_preselections[0].labels[0].label, (char*)atom_bufs_pl[2].p_data); 

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__error_getting_movie_info__parse_error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];

    (void)state;

    dmx = init_demuxer(mem);

    /* find the moov box */
    will_return_parse_atom_header(buf_size, "moov", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);
    /* get movie and ac-4 track info */
    will_return_mp4d_demuxer_get_movie_info(1, MP4D_E_INVALID_ATOM);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_PARSE);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__error_getting_stream_info__parse_error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];

    (void)state;

    dmx = init_demuxer(mem);

    /* find the moov box */
    will_return_parse_atom_header(buf_size, "moov", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);
    /* get movie and ac-4 track info */
    will_return_mp4d_demuxer_get_movie_info(1, MP4D_NO_ERROR);
    will_return_mp4d_demuxer_get_stream_info(1, "ac-4", MP4D_E_TRACK_NOT_FOUND);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_PARSE);

    alps_mp4dmx_destroy(dmx);
}

static void process_buffer__error_initializing_segment__parse_error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret;
    const size_t buf_size = 128;
    unsigned char buffer[buf_size];

    (void)state;

    dmx = init_demuxer(mem);

    /* find the moov box */
    will_return_parse_atom_header(buf_size, "moov", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_demuxer_parse, MP4D_NO_ERROR);
    /* get movie and ac-4 track info */
    will_return_mp4d_demuxer_get_movie_info(1, MP4D_NO_ERROR);
    will_return_mp4d_demuxer_get_stream_info(2, "ac-4", MP4D_NO_ERROR);
    will_return(__wrap_mp4d_trackreader_init_segment, MP4D_E_UNSUPPRTED_FORMAT);

    ret = alps_mp4dmx_process_buffer(dmx, buffer, buf_size);
    assert_int_equal(ret, ALPS_RET_E_PARSE);

    alps_mp4dmx_destroy(dmx);
}

static void next_sample__args_not_valid__invalid_arg_error_returned(void **state)
{
    alps_ret ret;

    (void)state;

    ret = alps_mp4dmx_next_sample(NULL, FAKE_DATA_PTR, FAKE_DATA_PTR);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);

    ret = alps_mp4dmx_next_sample(FAKE_DATA_PTR, NULL, FAKE_DATA_PTR);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);

    ret = alps_mp4dmx_next_sample(FAKE_DATA_PTR, FAKE_DATA_PTR, NULL);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
}

static void next_sample__segment_not_parsed__parse_error_returned(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);

    ret = alps_mp4dmx_next_sample(dmx, FAKE_DATA_PTR, FAKE_DATA_PTR);
    assert_int_equal(ret, ALPS_RET_E_PARSE);
}

static void next_sample__all_ok__data_and_size_set_correctly(void **state)
{
    unsigned char mem[TOTAL_MEM_SIZE];
    alps_mp4dmx *dmx;
    unsigned char *segment_buf = FAKE_DATA_PTR;
    size_t segment_buf_pos = 4321;
    const uint64_t sample_pos = 5678;
    const uint32_t sample_size = 9876;
    unsigned char *data = NULL;
    size_t size = 0;
    alps_ret ret;

    (void)state;

    dmx = init_demuxer(mem);
    set_segment_buf(dmx, segment_buf);
    set_segment_buf_pos(dmx, segment_buf_pos);

    will_return_mp4d_trackreader_next_sample(sample_pos, sample_size, MP4D_NO_ERROR);

    ret = alps_mp4dmx_next_sample(dmx, &data, &size);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_ptr_equal(data, (char*)segment_buf + sample_pos);
    assert_int_equal(size, sample_size);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(query_mem__all_good__ok),
        cmocka_unit_test(query_mem__null_ptr__invalid_arg),
        cmocka_unit_test(query_mem__mp4d_demuxer_error__undefined_error),
        cmocka_unit_test(query_mem__mp4d_trackreader_error__undefined_error),
        cmocka_unit_test(init__all_good__ok),
        cmocka_unit_test(init__dmx_query_mem_error__undefined_error),
        cmocka_unit_test(init__dmx_init_error__undefined_error),
        cmocka_unit_test(init__tr_query_mem_error__undefined_error),
        cmocka_unit_test(init__tr_init_error__undefined_error),
        cmocka_unit_test(init__tr_set_tenc_error__undefined_error),
        cmocka_unit_test(destroy__all_good__ok),
        cmocka_unit_test(process_buffer__dmx_not_initialized__invalid_arg),
        cmocka_unit_test(process_buffer__buffer_null__invalid_arg),
        cmocka_unit_test(process_buffer__moov_present__segment_parsed),
        cmocka_unit_test(process_buffer__parse_moov_and_moof__fragment_parsed),
        cmocka_unit_test(process_buffer__moov_moof_and_meta_present__preselections_available),
        cmocka_unit_test(process_buffer__only_ftyp_box_found__ok_returned),
        cmocka_unit_test(process_buffer__moof_present_but_no_track_id__no_movie_info_error_returned),
        cmocka_unit_test(process_buffer__no_prsl_boxes__preselections_unavailable),
        cmocka_unit_test(process_buffer__error_finding_moov__error_returned),
        cmocka_unit_test(process_buffer__mp4d_error_parsing_buffer__error_returned),
        cmocka_unit_test(process_buffer__error_counting_prsl__buff_too_small_returned),
        cmocka_unit_test(process_buffer__prsl_buf_contains_no_data__buf_too_small_returned),
        cmocka_unit_test(process_buffer__prsl_buf_contains_just_version__buf_too_small_returned),
        cmocka_unit_test(process_buffer__prsl_buf_size_up_to_flags__buf_too_small_returned),
        cmocka_unit_test(process_buffer__prsl_buf_size_up_to_num_entities__buf_too_small_returned),
        cmocka_unit_test(process_buffer__prsl_buf_size_up_to_entity_ids__buf_too_small_returned),
        cmocka_unit_test(process_buffer__prsl_buf_size_up_to_prsl_tag__buf_too_small_returned),
        cmocka_unit_test(process_buffer__cannot_find_elng_box__parse_error_returned),
        cmocka_unit_test(process_buffer__elng_box_too_small__buf_too_small_returned),
        cmocka_unit_test(process_buffer__cannot_find_labl_box__parse_error_returned),
        cmocka_unit_test(process_buffer__labl_box_too_small__buf_too_small_returned),
        cmocka_unit_test(process_buffer__labl_buf_size_up_to_label__buf_too_small_returned),
        cmocka_unit_test(process_buffer__no_prsl_tag__preselection_tag_not_valid),
        cmocka_unit_test(process_buffer__error_getting_movie_info__parse_error_returned),
        cmocka_unit_test(process_buffer__error_getting_stream_info__parse_error_returned),
        cmocka_unit_test(process_buffer__error_initializing_segment__parse_error_returned),
        cmocka_unit_test(next_sample__args_not_valid__invalid_arg_error_returned),
        cmocka_unit_test(next_sample__segment_not_parsed__parse_error_returned),
        cmocka_unit_test(next_sample__all_ok__data_and_size_set_correctly)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
