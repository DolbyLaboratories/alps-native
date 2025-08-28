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
