#ifndef BAM_BINARY_SINK_H
#define BAM_BINARY_SINK_H

#include "bam.h"

#include "dlb_alps_native/utils/types.h"

typedef struct bam_binary_sink_t bam_binary_sink;

BAMSink *bam_binary_sink_create(
    unsigned char *buffer,
    size_t size_in_bytes);

alps_ret bam_binary_sink_get_presentations(
    bam_binary_sink *sink,
    alps_presentation **presentations,
    size_t *presentation_count
);

alps_ret bam_binary_sink_set_presentation_level(
    bam_binary_sink *sink,
    size_t presentation_index,
    unsigned int presentation_level
);

#endif /* BAM_BINARY_SINK_H */
