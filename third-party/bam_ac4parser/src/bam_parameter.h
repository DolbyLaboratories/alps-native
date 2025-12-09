/***********************************************************************************************************************
 * Copyright (C) 2024-2025 by Dolby International AB.
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
