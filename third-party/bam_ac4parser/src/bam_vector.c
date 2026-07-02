/***********************************************************************************************************************
 * Copyright (C) 2024-2026 by Dolby International AB.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 *    disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *    following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************************************************************/

#include <bam.h>
#include <stdio.h>
#include <stdlib.h>
#include <bam_vector.h>
#include <bam_parameter.h>
#define DEFAULT_VECTOR_SIZE 16

#define define_BAMVector_push(T, TYPE) \
bam_error BAMVector_push_##T(BAMVector *a, TYPE element) \
{ \
    if( a->used >= a->limit) \
    {                        \
        a->limit *= 2; \
        a->data = (TYPE *)realloc(a->data, a->limit * sizeof(element)); \
        \
        if(a == NULL) \
        { \
            free(a); \
            return BAM_REALLOC_ERROR; \
        }  \
    } \
    \
   (( TYPE *) a->data)[a->used++] = element; \
   return BAM_OK; \
}

typedef struct
BAMVector
{
    void *data;
    size_t limit;
    size_t used;
    int b_elements_are_strings;
    int number_of_references;

} BAMVector;

BAMint
BAMVector_size(const BAMVector *a)
{
     return (BAMint) a->used;
}

/* BAMVector push implementation for various types */

define_BAMVector_push(vector_int, BAMint);

define_BAMVector_push(vector_float, BAMfloat);

define_BAMVector_push(vector_vector_int, BAMVector *);

define_BAMVector_push(vector_vector_vector_int, BAMVector *);

define_BAMVector_push(vector_vector_vector_vector_int, BAMVector *);

define_BAMVector_push(vector_vector_vector_vector_vector_int, BAMVector *);

define_BAMVector_push(vector_string, const char *);

define_BAMVector_push(vector_parameter, const struct BAMParameter *);

define_BAMVector_push(vector_map, struct BAMMap *);

/* BAMVector concat implementation for various types */
struct BAMVector *
BAMVector_concat_float (struct BAMVector* v1
						, struct BAMVector* v2)
{
	struct BAMVector* bamvector = BAMVector_create(sizeof(BAMfloat), 0);

	int loop_vector1;
	int loop_vector2;

	for (loop_vector1 = 0; loop_vector1 < BAMVector_size(v1); loop_vector1++)
	{
		BAMVector_push_vector_float(bamvector, ((BAMfloat*)v1->data)[loop_vector1]);
	}

	for (loop_vector2 = 0; loop_vector2 < BAMVector_size(v2); loop_vector2++)
	{
		BAMVector_push_vector_float(bamvector, ((BAMfloat*)v2->data)[loop_vector2]);
	}

	return bamvector;
}
struct BAMVector *
BAMVector_concat_vector_int( struct BAMVector *v1
							,  struct BAMVector*v2)
{
	struct BAMVector* bamvector = BAMVector_create(sizeof(BAMVector *), 0);

	int loop_vector1;
	int loop_vector2;

	for (loop_vector1 = 0; loop_vector1 < BAMVector_size(v1); loop_vector1++)
	{
		BAMVector_push_vector_vector_int(bamvector, BAMVector_deep_copy_int(BAMVector_data_vector_int(v1)[loop_vector1]));
	}

	for (loop_vector2 = 0; loop_vector2 < BAMVector_size(v2); loop_vector2++)
	{
		BAMVector_push_vector_vector_int(bamvector, BAMVector_deep_copy_int(BAMVector_data_vector_int(v2)[loop_vector2]));
	}

	return bamvector;
}

struct BAMVector *
BAMVector_concat_int(const struct BAMVector *v1
					 , const struct BAMVector *v2)
{
    struct BAMVector * bamvector = BAMVector_create(sizeof(BAMint), 0);

    int loop_vector1;
    int loop_vector2;

    for (loop_vector1 = 0; loop_vector1 < BAMVector_size(v1); loop_vector1++)
    {
       BAMVector_push_vector_int(bamvector, ((BAMint *)v1->data)[loop_vector1]);
    }

    for (loop_vector2 = 0; loop_vector2 < BAMVector_size(v2); loop_vector2 ++)
    {
        BAMVector_push_vector_int(bamvector, ((BAMint *)v2->data)[loop_vector2]);
    }
	
    return bamvector;
}

struct BAMVector *
BAMVector_deep_copy_int(const struct BAMVector *bamvector)
{
    struct BAMVector *vector_copy = BAMVector_create(sizeof(BAMint), 0);
    int loop_vector;

    for(loop_vector = 0; loop_vector < BAMVector_size(bamvector); loop_vector++)
    {
        BAMVector_push_vector_int(vector_copy, ((BAMint *)bamvector->data)[loop_vector]);
    }

    return vector_copy;
}


struct BAMVector *
BAMVector_deep_copy_2d_int(const struct BAMVector *bamvector)
{
    struct BAMVector *vector_copy_2d = BAMVector_create(sizeof(struct BAMVector *), 0);
    int loop_vector;

    for(loop_vector = 0; loop_vector < BAMVector_size(bamvector); loop_vector++)
    {
       BAMVector_push_vector_vector_int(vector_copy_2d, BAMVector_deep_copy_int(((struct BAMVector **)bamvector->data)[loop_vector]));
    }

    return vector_copy_2d;
}

struct BAMVector *
BAMVector_deep_copy_3d_int(const struct BAMVector *bamvector)
{
    struct BAMVector *vector_copy_3d = BAMVector_create(sizeof(struct BAMVector *), 1);
    int loop_vector;

    for(loop_vector = 0; loop_vector < BAMVector_size(bamvector); loop_vector++)
    {
       BAMVector_push_vector_vector_vector_int(vector_copy_3d, BAMVector_deep_copy_2d_int(((struct BAMVector **)bamvector->data)[loop_vector]));
    }

    return vector_copy_3d;
}

BAMint *
BAMVector_data_int(BAMVector *a)
{
    return ((BAMint *) a->data);
}

char **
BAMVector_data_string(BAMVector *a)
{
    return a->data;
}

struct BAMVector **
BAMVector_data_vector_int(BAMVector *a)
{
    return a->data;
}

struct BAMVector **
BAMVector_data_vector_vector_int(BAMVector *a)
{
    return a->data;
}

struct BAMVector **
BAMVector_data_vector_vector_vector_int(BAMVector *a)
{
    return a->data;
}

struct BAMVector **
BAMVector_data_vector_vector_vector_vector_int(BAMVector *a)
{
    return a->data;
}

BAMfloat *
BAMVector_data_float(BAMVector *a)
{
    return ((BAMfloat *) a->data);
}

struct BAMParameter **
BAMVector_data_parameter(const BAMVector *a)
{
    return ((struct BAMParameter **)a->data);
}

struct BAMMap **
BAMVector_data_map(BAMVector *a)
{
    return ((struct BAMMap **)a->data);
}

void
BAMVector_destroy(BAMVector *a, int depth)
{
    a->number_of_references = a->number_of_references - 1;

    if(a->number_of_references <= 0)
    {
        if (depth > 1)
        {
            int i;
            for(i = 0; i < BAMVector_size(a); i++)
            {
                BAMVector_destroy(BAMVector_data_vector_int(a)[i], depth - 1);
            }
        }

        if(a->b_elements_are_strings == 1 && depth > 1)
        {
            int i;
            for(i = 0; i < BAMVector_size(a); i++)
            {
				destroy_string_element(a, i);
            }
        }
		
        free(a->data);
        a->data = NULL;
        a->used = 0;
		
        free(a);
    }
}

bam_error
BAMVector_clear(BAMVector *a)
{
	a->used = 0;
    return BAM_OK;
}

bam_error
BAMVector_clear_parameter(BAMVector  *a)
{
	int i;
	
	for (i = 0; i < BAMVector_size(a); i++)
	{
		BAMParameter_destroy(BAMVector_data_parameter(a)[i]);
	}
	a->used = 0;
	return BAM_OK;
}

void
destroy_string_element(BAMVector *a, int index)
{
    free(BAMVector_data_string(a)[index]);
}

bam_error
BAMVector_resize_vector_string(BAMVector *a,
                               size_t new_size)
{
    int i;
    bam_error err;

    for(i = a->used; i < ((BAMint)new_size); i++)
    {
        err = BAMVector_push_vector_string(a, "Extra");
        if(err != BAM_OK)
        {
            return err;
        }
    }

    return BAM_OK;
}

bam_error
BAMVector_resize_vector_int(BAMVector *a, size_t new_size)
{
    int i;
    bam_error err;

    for(i = a->used; i < ((BAMint)new_size); i++)
    {
        err = BAMVector_push_vector_int(a, 0);
        if(err != BAM_OK)
        {
            return err;
        }
    }

    return BAM_OK;
}

bam_error
BAMVector_resize_vector_vector_int (BAMVector *a, size_t new_size)
{
    int i;
    bam_error err;

    for(i = a->used; i < ((BAMint)new_size); i++)
    {
        err = BAMVector_push_vector_vector_int(a, BAMVector_create(sizeof(struct BAMVector*), 0));
        if(err != BAM_OK)
        {
            return err;
        }
    }

    return BAM_OK;
}

bam_error
BAMVector_resize_vector_vector_vector_int (BAMVector *a, size_t new_size)
{
    int i;
    bam_error err;
	
    for(i = a->used; i < ((BAMint)new_size); i++)
    {
        err = BAMVector_push_vector_vector_vector_int(a, BAMVector_create(sizeof(struct BAMVector *), 0));
        if(err != BAM_OK)
        {
            return err;
        }
    }

    return BAM_OK;
}

bam_error
BAMVector_resize_vector_vector_vector_vector_int  (BAMVector *a
                                                   , size_t new_size)
{
    int i;
    bam_error err;

    for(i = a->used; i < ((BAMint)new_size); i++)
    {
        err = BAMVector_push_vector_vector_vector_vector_int(a, BAMVector_create(sizeof(struct BAMVector *), 0));
        if(err != BAM_OK)
        {
            return err;
        }
    }

    return BAM_OK;
}

bam_error
BAMVector_resize_vector_vector_vector_vector_vector_int  (BAMVector *a
                                                          , size_t new_size)
{
    int i;
    bam_error err;

    for(i = a->used; i < ((BAMint)new_size); i++)
    {
        err = BAMVector_push_vector_vector_vector_vector_vector_int(a, BAMVector_create(sizeof(struct BAMVector *), 0));
        if(err != BAM_OK)
        {
            return err;
        }
    }

    return BAM_OK;
}

BAMint
BAMVector_find (BAMVector *a
               , BAMint object_to_find)
{
    int found = 0;
    int i = 0;

    for(i = 0; i < BAMVector_size(a); i++)
    {
        if(((BAMint *)a->data)[i] == object_to_find)
        {
            found = found + 1;
        }
    }

    if (found > 0)
    {
        return 1;
    }

    else
    {
        return 0;
    }
}

int
BAMVector_find_string (const BAMVector *a
                       , const char *object_to_find)
{
    int found = 0;
    int i = 0;

    for(i = 0; i < BAMVector_size(a); i++)
    {
        if (strcmp(((const char**)a->data)[i], object_to_find) == 0)
        {
            found = found + 1;
        }
    }

    if (found > 0)
    {
        return 1; /* boolean */
    }

    else
    {
        return 0;
    }
}

struct BAMVector *
BAMVector_create_reference(BAMVector *a)
{
    a->number_of_references = a->number_of_references + 1;
    return a;
}

struct BAMVector *
BAMVector_create(size_t element_size, int b_elements_are_strings)
{
    BAMVector *bamvector = malloc(sizeof *bamvector);
    if (bamvector == NULL)
    {
        free(bamvector);
        return 0;
    }

    bamvector->data = malloc(element_size * DEFAULT_VECTOR_SIZE);
    if (bamvector->data == NULL)
    {
        free(bamvector);
        return 0;
    }

    bamvector->b_elements_are_strings = b_elements_are_strings;
    bamvector->limit = DEFAULT_VECTOR_SIZE;
    bamvector->used = 0;
    bamvector->number_of_references = 1;

    return bamvector;
}
