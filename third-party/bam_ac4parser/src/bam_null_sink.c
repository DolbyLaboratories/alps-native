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

#include <stdlib.h>
#include <bam.h>
#include <bam_vector.h>
#include <bam_map.h>
#include <bam_null_sink.h>
#include <stdio.h>
#include <bam_parameter.h>
#include <string.h>
#include <assert.h>

typedef struct
NullSink
{
    struct BAMSink super;
    unsigned int indent;

} NullSink;

static void
NullSink_destroy(struct BAMSink *self)
{
    free(self);
}

struct BAMSink *
NullSink_create()
{
    struct NullSink *nullsink = malloc(sizeof * nullsink);
	nullsink->indent = 0;

    if (nullsink == NULL)
    {
        free(nullsink);
        return NULL;
    }

    BAMSink_init(&nullsink->super);
	nullsink->super.destroy									= NullSink_destroy;

    return &nullsink->super;
};
