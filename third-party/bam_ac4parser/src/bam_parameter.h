#ifndef BAMPARAMETER_H_INCLUDED
#define BAMPARAMETER_H_INCLUDED

#include <bam_types.h>

typedef enum
BAMPARAMETER_type
{
	INT,
	FLOAT,
	STRING,
	VECTOR_INT,
	VECTOR_FLOAT,
	NO_BAMPARAMETER_TYPE
} BAMPARAMETER_type;

/* opaque data type */
struct BAMParameter;
struct BAMVector;

/* returns NULL if BAMParameter could not be allocated,
   returns a new BAMParameter object					*/
const struct BAMParameter*
BAMParameter_create_int(const BAMint elements);

const struct BAMParameter*
BAMParameter_create_float(const BAMfloat elements);

const struct BAMParameter*
BAMParameter_create_vector_int(struct BAMVector* elements);

const struct BAMParameter*
BAMParameter_create_vector_float(struct BAMVector* elements);

const struct BAMParameter*
BAMParameter_create_string(const char* elements);

/* returns internal BAMParameter values */
BAMint
BAMParameter_as_int(struct BAMParameter* bamparameter);

BAMfloat
BAMParameter_as_float(struct BAMParameter* bamparameter);

struct BAMVector*
	BAMParameter_as_vector_int(struct BAMParameter* bamparameter);

struct BAMVector*
	BAMParameter_as_vector_float(struct BAMParameter* bamparameter);

const char*
BAMParameter_as_string(struct BAMParameter* bamparameter);

/* returns type of BAMParameter value (INT, STRING...) */
BAMPARAMETER_type
BAMParameter_return_parameter_type(struct BAMParameter* bamparameter);

bam_error
BAMParameter_destroy(const struct BAMParameter* bamparameter);

#endif
