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

#ifndef BAM_MAP_H_INCLUDED
#define BAM_MAP_H_INCLUDED
#include <bam_types.h>
#include <bam_parameter.h>
#include <bam_vector.h>

/* opaque data type */
struct BAMMap;

/*  creates empty map,
    returns NULL if MAP could not be allocated */
struct BAMMap *
BAMMap_create(size_t key_size
              , size_t value_size);

/* Insert Key/Value as Map(INT -> INT) */
bam_error
BAMMap_insert_int_int   (struct BAMMap *bammap
                        , BAMint key
                        , BAMint value);

/* Insert Key/Value as Map(INT -> Vector(INT)) */
bam_error
BAMMap_insert_int_vector   (struct BAMMap *bammap
                            , BAMint key
                            , struct BAMVector *value);

/* Insert Key/Value as Map(INT -> VSTRING) */
bam_error
BAMMap_insert_int_string (struct BAMMap *bammap
                          , BAMint key
                          , char *value);

bam_error
BAMMap_insert_int_map (struct BAMMap *bammap
                       , BAMint key
                       , struct BAMMap *value);

BAMint
BAMMap_get_int  (const struct BAMMap *bammap
                , BAMint key);

struct BAMVector *
BAMMap_get_vector_int   (const struct BAMMap *bammap
                         , BAMint key);

char *
BAMMap_get_string (const struct BAMMap *bammap
                   , BAMint key);

struct BAMMap *
BAMMap_get_map(const struct BAMMap *bammap
               , BAMint key);

struct BAMVector *
BAMMap_get_keys(struct BAMMap *bammap);

struct BAMVector *
BAMMap_get_values(struct BAMMap *bammap);

void
BAMMap_destroy (struct BAMMap *bammap);

void
BAMMap_clear(struct BAMMap* bammap);

struct BAMMap *
BAMMap_deep_copy(struct BAMMap *bammap);

#endif
