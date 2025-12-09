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

#include <bam.h>
#include <dlb_bitbuf.h>
#include <dlb_bitbuf_read.h>
#include <dlb_bitbuf_write.h>
#include <stdio.h>

/* static */
unsigned long
position;

/* returns true if (the beginning of) another frame is available */
bam_error
BAMSource_next_frame (struct BAMSource * self
                      , int *b_next_frame)
{
    self->position = 0;
    return BAM_OK; /* unused value - derived classes should compute the return value */
}

void
BAMSource_init(struct BAMSource * self)
{
    /* declare functions that are not abstract here */
    self->next_frame = BAMSource_next_frame;
    self->position = 0;
}

bam_error
BAMSource_add_position (struct BAMSource * self
                        , unsigned width)
{
    self->position += width;
    return BAM_OK;
}

unsigned long
BAMSource_get_position (struct BAMSource * self)
{
    return self->position;
}
