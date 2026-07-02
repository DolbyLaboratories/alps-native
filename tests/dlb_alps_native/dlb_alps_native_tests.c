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

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmocka.h>

#include "dlb_alps_native/dlb_alps_native.h"
#include "dlb_alps_native/alps_mp4dmx/alps_mp4dmx.h"

#define MP4DMX_MEM_SIZE (1234)
#define SIZEOF_ALPS_CTX (48)

#define PRESELECTION_1 {0, NULL, 1, "pl", NULL, 0, NULL,0,0,0,0,0}
#define PRESELECTION_2 {0, NULL, 2, "pl", NULL, 0, NULL,0,0,0,0,0}
#define PRESELECTION_3 {0, NULL, 3, "pl", NULL, 0, NULL,0,0,0,0,0}

/* FUNCTIONS' MOCKS */

alps_ret __wrap_alps_mp4dmx_query_mem(size_t *mem_size)
{
    *mem_size = MP4DMX_MEM_SIZE;

    return mock();
}

alps_ret __wrap_alps_mp4dmx_init(void **demuxer, void *mem)
{
    (void)demuxer;
    (void)mem;

    return mock();
}

void __wrap_alps_mp4dmx_destroy(void *dmx)
{
    (void)dmx;
}

alps_ret __wrap_alps_mp4dmx_process_buffer(void *dmx, unsigned char *buffer, size_t size)
{
    (void)dmx;

    check_expected_ptr(buffer);
    check_expected(size);

    return mock();
}

alps_ret __wrap_alps_mp4dmx_get_preselections(void *dmx, alps_mp4dmx_preselection **preselections, size_t *count)
{
    (void)dmx;

    *preselections = mock_ptr_type(alps_mp4dmx_preselection*);
    *count = mock_type(size_t);

    return mock();
}

alps_ret __wrap_alps_mp4dmx_next_sample(alps_mp4dmx *dmx, unsigned char **data, size_t *size)
{
    (void)dmx;

    *data = mock_type(unsigned char*);
    *size = mock_type(size_t);

    return mock();
}

alps_ret __wrap_alps_ac4parser_set_active_presentation_id(
    unsigned char *frame_buf, size_t size_in_bytes, int active_presentation_id)
{
    (void)frame_buf;
    (void)size_in_bytes;

    check_expected_ptr(frame_buf);
    check_expected(size_in_bytes);
    check_expected(active_presentation_id);

    return mock();
}

/* HELPER FUNCTIONS */

static void will_return_get_preselections(alps_mp4dmx_preselection *preselections, size_t count, alps_ret ret_val)
{
    will_return(__wrap_alps_mp4dmx_get_preselections, preselections);
    will_return(__wrap_alps_mp4dmx_get_preselections, count);
    will_return(__wrap_alps_mp4dmx_get_preselections, ret_val);
}

static void will_return_sample_iteration(size_t sample_count, unsigned char *frame_buf, size_t buf_size, int active_pres_id)
{
    size_t i;
    for (i = 0; i < sample_count; i++)
    {
        will_return(__wrap_alps_mp4dmx_next_sample, frame_buf);
        will_return(__wrap_alps_mp4dmx_next_sample, buf_size);
        will_return(__wrap_alps_mp4dmx_next_sample, ALPS_RET_OK);
        expect_value(__wrap_alps_ac4parser_set_active_presentation_id, frame_buf, frame_buf);
        expect_value(__wrap_alps_ac4parser_set_active_presentation_id, size_in_bytes, buf_size);
        expect_value(__wrap_alps_ac4parser_set_active_presentation_id, active_presentation_id, active_pres_id);
        will_return(__wrap_alps_ac4parser_set_active_presentation_id, ALPS_RET_OK);
    }

    will_return(__wrap_alps_mp4dmx_next_sample, NULL);
    will_return(__wrap_alps_mp4dmx_next_sample, 0);
    will_return(__wrap_alps_mp4dmx_next_sample, ALPS_RET_E_NEXT_SEGMENT);
}

static void will_return_process_buffer(unsigned char *segment_buf, size_t buf_size, alps_ret ret_val)
{
    expect_value(__wrap_alps_mp4dmx_process_buffer, buffer, segment_buf);
    expect_value(__wrap_alps_mp4dmx_process_buffer, size, buf_size);
    will_return(__wrap_alps_mp4dmx_process_buffer, ret_val);
}

/* TEST CASES */

static void query_mem__all_ok__correct_memory_size_returned(void **state)
{
    alps_ret ret;
    size_t mem_size = 0;

    (void)state;

    will_return(__wrap_alps_mp4dmx_query_mem, ALPS_RET_OK);

    ret = alps_query_mem(&mem_size);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_int_equal(mem_size, SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE);
}

static void query_mem__null_arg__invalid_arg_returned(void **state)
{
    alps_ret ret;

    (void)state;

    ret = alps_query_mem(NULL);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
}

static void query_mem__mp4dmx_fails__error_returned(void **state)
{
    alps_ret ret;
    size_t mem_size = 0;

    (void)state;

    will_return(__wrap_alps_mp4dmx_query_mem, ALPS_RET_E_UNDEFINED);

    ret = alps_query_mem(&mem_size);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
    assert_int_equal(mem_size, 0);
}

static void init__all_ok__library_initialized(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];
    alps_presentation *presentations = NULL;
    size_t pres_count = 0;
    int active_presentation_id = 1;

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_OK);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_get_presentations(ctx, &presentations, &pres_count);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_ptr_equal(presentations, NULL);
    assert_int_equal(pres_count, 0);

    alps_get_active_presentation_id(ctx, &active_presentation_id);
    assert_int_equal(active_presentation_id, ALPS_INVALID_PRES_ID);

    alps_destroy(ctx);
}

static void init__null_ctx_arg__invalid_arg_error_returned(void **state)
{
    alps_ret ret;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];

    (void)state;

    ret = alps_init(NULL, mem);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
}

static void init__null_mem_arg__invalid_arg_error_returned(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;

    (void)state;

    ret = alps_init(&ctx, NULL);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
    assert_ptr_equal(ctx, NULL);
}

static void init__mp4dmx_fails__error_returned(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_E_UNDEFINED);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void process_isobmff_segment__all_ok__presentations_available(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];
    unsigned char segment_buf[128];
    alps_mp4dmx_preselection preselections[] = {
        PRESELECTION_1,
        PRESELECTION_2,
        PRESELECTION_3
    };
    size_t prsl_count = sizeof(preselections) / sizeof(preselections[0]);
    unsigned char *frame_buf = (unsigned char*)0x123abc;
    size_t frame_buf_size = 654321;
    int expected_active_pres_id = preselections[prsl_count - 1].preselection_tag;
    alps_presentation *presentations = NULL;
    size_t pres_count = 0;
    size_t i;
    int active_presentation_id = 1;

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_OK);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_set_active_presentation_id(ctx, expected_active_pres_id);

    will_return_process_buffer(segment_buf, sizeof(segment_buf), ALPS_RET_OK);
    will_return_get_preselections(preselections, prsl_count, ALPS_RET_OK);
    will_return_sample_iteration(3, frame_buf, frame_buf_size, expected_active_pres_id);

    ret = alps_process_isobmff_segment(ctx, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_get_presentations(ctx, &presentations, &pres_count);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_non_null(presentations);
    assert_int_equal(pres_count, prsl_count);
    for (i = 0; i < pres_count; i++) {
        assert_int_equal(presentations[i].id,
                         preselections[i].preselection_tag);
        assert_int_equal(presentations[i].labels_count, preselections[i].labels_count);
        assert_string_equal(presentations[i].extended_language, preselections[i].extended_language);
    }

    alps_get_active_presentation_id(ctx, &active_presentation_id);
    assert_int_equal(active_presentation_id, expected_active_pres_id);

    alps_destroy(ctx);
}

static void process_isobmff_segment__ctx_null__invalid_arg_error_returned(void **state)
{
    alps_ret ret;
    unsigned char segment_buf[128];

    (void)state;

    ret = alps_process_isobmff_segment(NULL, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
}

static void process_isobmff_segment__buffer_null__invalid_arg_error_returned(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_OK);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_process_isobmff_segment(ctx, NULL, 0);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
}

static void process_isobmff_segment__mp4dmx_process_fails__error_returned(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];
    unsigned char segment_buf[128];

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_OK);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    will_return_process_buffer(segment_buf, sizeof(segment_buf), ALPS_RET_E_BUFF_TOO_SMALL);

    ret = alps_process_isobmff_segment(ctx, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_E_BUFF_TOO_SMALL);
}

static void process_isobmff_segment__mp4dmx_getting_prsl_fails__error_returned(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];
    unsigned char segment_buf[128];
    alps_mp4dmx_preselection preselections[] = {
        PRESELECTION_1,
        PRESELECTION_2,
        PRESELECTION_3
    };
    size_t prsl_count = sizeof(preselections) / sizeof(preselections[0]);

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_OK);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    will_return_process_buffer(segment_buf, sizeof(segment_buf), ALPS_RET_OK);
    will_return_get_preselections(preselections, prsl_count, ALPS_RET_E_UNDEFINED);

    ret = alps_process_isobmff_segment(ctx, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_E_UNDEFINED);
}

static void process_isobmff_segment__mp4dmx_zero_prsl_zero_samples__error_returned(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];
    unsigned char segment_buf[128];
    alps_presentation *presentations = NULL;
    size_t pres_count = 0;

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_OK);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    will_return_process_buffer(segment_buf, sizeof(segment_buf), ALPS_RET_OK);
    will_return_get_preselections(NULL, 0, ALPS_RET_OK);
    will_return_sample_iteration(0, NULL, 0, ALPS_INVALID_PRES_ID);

    ret = alps_process_isobmff_segment(ctx, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_get_presentations(ctx, &presentations, &pres_count);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_ptr_equal(presentations, NULL);
    assert_int_equal(pres_count, 0);
}

void pres_changed_cb(callback_ctx ctx)
{
    check_expected_ptr(ctx);
}

static void process_isobmff_segment__pres_list_changes_twice__callback_called_twice(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];
    callback_ctx cb_ctx = (callback_ctx)0x123abc;
    unsigned char segment_buf[128];
    alps_mp4dmx_preselection prsl_1[] = {
        PRESELECTION_1,
        PRESELECTION_2
    };
    size_t prsl_1_count = sizeof(prsl_1) / sizeof(prsl_1[0]);
    alps_mp4dmx_preselection prsl_2[] = {
        PRESELECTION_1,
        PRESELECTION_2,
        PRESELECTION_3
    };
    size_t prsl_2_count = sizeof(prsl_2) / sizeof(prsl_2[0]);
    alps_presentation *presentations = NULL;
    size_t pres_count = 0;
    size_t i;

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_OK);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    alps_set_presentations_changed_callback(ctx, pres_changed_cb, cb_ctx);

    will_return_process_buffer(segment_buf, sizeof(segment_buf), ALPS_RET_OK);
    will_return_get_preselections(prsl_1, prsl_1_count, ALPS_RET_OK);
    expect_value(pres_changed_cb, ctx, cb_ctx);
    will_return_sample_iteration(0, NULL, 0, ALPS_INVALID_PRES_ID);

    ret = alps_process_isobmff_segment(ctx, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_OK);

    will_return_process_buffer(segment_buf, sizeof(segment_buf), ALPS_RET_OK);
    will_return_get_preselections(prsl_2, prsl_2_count, ALPS_RET_OK);
    expect_value(pres_changed_cb, ctx, cb_ctx);
    will_return_sample_iteration(0, NULL, 0, ALPS_INVALID_PRES_ID);

    ret = alps_process_isobmff_segment(ctx, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_get_presentations(ctx, &presentations, &pres_count);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_non_null(presentations);
    assert_int_equal(pres_count, prsl_2_count);
    for (i = 0; i < pres_count; i++) {
        assert_int_equal(presentations[i].id,
                         prsl_2[i].preselection_tag);
        assert_int_equal(presentations[i].labels_count, prsl_2[i].labels_count);
        assert_string_equal(presentations[i].extended_language, prsl_2[i].extended_language);
    }
}

static void process_isobmff_segment__pres_list_changes_once__callback_called_once(void **state)
{
    alps_ret ret;
    alps_ctx *ctx = NULL;
    unsigned char mem[SIZEOF_ALPS_CTX + MP4DMX_MEM_SIZE];
    callback_ctx cb_ctx = (callback_ctx)0x123abc;
    unsigned char segment_buf[128];
    alps_mp4dmx_preselection prsl[] = {
        PRESELECTION_1,
        PRESELECTION_2
    };
    size_t prsl_count = sizeof(prsl) / sizeof(prsl[0]);
    alps_presentation *presentations = NULL;
    size_t pres_count = 0;
    size_t i;

    (void)state;

    will_return(__wrap_alps_mp4dmx_init, ALPS_RET_OK);

    ret = alps_init(&ctx, mem);
    assert_int_equal(ret, ALPS_RET_OK);

    alps_set_presentations_changed_callback(ctx, pres_changed_cb, cb_ctx);

    will_return_process_buffer(segment_buf, sizeof(segment_buf), ALPS_RET_OK);
    will_return_get_preselections(prsl, prsl_count, ALPS_RET_OK);
    expect_value(pres_changed_cb, ctx, cb_ctx);
    will_return_sample_iteration(0, NULL, 0, ALPS_INVALID_PRES_ID);

    ret = alps_process_isobmff_segment(ctx, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_OK);

    will_return_process_buffer(segment_buf, sizeof(segment_buf), ALPS_RET_OK);
    will_return_get_preselections(prsl, prsl_count, ALPS_RET_OK);
    will_return_sample_iteration(0, NULL, 0, ALPS_INVALID_PRES_ID);

    ret = alps_process_isobmff_segment(ctx, segment_buf, sizeof(segment_buf));
    assert_int_equal(ret, ALPS_RET_OK);

    ret = alps_get_presentations(ctx, &presentations, &pres_count);
    assert_int_equal(ret, ALPS_RET_OK);
    assert_non_null(presentations);
    assert_int_equal(pres_count, prsl_count);
    for (i = 0; i < pres_count; i++) {
        assert_int_equal(presentations[i].id,
                         prsl[i].preselection_tag);
        assert_int_equal(presentations[i].labels_count, prsl[i].labels_count);
        assert_string_equal(presentations[i].extended_language, prsl[i].extended_language);
    }
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(query_mem__all_ok__correct_memory_size_returned),
        cmocka_unit_test(query_mem__null_arg__invalid_arg_returned),
        cmocka_unit_test(query_mem__mp4dmx_fails__error_returned),
        cmocka_unit_test(init__all_ok__library_initialized),
        cmocka_unit_test(init__null_mem_arg__invalid_arg_error_returned),
        cmocka_unit_test(init__null_ctx_arg__invalid_arg_error_returned),
        cmocka_unit_test(init__mp4dmx_fails__error_returned),
        cmocka_unit_test(process_isobmff_segment__all_ok__presentations_available),
        cmocka_unit_test(process_isobmff_segment__ctx_null__invalid_arg_error_returned),
        cmocka_unit_test(process_isobmff_segment__buffer_null__invalid_arg_error_returned),
        cmocka_unit_test(process_isobmff_segment__mp4dmx_process_fails__error_returned),
        cmocka_unit_test(process_isobmff_segment__mp4dmx_getting_prsl_fails__error_returned),
        cmocka_unit_test(process_isobmff_segment__mp4dmx_zero_prsl_zero_samples__error_returned),
        cmocka_unit_test(process_isobmff_segment__pres_list_changes_twice__callback_called_twice),
        cmocka_unit_test(process_isobmff_segment__pres_list_changes_once__callback_called_once)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
