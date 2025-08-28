#ifndef BAM_MAP_H_INCLUDED
#define BAM_MAP_H_INCLUDED
#include <bam_types.h>
#include <bam_parameter.h>
#include <bam_vector.h>

/* opaque data type */
struct BAMMap;

/*  creates empty map,
    returns NULL if MAP could not be allocated */
struct BAMMap *
BAMMap_create(size_t key_size
              , size_t value_size);

/* Insert Key/Value as Map(INT -> INT) */
bam_error
BAMMap_insert_int_int   (struct BAMMap *bammap
                        , BAMint key
                        , BAMint value);

/* Insert Key/Value as Map(INT -> Vector(INT)) */
bam_error
BAMMap_insert_int_vector   (struct BAMMap *bammap
                            , BAMint key
                            , struct BAMVector *value);

/* Insert Key/Value as Map(INT -> VSTRING) */
bam_error
BAMMap_insert_int_string (struct BAMMap *bammap
                          , BAMint key
                          , char *value);

bam_error
BAMMap_insert_int_map (struct BAMMap *bammap
                       , BAMint key
                       , struct BAMMap *value);

BAMint
BAMMap_get_int  (const struct BAMMap *bammap
                , BAMint key);

struct BAMVector *
BAMMap_get_vector_int   (const struct BAMMap *bammap
                         , BAMint key);

char *
BAMMap_get_string (const struct BAMMap *bammap
                   , BAMint key);

struct BAMMap *
BAMMap_get_map(const struct BAMMap *bammap
               , BAMint key);

struct BAMVector *
BAMMap_get_keys(struct BAMMap *bammap);

struct BAMVector *
BAMMap_get_values(struct BAMMap *bammap);

void
BAMMap_destroy (struct BAMMap *bammap);

void
BAMMap_clear(struct BAMMap* bammap);

struct BAMMap *
BAMMap_deep_copy(struct BAMMap *bammap);

#endif
