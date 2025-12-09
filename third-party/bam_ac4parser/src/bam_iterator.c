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

#include <bam_iterator.h>
#include <bam_vector.h>
#include <stdio.h>
#include <string.h>

bam_error
after (struct BAMSource * BAM_source
       , struct BAMSink * BAM_sink
       , const char *name)
{
    bam_error err;
    long BAM_align;
    long BAM_align_val;

    err = BAM_sink->after_call_void(BAM_sink, name);
    if(err != BAM_OK)
    {
        return err;
    }
    /* Align after frame */
    {
        BAM_align = (8 - BAMSource_get_position(BAM_source) % 8) % 8;

        err = BAM_source->read_align(BAM_source, BAM_align, &BAM_align_val);
        if(err != BAM_OK)
        {
            return err;
        }

        err = BAM_sink->write_align(BAM_sink, BAM_align, BAM_align_val);
        if(err != BAM_OK)
        {
            return err;
        }

        return BAM_OK;
    }
    
	err = BAM_sink->after_frame(BAM_sink);
	if (err != BAM_OK)
	{
		return err;
	}
    return BAM_OK;
}

bam_error
iterate(struct BAMSource *BAM_source
        , struct BAMSink *BAM_sink
        , struct BAMSyntax *BAM_Syntax
        , struct BAMVector *param_vector
        , int b_return_value
        , const char *class_name
        , unsigned long from
        , unsigned long to
        , BAMint *returnvalue)
{
    bam_error err;
    unsigned long frame = 0;
    int done = 0;

    while ((to == 0
          || frame < to)
          && done != 1)
    {
        if (BAM_source->next_frame(BAM_source, (int*) 1) == BAM_OK)
        {
            err = BAM_Syntax->reset(BAM_Syntax);
            if(err != BAM_OK)
            {

                return err;
            }

            err = BAM_sink->before_frame(BAM_sink);
			if (err != BAM_OK)
			{
				return err;

			}

            err = BAM_sink->before_call(BAM_sink, class_name, param_vector, NULL);  /* No return var */
            if(err != BAM_OK)
            {
                return err;

            }

            err = BAM_Syntax->set_input(BAM_Syntax, param_vector);
            if (err != BAM_OK)
            {
                return err;
            }

            err = BAM_Syntax->parse_generic(BAM_Syntax, BAM_source, BAM_sink);
            if (err != BAM_OK)
            {
                return err;
            }

            err = after(BAM_source, BAM_sink, class_name);
            if(err != BAM_OK)
            {
                return err;
            }

            frame++;
        }

        else
        {
            done = 1;
        }
    }

    if (b_return_value)
    {
        *returnvalue  = BAM_Syntax->get_result(BAM_Syntax);
        return BAM_OK;
    }

    else
    {
        /* undefined value */
        return BAM_OK;
    }
    return BAM_OK;
}
