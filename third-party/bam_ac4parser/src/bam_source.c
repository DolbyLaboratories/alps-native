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
