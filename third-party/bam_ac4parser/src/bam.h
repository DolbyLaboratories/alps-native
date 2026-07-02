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

#ifndef BAM_H
#define BAM_H
#include <string.h>
#include <bam_vector.h>
#include <bam_types.h>
#include <bam_parameter.h>
#include <bam_map.h>
#include <stdio.h>

typedef
struct BAMSource
{
    bam_error (*read_uint)              (struct BAMSource *self
                                        , unsigned width
                                        , long* value);     /* [out] */

    bam_error (*read_align)             (struct BAMSource *self
                                        , unsigned width
                                        , long* value);     /* [out] */

    bam_error (*push)                   (struct BAMSource *self
                                        , unsigned width
                                        , BAMint value);    /* [in] */

    bam_error (*next_frame)             (struct BAMSource *self
                                        , int *b_next_frame); /* [out] whether another frame is available */

    bam_error (*end_of_stream)          (struct BAMSource *self
                                        , int *b_end_of_stream /* [out] whether it is the end of the input stream */
                                        );

    void (*destroy)                     (struct BAMSource *self);

    unsigned long position;

} BAMSource;

/* converts int to char* */
char*
BAM_to_string(int value);

/* concatenation of two strings */
const char*
BAM_string_concat(const char* string_1
				, char* string_2);

void
BAMSource_init(struct BAMSource *self);

/* BAMSource methods which cannot be overridden */
bam_error
BAMSource_add_position(struct BAMSource *self
                       , unsigned width);

unsigned long
BAMSource_get_position(struct BAMSource *self);

/* default implementation of next_frame()
   Should be overridden by derived classes */
bam_error
BAMSource_next_frame(struct BAMSource *self
                     , int *b_next_frame);

/* Sink functions for the text output */
typedef
struct BAMSink
{
	bam_error (*before_frame)							(struct BAMSink *self);

	bam_error (*after_frame)							(struct BAMSink* self);

    bam_error (*before_call)                            (struct BAMSink *self
                                                         , const char *fname
                                                         , const struct BAMVector *params
                                                         , const char *return_var_name); 

    bam_error (*write_uint)                             (struct BAMSink *self
                                                         , const char *varname
                                                         , unsigned n
                                                         , BAMint val);

    bam_error (*write_code)                             (struct BAMSink *self
                                                         , const char *code_name
                                                         , const char *value_name
                                                         , BAMint code
                                                         , unsigned length
                                                         , BAMint value);

    bam_error (*write_align)                            (struct BAMSink *self
                                                         , unsigned num
                                                         , unsigned value);

    bam_error (*after_call_int)                         (struct BAMSink *self
                                                         , const char *fname
                                                         , const char *return_var_name
                                                         , BAMint return_value);

    bam_error (*after_call_float)                       (struct BAMSink *self
                                                         , const char *fname
                                                         , const char *return_var_name
                                                         , BAMfloat return_value);

    bam_error (*after_call_vector_int)                  (struct BAMSink *self
                                                         , const char *fname
                                                         , const char *return_var_name
                                                         , struct BAMVector *bamvector);

    bam_error (*after_call_vector_vector_int)           (struct BAMSink *self
                                                         , const char *fname
                                                         , const char *return_var_name
                                                         , struct BAMVector *bamvector);


    bam_error (*after_call_vector_vector_vector_int)    (struct BAMSink * self
                                                         , const char *fname
                                                         , const char *return_var_name
                                                         , struct BAMVector *bamvector);

    bam_error (*after_call_void)                        (struct BAMSink *self
                                                         , const char *fname);

    bam_error (*after_call_string)                      (struct BAMSink *self
                                                         , const char *fname
                                                         , const char *return_var_name
                                                         , const char *string_name);

    bam_error (*after_call_map_int_int)                 (struct BAMSink *self
                                                         , const char *fname
                                                         , const char *return_var_name
                                                         , struct BAMMap *bammap);

    bam_error (*after_assign_int)                       (struct BAMSink *self
                                                         , const char *varname
                                                         , BAMint val);

    bam_error (*after_assign_string)                    (struct BAMSink *self
                                                         , const char *varname
                                                         , const char *value);

    bam_error (*after_assign_vector_int)                (struct BAMSink *self
                                                         , const char *varname
                                                         , struct BAMVector *bamvector);

	bam_error(*after_assign_vector_float)				(struct BAMSink* self
														, const char* varname
														, struct BAMVector* bamvector);

    bam_error (*after_assign_vector_string)             (struct BAMSink *self
                                                         , const char *name
                                                         , struct BAMVector *bamvector);

    bam_error (*after_assign_vector_vector_int)         (struct BAMSink *self
                                                         , const char *name
                                                         , struct BAMVector *bamvector);

	bam_error(*after_assign_vector_vector_vector_int)	(struct BAMSink *self
														, const char *varname
														, struct BAMVector *bamvector);

    bam_error (*after_assign_map_int_int)               (struct BAMSink *self
                                                         , const char *varname
                                                         , struct BAMMap *bammap);

    bam_error (*after_assign_map_int_vector_int)        (struct BAMSink *self
                                                         , const char *name
                                                         , struct BAMMap *bammap);

    bam_error (*after_assign_map_int_map_int_vector_int)(struct BAMSink *self
                                                         , const char *varname
                                                         , struct BAMMap *bammap);

    bam_error (*after_assign_map_vector_int_int)        (struct BAMSink *self
                                                         , const char *name
                                                         , struct BAMMap *bammap);

    bam_error (*after_push)                             (struct BAMSink *self
                                                         , unsigned num
                                                         , BAMint value);

    bam_error (*position)                               (struct BAMSink *self
                                                         , const char *varname
                                                         , BAMint value);

    void      (*destroy)                                (struct BAMSink *self);
} BAMSink;

void
BAMSink_init (struct BAMSink *self);

typedef
struct BAMSyntax
{
    bam_error (*reset)          (struct BAMSyntax *self);

    bam_error (*set_input)      (struct BAMSyntax *self
                                 , struct BAMVector *params); 

    BAMint (*get_result)        (struct BAMSyntax *self);

    bam_error (*parse_generic)  (struct BAMSyntax *self
                                 , struct BAMSource *BAM_source
                                 , struct BAMSink *BAM_sink);

    void (*destroy)             (struct BAMSyntax *self);

    BAMint BAM_result;
    const char *BAM_result_string;
    struct BAMVector *BAM_result_vector;
    struct BAMMap *BAM_result_map;
} BAMSyntax;

#endif
