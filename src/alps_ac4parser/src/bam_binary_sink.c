#include "bam_binary_sink.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "dlb_bitbuf.h"
#include "dlb_bitbuf_write.h"

#include "dlb_alps_native/utils/utils.h"

typedef struct presentation_level_t
{
    unsigned long offset; /* offset in bits of presentation_level field in sample buffer */
    uint8_t value;        /* value of presentation_level field */
} presentation_level;

struct bam_binary_sink_t
{
    struct BAMSink      super;
    dlb_bitbuf          bitbuf;              /* bit buffer used to track position and write to an AC-4 sample buffer */
    alps_presentation  *presentations;       /* array of presentations */
    presentation_level *levels;              /* array of presentation_level fields; each entry refers to an entry in presentations array with the same index */
    size_t              presentations_count; /* number of entries in presentations and levels arrays */
    size_t              curr_pres;           /* currently parsed presentation */
};

static bam_error bam_binary_sink_write_align(
    struct BAMSink *self,
    unsigned int num_bits,
    unsigned int value)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;

    dlb_bitbuf_skip(&sink->bitbuf, num_bits);

    return err;
}

static bam_error bam_binary_sink_write_uint(
    struct BAMSink *self,
    const char *varname,
    unsigned int num_bits,
    BAMint val)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;

    if (!strcmp(varname, "presentation_level"))
    {
        sink->levels[sink->curr_pres].offset = dlb_bitbuf_get_abs_pos(&sink->bitbuf);
        sink->levels[sink->curr_pres].value = (uint8_t)val;
    }

    dlb_bitbuf_skip(&sink->bitbuf, num_bits);

    return err;
}

static bam_error bam_binary_sink_after_assign_int(
    struct BAMSink *self,
    const char *varname,
    BAMint val)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;
    size_t i;

    /* reallocate presentations array if needed */
    if (!strcmp(varname, "n_presentations"))
    {
        if ((size_t)val > sink->presentations_count)
        {
            sink->presentations = realloc(sink->presentations, val * sizeof(*sink->presentations));
            sink->levels = realloc(sink->levels, val * sizeof(*sink->levels));
        }

        memset(sink->presentations, 0, sizeof(*sink->presentations) * val);
        memset(sink->levels, 0, sizeof(*sink->levels) * val);
        sink->presentations_count = val;
        sink->curr_pres = 0;

        for (i = 0; i < sink->presentations_count; i++)
        {
            sink->presentations[i].presentation_id = ALPS_INVALID_PRES_ID;
        }
    }

    return err;
}

static bam_error bam_binary_sink_after_call_void(
    struct BAMSink *self,
    const char *fname)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;

    if (!strcmp(fname, "ac4_presentation_info") || !strcmp(fname, "ac4_presentation_v2_info"))
    {
        sink->curr_pres++;
    }

    return err;
}

static bam_error bam_binary_sink_after_call_int(
    struct BAMSink *self,
    const char *fname,
    const char *return_var_name,
    BAMint return_value)
{
    bam_error err = BAM_OK;
    bam_binary_sink *sink = (bam_binary_sink*)self;

    if (!strcmp(return_var_name, "presentation_id"))
    {
        sink->presentations[sink->curr_pres].presentation_id = return_value;
    }

    return err;
}

static void bam_binary_sink_destroy(
    BAMSink *self)
{
    bam_binary_sink *sink = (bam_binary_sink*)self;

    if (sink)
    {
        free(sink->presentations);
        free(sink->levels);
    }

    free(self);
}

BAMSink *bam_binary_sink_create(
    unsigned char *buffer,
    size_t size_in_bytes)
{
    bam_binary_sink *sink;

    if (buffer == NULL || size_in_bytes == 0)
    {
        return NULL;
    }

    sink = malloc(sizeof(*sink));
    if (sink == NULL)
    {
        return NULL;
    }

    BAMSink_init(&sink->super);

    sink->super.write_align = bam_binary_sink_write_align;
    sink->super.write_uint = bam_binary_sink_write_uint;
    sink->super.after_assign_int = bam_binary_sink_after_assign_int;
    sink->super.after_call_void = bam_binary_sink_after_call_void;
    sink->super.after_call_int = bam_binary_sink_after_call_int;
    sink->super.destroy = bam_binary_sink_destroy;

    dlb_bitbuf_init(&sink->bitbuf, buffer, 8 * size_in_bytes);
    sink->presentations = NULL;
    sink->levels = NULL;
    sink->presentations_count = 0;
    sink->curr_pres = 0;

    return &sink->super;
}

alps_ret bam_binary_sink_get_presentations(
    bam_binary_sink *sink,
    alps_presentation **presentations,
    size_t *presentation_count)
{
    alps_ret ret = ALPS_RET_OK;

    CHECK_EXPR_AND_BAIL(!sink || !presentations || !presentation_count, ALPS_RET_E_INVALID_ARG);

    *presentations = sink->presentations;
    *presentation_count = sink->presentations_count;

bail:
    return ret;
}

alps_ret bam_binary_sink_set_presentation_level(
    bam_binary_sink *sink,
    size_t presentation_index,
    unsigned int presentation_level)
{
    alps_ret ret = ALPS_RET_OK;
    size_t current_position;

    CHECK_EXPR_AND_BAIL(!sink || presentation_index >= sink->presentations_count || presentation_level > 7, ALPS_RET_E_INVALID_ARG);

    current_position = dlb_bitbuf_get_abs_pos(&sink->bitbuf);

    dlb_bitbuf_set_abs_pos(&sink->bitbuf, sink->levels[presentation_index].offset);
    dlb_bitbuf_write(&sink->bitbuf, presentation_level, 3);

    dlb_bitbuf_set_abs_pos(&sink->bitbuf, current_position);

bail:
    return ret;
}
