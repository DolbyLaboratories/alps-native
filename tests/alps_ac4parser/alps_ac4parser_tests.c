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

#include "bam.h"

#include "dlb_alps_native/alps_ac4parser/alps_ac4parser.h"

#define PRESENTATIONS_COUNT (3)
#define FIRST_PRESENTATION_ID (1)
#define PRESENTATION_ID_OUT_OF_RANGE (FIRST_PRESENTATION_ID - 1)
#define FOR_ALL_PRESENTATIONS for (i = FIRST_PRESENTATION_ID; i <= PRESENTATIONS_COUNT; i++)
#define UNUSED_VAL (1234)
#define NOT_PARSED_VAR_NAME "not_parsed_var_name"

static const unsigned char PRE_PRES_DATA[] = "pre_pres_data";
static const unsigned char PRE_ID_DATA[] = "pre_id_data";
typedef int pres_id_type;
static const unsigned char MIDDLE_DATA[] = "middle_data";
typedef uint8_t pres_level_type;
static const unsigned char POST_LEVEL_DATA[] = "post_level_data";
static const unsigned char POST_PRES_DATA[] = "post_pres_data";

typedef struct test_buffer_t
{
    unsigned char data[
        sizeof(PRE_PRES_DATA) + sizeof(POST_PRES_DATA) +
        3 * (sizeof(PRE_ID_DATA) + sizeof(pres_id_type) + sizeof(MIDDLE_DATA) + sizeof(pres_level_type) + sizeof(POST_LEVEL_DATA))];
} test_buffer;

/* FUNCTIONS' MOCKS */

static void destroy_source(BAMSource *self)
{
    function_called();

    free(self);
}

BAMSource *__wrap_BinarySource_create(unsigned char *buffer, size_t _size)
{
    (void)buffer;
    (void)_size;

    BAMSource *source = malloc(sizeof(BAMSource));
    source->destroy = destroy_source;

    expect_function_call(destroy_source);

    return source;
}

void destroy_syntax(struct BAMSyntax *self)
{
    function_called();

    free(self);
}

BAMSyntax *__wrap_F_raw_ac4_frame_toc_only_create(void)
{
    BAMSyntax *syntax = malloc(sizeof(BAMSyntax));
    syntax->destroy = destroy_syntax;

    expect_function_call(destroy_syntax);

    return syntax;
}

void parse_presentation(BAMSink *sink, int pres_idx)
{
    /* skip over pre_id_data */
    sink->write_align(sink, sizeof(PRE_ID_DATA) * 8, UNUSED_VAL);

    /* read presentation_id */
    sink->after_call_int(sink, NULL, "presentation_id", pres_idx);
    sink->write_align(sink, sizeof(pres_id_type) * 8, UNUSED_VAL);

    /* skip over middle_data */
    sink->write_align(sink, sizeof(MIDDLE_DATA) * 8, UNUSED_VAL);

    /* skip first 5 bits of pres_level_type */
    sink->write_align(sink, 5, UNUSED_VAL);
    /* read presentation level */
    sink->write_uint(sink, "presentation_level", 3, pres_idx);

    /* skip over post_level_data */
    sink->write_uint(sink, NOT_PARSED_VAR_NAME, sizeof(POST_LEVEL_DATA) * 8, UNUSED_VAL);
}

bam_error __wrap_F_raw_ac4_frame_toc_only_parse(void *syntax, BAMSource *source, BAMSink *sink)
{
    int i;
    int ret_val = mock();

    (void)syntax;
    (void)source;

    if (ret_val == BAM_OK)
    {
        /* allocate and initialize presentations array */
        sink->after_assign_int(sink, "n_presentations", PRESENTATIONS_COUNT);
        /* call below to test if the same function with different params does not break anything */
        sink->after_assign_int(sink, NOT_PARSED_VAR_NAME, PRESENTATIONS_COUNT);

        /* skip pre_pres_data */
        sink->write_uint(sink, NOT_PARSED_VAR_NAME, sizeof(PRE_PRES_DATA) * 8, UNUSED_VAL);

        FOR_ALL_PRESENTATIONS
        {
            parse_presentation(sink, i);
            /* iterate to the next presentation */
            sink->after_call_void(sink, "ac4_presentation_info");
        }
    }

    return ret_val;
}

/* HELPER STRUCTS AND FUNCTIONS */

void init_test_buffer(test_buffer *test_buf)
{
    int i;
    size_t offset = 0;

    memcpy(((char*)test_buf->data + offset), PRE_PRES_DATA, sizeof(PRE_PRES_DATA));
    offset += sizeof(PRE_PRES_DATA);
    FOR_ALL_PRESENTATIONS
    {
        memcpy(((char*)test_buf->data + offset), PRE_ID_DATA, sizeof(PRE_ID_DATA));
        offset += sizeof(PRE_ID_DATA);
        *((pres_id_type*)(((char*)test_buf->data + offset))) = i;
        offset += sizeof(pres_id_type);
        memcpy(((char*)test_buf->data + offset), MIDDLE_DATA, sizeof(MIDDLE_DATA));
        offset += sizeof(MIDDLE_DATA);
        *((pres_level_type*)(((char*)test_buf->data + offset))) = i;
        offset += sizeof(pres_level_type);
        memcpy(((char*)test_buf->data + offset), POST_LEVEL_DATA, sizeof(POST_LEVEL_DATA));
        offset += sizeof(POST_LEVEL_DATA);
    }

    memcpy(((char*)test_buf->data + offset), POST_PRES_DATA, sizeof(POST_PRES_DATA));
    offset += sizeof(POST_PRES_DATA);
}

void verify_test_buffer(test_buffer *test_buf, const int active_pres_id)
{

    int i;
    size_t offset = 0;
    int pres_id;
    int active_pres_id_out_of_range =
        active_pres_id < FIRST_PRESENTATION_ID || active_pres_id > FIRST_PRESENTATION_ID + PRESENTATIONS_COUNT;
    int pres_active;

    assert_string_equal(((char*)test_buf->data + offset), (char*)PRE_PRES_DATA);
    offset += sizeof(PRE_PRES_DATA);
    FOR_ALL_PRESENTATIONS
    {
        assert_string_equal(((char*)test_buf->data + offset), (char*)PRE_ID_DATA);
        offset += sizeof(PRE_ID_DATA);
        pres_id = *((pres_id_type*)(((char*)test_buf->data + offset)));
        assert_int_equal(pres_id, i);
        offset += sizeof(pres_id_type);
        assert_string_equal(((char*)test_buf->data + offset), (char*)MIDDLE_DATA);
        offset += sizeof(MIDDLE_DATA);
        pres_active = active_pres_id == pres_id || active_pres_id_out_of_range;
        assert_int_equal(*((pres_level_type*)(((char*)test_buf->data + offset))), pres_active ? pres_id : 7);
        offset += sizeof(pres_level_type);
        assert_string_equal(((char*)test_buf->data + offset), (char*)POST_LEVEL_DATA);
        offset += sizeof(POST_LEVEL_DATA);
    }

    assert_string_equal(((char*)test_buf->data + offset), (char*)POST_PRES_DATA);
    offset += sizeof(POST_PRES_DATA);
}

/* TEST CASES */

static void set_active_presentation_id__all_ok__level_set_correctly(void **state)
{
    int i;
    test_buffer test_buf;
    alps_ret ret;

    (void)state;

    FOR_ALL_PRESENTATIONS
    {
        init_test_buffer(&test_buf);

        will_return(__wrap_F_raw_ac4_frame_toc_only_parse, BAM_OK);

        ret = alps_ac4parser_set_active_presentation_id(test_buf.data, sizeof(test_buf.data), i);
        assert_int_equal(ret, ALPS_RET_OK);
        verify_test_buffer(&test_buf, i);
    }
}

static void set_active_presentation_id__active_pres_id_invalid__level_not_changed(void **state)
{
    test_buffer test_buf;
    alps_ret ret;

    (void)state;

    init_test_buffer(&test_buf);

    ret = alps_ac4parser_set_active_presentation_id(test_buf.data, sizeof(test_buf.data), ALPS_INVALID_PRES_ID);
    assert_int_equal(ret, ALPS_RET_OK);
    verify_test_buffer(&test_buf, ALPS_INVALID_PRES_ID);
}

static void set_active_presentation_id__active_pres_id_not_found__err_returned_level_not_changed(void **state)
{
    test_buffer test_buf;
    alps_ret ret;

    (void)state;

    init_test_buffer(&test_buf);

    will_return(__wrap_F_raw_ac4_frame_toc_only_parse, BAM_OK);

    ret = alps_ac4parser_set_active_presentation_id(test_buf.data, sizeof(test_buf.data), PRESENTATION_ID_OUT_OF_RANGE);
    assert_int_equal(ret, ALPS_RET_E_PRES_ID_NOT_FOUND);
    verify_test_buffer(&test_buf, PRESENTATION_ID_OUT_OF_RANGE);
}

static void set_active_presentation_id__invalid_arg__invalid_arg_error_returned(void **state)
{
    test_buffer test_buf;
    alps_ret ret;

    (void)state;

    init_test_buffer(&test_buf);

    ret = alps_ac4parser_set_active_presentation_id(NULL, sizeof(test_buf.data), FIRST_PRESENTATION_ID);
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
    /* verify that the date did not change */
    verify_test_buffer(&test_buf, PRESENTATION_ID_OUT_OF_RANGE);

    ret = alps_ac4parser_set_active_presentation_id(test_buf.data, 0, FIRST_PRESENTATION_ID);
    /* verify that the date did not change */
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
    verify_test_buffer(&test_buf, PRESENTATION_ID_OUT_OF_RANGE);

    ret = alps_ac4parser_set_active_presentation_id(NULL, 0, FIRST_PRESENTATION_ID);
    /* verify that the date did not change */
    assert_int_equal(ret, ALPS_RET_E_INVALID_ARG);
    verify_test_buffer(&test_buf, PRESENTATION_ID_OUT_OF_RANGE);
}

static void set_active_presentation_id__toc_parse_fails__parse_error_returned(void **state)
{
    test_buffer test_buf;
    alps_ret ret;

    (void)state;

    init_test_buffer(&test_buf);

    will_return(__wrap_F_raw_ac4_frame_toc_only_parse, BAM_REALLOC_ERROR);

    ret = alps_ac4parser_set_active_presentation_id(test_buf.data, sizeof(test_buf.data), FIRST_PRESENTATION_ID);
    assert_int_equal(ret, ALPS_RET_E_PARSE);
    verify_test_buffer(&test_buf, PRESENTATION_ID_OUT_OF_RANGE);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(set_active_presentation_id__all_ok__level_set_correctly),
        cmocka_unit_test(set_active_presentation_id__active_pres_id_invalid__level_not_changed),
        cmocka_unit_test(set_active_presentation_id__active_pres_id_not_found__err_returned_level_not_changed),
        cmocka_unit_test(set_active_presentation_id__invalid_arg__invalid_arg_error_returned),
        cmocka_unit_test(set_active_presentation_id__toc_parse_fails__parse_error_returned)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
