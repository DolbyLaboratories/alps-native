#ifndef BAM_VECTOR_H_INCLUDED
#define BAM_VECTOR_H_INCLUDED
#include <bam_types.h>
#include <bam_parameter.h>

#include <stddef.h>
#include <bam_map.h>

struct BAMVector;

/* returns NULL if vector could not be allocated,
   returns a new object, with num_references = 1 */
struct BAMVector *
BAMVector_create(size_t element_size, int b_elements_are_strings);

/* increases the reference count of 'vector' by 1 and returns 'vector' */
struct BAMVector *
BAMVector_create_reference(struct BAMVector *bamvector);

/* number of elements in the vector */
BAMint
BAMVector_size(const struct BAMVector *bamvector);

/* returns pointer to the data array */
struct BAMParameter **
BAMVector_data_parameter(const struct BAMVector *bamvector);

BAMint *
BAMVector_data_int(struct BAMVector *bamvector);

BAMfloat *
BAMVector_data_float(struct BAMVector *bamvector);

char **
BAMVector_data_string(struct BAMVector *bamvector);

struct BAMMap **
BAMVector_data_map(struct BAMVector *bamvector);

/* returns an array of BAMVector* */
struct BAMVector **
BAMVector_data_vector_int(struct BAMVector *bamvector);

struct BAMVector **
BAMVector_data_vector_vector_int(struct BAMVector *bamvector);

struct BAMVector **
BAMVector_data_vector_vector_vector_int(struct BAMVector *bamvector);

struct BAMVector **
BAMVector_data_vector_vector_vector_vector_int (struct BAMVector *bamvector);

BAMint
BAMVector_find (struct BAMVector *bamvector
               , BAMint object_to_find);

int
BAMVector_find_string (const struct BAMVector *BAMVector
                       , const char *object_to_find);

/* appends element, grows internal buffer if necessary */

bam_error
BAMVector_push_vector_map(struct BAMVector *bamvector
                          , struct BAMMap *bammap);

bam_error
BAMVector_push_vector_int(struct BAMVector *bamvector
                          , BAMint element);

bam_error
BAMVector_push_vector_float(struct BAMVector *bamvector
                            , BAMfloat element);

bam_error
BAMVector_push_vector_vector_int(struct BAMVector *bamvector
                                 , struct BAMVector *element);

bam_error
BAMVector_push_vector_vector_vector_int(struct BAMVector *bamvector
                                        , struct BAMVector *element);

bam_error
BAMVector_push_vector_vector_vector_vector_int(struct BAMVector *bamvector
                                        , struct BAMVector *element);

bam_error
BAMVector_push_vector_vector_vector_vector_vector_int(struct BAMVector *bamvector
                                        , struct BAMVector *element);

bam_error
BAMVector_push_vector_string(struct BAMVector *bamvector
                             , const char *element);

bam_error
BAMVector_push_vector_parameter(struct BAMVector *BAMVector
                                , const struct BAMParameter *bamparameter);

/* sets vector size to zero */
bam_error
BAMVector_clear(struct BAMVector *bamvector);

bam_error
BAMVector_clear_parameter(struct BAMVector *bamvector);

struct BAMVector *
BAMVector_concat_vector_int (struct BAMVector *v1
							, struct BAMVector *v2);

struct BAMVector *
BAMVector_concat_int(const struct BAMVector *v1
					, const struct BAMVector *v2);

struct BAMVector*
BAMVector_concat_float (struct BAMVector *v1
						, struct BAMVector *v2);

struct BAMVector *
BAMVector_deep_copy_int(const struct BAMVector *bamvector);

struct BAMVector *
BAMVector_deep_copy_2d_int(const struct BAMVector *bamvector);

struct BAMVector *
BAMVector_deep_copy_3d_int(const struct BAMVector *bamvector);

bam_error
BAMVector_resize_vector_string (struct BAMVector *bamvector
                                , size_t new_size);

bam_error
BAMVector_resize_vector_int(struct BAMVector *bamvector
                            , size_t new_size);

bam_error
BAMVector_resize_vector_vector_int(struct BAMVector *bamvector
                                   , size_t new_size);

bam_error
BAMVector_resize_vector_vector_vector_int(struct BAMVector *bamvector
                                          , size_t new_size);

bam_error
BAMVector_resize_vector_vector_vector_vector_int (struct BAMVector *bamvector
                                                  , size_t new_size);

bam_error
BAMVector_resize_vector_vector_vector_vector_vector_int (struct BAMVector *bamvector
                                                         , size_t new_size);

/* decreases the reference count by one. If there are no more references, the memory is free'd */
void
BAMVector_destroy(struct BAMVector *bamvector
                  , int depth);

/* private */
void
destroy_string_element(struct BAMVector *bamvector
                       , int index);

#endif
