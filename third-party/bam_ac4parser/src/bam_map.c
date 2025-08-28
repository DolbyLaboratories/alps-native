#include <bam.h>
#include <stdio.h>
#include <stdlib.h>
#include <bam_map.h>
#include <bam_parameter.h>
#include <bam_vector.h>
#include <assert.h>

#define define_BAMMap_insert(T1, T2, TYPE1, TYPE2) \
bam_error BAMMap_insert_##T1(BAMMap *bammap, TYPE1 key, TYPE2 value) \
{ \
    bam_error err; \
    \
    err = BAMVector_push_vector_int(bammap->keys, key); \
    if(err != BAM_OK) \
    {   \
        return err; \
    } \
    \
    err = BAMVector_push_vector##T2(bammap->values, value); \
    if(err != BAM_OK) \
    {   \
        return err; \
    } \
    return BAM_OK; \
}


typedef struct
BAMMap
{
    struct BAMVector *keys;
    struct BAMVector *values;
} BAMMap;

/* BAMMap_insert implementation for various types */
define_BAMMap_insert(int_int, _int, BAMint, BAMint);

define_BAMMap_insert(int_vector, _vector_int, BAMint, struct BAMVector *);

define_BAMMap_insert(int_string, _string, BAMint, char *);

bam_error
BAMMap_insert_int_map (struct BAMMap *bammap
                       , BAMint key
                       , struct BAMMap *values)
{
    bam_error err;

    err = BAMVector_push_vector_int(bammap->keys, key);
    if (err != BAM_OK)
    {
        return err;
    }

    err = BAMVector_push_vector_map(bammap->values, values);
    if(err != BAM_OK)
    {
        return err;
    }

    return BAM_OK;
}



BAMint
BAMMap_get_int(const BAMMap *bammap
               , BAMint key)
{
    int i;

    for(i = 0; i < BAMVector_size(bammap->keys); i++)
    {
        if (BAMVector_data_int(bammap->keys)[i] == key) 
        {
            assert(BAMVector_data_int(bammap->keys)[i] == key);
            return BAMVector_data_int(bammap->values)[i];
        }
    }

    return BAMMAP_NO_KEY_VALUE;
}

struct BAMVector *
BAMMap_get_vector_int(const BAMMap *bammap
                      , BAMint key)
{
    int i;

    for(i = 0; i < BAMVector_size(bammap->keys); i++)
    {
        if (BAMVector_data_int(bammap->keys)[i] == key) 
        {
            return BAMVector_data_vector_int(bammap->values)[i];
        }
    }
	assert(0); /* key lookup error */
}

char *
BAMMap_get_string(const BAMMap *bammap
                  , BAMint key)
{
    int i;

    for(i = 0; i < BAMVector_size(bammap->keys); i++)
    {
        if (BAMVector_data_int(bammap->keys)[i] == key)
        {
            return BAMVector_data_string(bammap->values)[i];
        }
    }
	assert(0); /* key lookup error */
}

struct BAMMap *
BAMMap_get_map (const BAMMap *bammap
                , BAMint key)
{
    int i;

    for(i = 0; i < BAMVector_size(bammap->keys); i++)
    {
        if (BAMVector_data_int(bammap->keys)[i] == key)
        {
            return BAMVector_data_map(bammap->values)[i];
        }
    }
	assert(0); /* key lookup error */
}

struct BAMVector *
BAMMap_get_keys(BAMMap *bammap)
{
    return bammap->keys;
}

struct BAMVector *
BAMMap_get_values(BAMMap *bammap)
{
    return bammap->values;
}

struct BAMMap *
BAMMap_deep_copy(BAMMap *bammap)
{
    struct BAMMap *map_copy = BAMMap_create(sizeof(BAMint), sizeof(BAMint));
    int loop_map;

    for (loop_map = 0; loop_map < BAMVector_size(bammap->keys); loop_map++)
    {
        BAMVector_push_vector_int(map_copy->keys, BAMVector_data_int(bammap->keys)[loop_map]);
    }

    for (loop_map = 0; loop_map < BAMVector_size(bammap->values); loop_map++)
    {
        BAMVector_push_vector_int(map_copy->values, BAMVector_data_int(bammap->values)[loop_map]);
    }

    return map_copy;
}

void
BAMMap_destroy(BAMMap *bammap)
{
	BAMVector_destroy(bammap->values, 1);
    BAMVector_destroy(bammap->keys, 1);
    free(bammap);
}

void
BAMMap_clear(BAMMap* bammap)
{
	bammap->values = NULL;
	bammap->keys = NULL;
}

struct BAMMap *
BAMMap_create(size_t key_size, size_t value_size)
{
    BAMMap *bammap = malloc(sizeof *bammap);
    if (bammap == NULL)
    {
        free(bammap);
        return 0;
    }

    bammap->keys    = BAMVector_create(key_size, 0);
    bammap->values  = BAMVector_create(value_size, 0);

    return bammap;
}
