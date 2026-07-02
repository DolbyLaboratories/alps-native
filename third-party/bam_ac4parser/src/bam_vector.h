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

#ifndef BAM_VECTOR_H_INCLUDED
#define BAM_VECTOR_H_INCLUDED
#include <bam_types.h>
#include <bam_parameter.h>

#include <stddef.h>
#include <bam_map.h>

struct BAMVector;

/* returns NULL if vector could not be allocated,
   returns a new object, with num_references = 1 */
struct BAMVector *
BAMVector_create(size_t element_size, int b_elements_are_strings);

/* increases the reference count of 'vector' by 1 and returns 'vector' */
struct BAMVector *
BAMVector_create_reference(struct BAMVector *bamvector);

/* number of elements in the vector */
BAMint
BAMVector_size(const struct BAMVector *bamvector);

/* returns pointer to the data array */
struct BAMParameter **
BAMVector_data_parameter(const struct BAMVector *bamvector);

BAMint *
BAMVector_data_int(struct BAMVector *bamvector);

BAMfloat *
BAMVector_data_float(struct BAMVector *bamvector);

char **
BAMVector_data_string(struct BAMVector *bamvector);

struct BAMMap **
BAMVector_data_map(struct BAMVector *bamvector);

/* returns an array of BAMVector* */
struct BAMVector **
BAMVector_data_vector_int(struct BAMVector *bamvector);

struct BAMVector **
BAMVector_data_vector_vector_int(struct BAMVector *bamvector);

struct BAMVector **
BAMVector_data_vector_vector_vector_int(struct BAMVector *bamvector);

struct BAMVector **
BAMVector_data_vector_vector_vector_vector_int (struct BAMVector *bamvector);

BAMint
BAMVector_find (struct BAMVector *bamvector
               , BAMint object_to_find);

int
BAMVector_find_string (const struct BAMVector *BAMVector
                       , const char *object_to_find);

/* appends element, grows internal buffer if necessary */

bam_error
BAMVector_push_vector_map(struct BAMVector *bamvector
                          , struct BAMMap *bammap);

bam_error
BAMVector_push_vector_int(struct BAMVector *bamvector
                          , BAMint element);

bam_error
BAMVector_push_vector_float(struct BAMVector *bamvector
                            , BAMfloat element);

bam_error
BAMVector_push_vector_vector_int(struct BAMVector *bamvector
                                 , struct BAMVector *element);

bam_error
BAMVector_push_vector_vector_vector_int(struct BAMVector *bamvector
                                        , struct BAMVector *element);

bam_error
BAMVector_push_vector_vector_vector_vector_int(struct BAMVector *bamvector
                                        , struct BAMVector *element);

bam_error
BAMVector_push_vector_vector_vector_vector_vector_int(struct BAMVector *bamvector
                                        , struct BAMVector *element);

bam_error
BAMVector_push_vector_string(struct BAMVector *bamvector
                             , const char *element);

bam_error
BAMVector_push_vector_parameter(struct BAMVector *BAMVector
                                , const struct BAMParameter *bamparameter);

/* sets vector size to zero */
bam_error
BAMVector_clear(struct BAMVector *bamvector);

bam_error
BAMVector_clear_parameter(struct BAMVector *bamvector);

struct BAMVector *
BAMVector_concat_vector_int (struct BAMVector *v1
							, struct BAMVector *v2);

struct BAMVector *
BAMVector_concat_int(const struct BAMVector *v1
					, const struct BAMVector *v2);

struct BAMVector*
BAMVector_concat_float (struct BAMVector *v1
						, struct BAMVector *v2);

struct BAMVector *
BAMVector_deep_copy_int(const struct BAMVector *bamvector);

struct BAMVector *
BAMVector_deep_copy_2d_int(const struct BAMVector *bamvector);

struct BAMVector *
BAMVector_deep_copy_3d_int(const struct BAMVector *bamvector);

bam_error
BAMVector_resize_vector_string (struct BAMVector *bamvector
                                , size_t new_size);

bam_error
BAMVector_resize_vector_int(struct BAMVector *bamvector
                            , size_t new_size);

bam_error
BAMVector_resize_vector_vector_int(struct BAMVector *bamvector
                                   , size_t new_size);

bam_error
BAMVector_resize_vector_vector_vector_int(struct BAMVector *bamvector
                                          , size_t new_size);

bam_error
BAMVector_resize_vector_vector_vector_vector_int (struct BAMVector *bamvector
                                                  , size_t new_size);

bam_error
BAMVector_resize_vector_vector_vector_vector_vector_int (struct BAMVector *bamvector
                                                         , size_t new_size);

/* decreases the reference count by one. If there are no more references, the memory is free'd */
void
BAMVector_destroy(struct BAMVector *bamvector
                  , int depth);

/* private */
void
destroy_string_element(struct BAMVector *bamvector
                       , int index);

#endif
