#ifndef DLB_ALPS_NATIVE_ALPS_AC4PARSER_ALPS_AC4PARSER_H
#define DLB_ALPS_NATIVE_ALPS_AC4PARSER_ALPS_AC4PARSER_H

#include <stddef.h>

#include "dlb_alps_native/utils/types.h"

alps_ret alps_ac4parser_set_active_presentation_id(
    unsigned char *frame_buf,
    size_t size_in_bytes,
    int active_presentation_id);

#endif /* DLB_ALPS_NATIVE_ALPS_AC4PARSER_ALPS_AC4PARSER_H */
