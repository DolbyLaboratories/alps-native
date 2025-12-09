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

#include <bam_binary_source.h>
#include <bam.h>
#include <stdio.h>
#include <stdlib.h>
#include <bitbuf_pvt.h>
#include <dlb_bitbuf.h>
#include <dlb_bitbuf_read.h>
#include <dlb_bitbuf_write.h>

typedef struct
BinarySource
{
    struct BAMSource super;
    dlb_bitbuf bitbuf;

} BinarySource;

bam_error
BinarySource_next_frame (struct BAMSource *self
                       , int *b_next_frame)
{
    struct BinarySource *binarySource = (struct BinarySource*) self;
    bam_error err;

    err = BAMSource_next_frame(self, b_next_frame);
    if (err != BAM_OK)
    {
        return err;
    }

    if (dlb_bitbuf_get_bits_left(&binarySource->bitbuf) > 0)
    {
        return BAM_OK;
    }

    else
    {
        return BAM_BITS_OUT_OF_RANGE;
    }
}

bam_error
BinarySource_end_of_stream(struct BAMSource *self,
                           int *b_eos)
{
    struct BinarySource *binarySource = (struct BinarySource*) self;

    *b_eos = dlb_bitbuf_get_bits_left(&binarySource->bitbuf) <= 0;
    return BAM_OK;
}


bam_error
BinarySource_read_uint (struct BAMSource *self
                      , unsigned width
                      , long *value)
{
    struct BinarySource *binarySource = (struct BinarySource*) self;
    bam_error err;
    unsigned r = 0;

	/* dlb_bitbuf_read causes an invalid read with 0 bits (lib v2.0.7) */
    if( width == 0)
    {
         *value = 0;
         return BAM_OK;
    }

    err = BAMSource_add_position(self, width);
    if(err != BAM_OK)
    {
        return err;
    }

    if (dlb_bitbuf_get_bits_left(&binarySource->bitbuf) < (int) width)
    {
        return BAM_PREMATURE_EOF_ERROR;
    }

    while (width > 16)
    {
        r = (r << 16) + dlb_bitbuf_read_long(&binarySource->bitbuf, 16);
        width -= 16;
    }

    *value = ((r << width) + dlb_bitbuf_read_long(&binarySource->bitbuf, width));

    return BAM_OK;
}

bam_error
BinarySource_push (struct BAMSource *self
                 , unsigned width, BAMint value)
{
    struct BinarySource *binarySource = (struct BinarySource*) self;

    if(dlb_bitbuf_get_abs_pos(&binarySource->bitbuf) < width)
    {
        return BAM_PUSH_ERROR;
    }

    long num_bits = (long) width;

    /* dlb_bitbuf_read and dlb_bitbuf_write both move the current position "forward".
        Pushing moves backwards. */
    dlb_bitbuf_skip(&binarySource->bitbuf, -num_bits);
    dlb_bitbuf_write_long(&binarySource->bitbuf, value, width);
    dlb_bitbuf_skip(&binarySource->bitbuf, -num_bits);

    return BAM_OK;
}

static void
BinarySource_destroy(struct BAMSource *self)
{
    free(self);
}


struct BAMSource *
BinarySource_create(unsigned char *buffer
                  , size_t size_in_bytes)
{
    struct BinarySource * binarySource = malloc(sizeof *binarySource);

    if (binarySource == NULL)
    {
        return NULL;
    }

    BAMSource_init(&binarySource->super);
	binarySource->super.next_frame = BinarySource_next_frame;
	binarySource->super.end_of_stream = BinarySource_end_of_stream;
	binarySource->super.read_uint = BinarySource_read_uint;
	binarySource->super.read_align = BinarySource_read_uint; /* reuse read_uint */
	binarySource->super.push = BinarySource_push;
	binarySource->super.destroy = BinarySource_destroy;

    dlb_bitbuf_init(&binarySource->bitbuf
                    , size_in_bytes  == 0 ?
                    (unsigned char *) 1 :
                    buffer, 8 * size_in_bytes);

    return &binarySource->super;
}
