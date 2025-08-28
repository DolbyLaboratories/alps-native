#include <stdlib.h>
#include <bam.h>
#include <bam_vector.h>
#include <bam_map.h>
#include <bam_null_sink.h>
#include <stdio.h>
#include <bam_parameter.h>
#include <string.h>
#include <assert.h>

typedef struct
NullSink
{
    struct BAMSink super;
    unsigned int indent;

} NullSink;

static void
NullSink_destroy(struct BAMSink *self)
{
    free(self);
}

struct BAMSink *
NullSink_create()
{
    struct NullSink *nullsink = malloc(sizeof * nullsink);
	nullsink->indent = 0;

    if (nullsink == NULL)
    {
        free(nullsink);
        return NULL;
    }

    BAMSink_init(&nullsink->super);
	nullsink->super.destroy									= NullSink_destroy;

    return &nullsink->super;
};
