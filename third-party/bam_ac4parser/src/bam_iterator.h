#ifndef ITERATOR_H_INCLUDED
#define ITERATOR_H_INCLUDED
#include <bam_vector.h>
#include <bam.h>

/* Iterates over given frames.
   Returns: result of last frame, if function returns any result. */
bam_error
iterate(struct BAMSource *BAM_source
        , struct BAMSink *BAM_sink
        , struct BAMSyntax *BAM_syntax
        , struct BAMVector *param_vector
        , int b_return_value
        , const char *name
        , unsigned long from 
        , unsigned long to
        , BAMint *returnvalue
        );

#endif
