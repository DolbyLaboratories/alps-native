#ifndef DLB_ALPS_NATIVE_ALPS_MP4DMX_ALPS_MP4DMX_H
#define DLB_ALPS_NATIVE_ALPS_MP4DMX_ALPS_MP4DMX_H

#include <stddef.h>

#include "dlb_alps_native/utils/types.h"

typedef struct alps_mp4dmx_t alps_mp4dmx;

typedef struct alps_mp4dmx_preselection_t
{
    unsigned int num_entities_in_group;
    unsigned int *entity_ids;
    int preselection_tag;
    char *label;
    char *extended_language;
} alps_mp4dmx_preselection;

alps_ret alps_mp4dmx_query_mem(
    size_t *mem_size
);

alps_ret alps_mp4dmx_init(
    alps_mp4dmx **dmx_ptr,
    void         *mem
);

void alps_mp4dmx_destroy(
    alps_mp4dmx   *dmx
);

alps_ret alps_mp4dmx_process_buffer(
    alps_mp4dmx   *dmx,
    unsigned char *buffer,
    size_t         size
);

alps_ret alps_mp4dmx_next_sample(
    alps_mp4dmx    *dmx,
    unsigned char **data,
    size_t         *size
);

alps_ret alps_mp4dmx_get_preselections(
    alps_mp4dmx               *dmx,
    alps_mp4dmx_preselection **preselections,
    size_t                    *count
);

#endif /* DLB_ALPS_NATIVE_ALPS_MP4DMX_ALPS_MP4DMX_H */
