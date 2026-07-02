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
#include <bam_parameter.h>
#include <assert.h>
#include <bam_vector.h>
#include <stdio.h>
#include <stdlib.h>

typedef
struct BAMParameter
{
	BAMPARAMETER_type bamparameter_type;
	BAMint int_value;
	BAMfloat float_value;
	const char* string_value;
	struct BAMVector* int_vector;
	struct BAMVector* float_vector;

} BAMParameter;

const struct BAMParameter*
BAMParameter_create_int(const BAMint elements)
{
	BAMParameter* bamparameter = malloc(sizeof * bamparameter);
	if (bamparameter == NULL)
	{
		return NULL;
	}

	bamparameter->int_value = elements;
	bamparameter->bamparameter_type = INT;

	return bamparameter;
}

const struct BAMParameter*
BAMParameter_create_string(const char* elements)
{
	BAMParameter* bamparameter = malloc(sizeof * bamparameter);
	if (bamparameter == NULL)
	{
		return NULL;
	}

	bamparameter->string_value = elements;
	bamparameter->bamparameter_type = STRING;

	return bamparameter;
}

const struct BAMParameter*
BAMParameter_create_float(const BAMfloat elements)
{
	BAMParameter* bamparameter = malloc(sizeof * bamparameter);
	if (bamparameter == NULL)
	{
		return NULL;
	}

	bamparameter->float_value = elements;
	bamparameter->bamparameter_type = FLOAT;

	return bamparameter;
}

const struct BAMParameter*
BAMParameter_create_vector_int(struct BAMVector* elements)
{
	BAMParameter* bamparameter = malloc(sizeof * bamparameter);
	if (bamparameter == NULL)
	{
		return NULL;
	}

	bamparameter->int_vector = elements;
	bamparameter->bamparameter_type = VECTOR_INT;

	return bamparameter;
}

const struct BAMParameter*
BAMParameter_create_vector_float(struct BAMVector* elements)
{
	BAMParameter* bamparameter = malloc(sizeof * bamparameter);
	if (bamparameter == NULL)
	{
		return NULL;
	}

	bamparameter->float_vector = elements;
	bamparameter->bamparameter_type = VECTOR_FLOAT;
	return bamparameter;
}

BAMint
BAMParameter_as_int(struct BAMParameter* bamparameter)
{
	return bamparameter->int_value;
}

BAMfloat
BAMParameter_as_float(struct BAMParameter* bamparameter)
{
	return bamparameter->float_value;
}

struct BAMVector*
	BAMParameter_as_vector_float(struct BAMParameter* bamparameter)
{
	return bamparameter->float_vector;
}

struct BAMVector*
	BAMParameter_as_vector_int(struct BAMParameter* bamparameter)
{
	return bamparameter->int_vector;
}

const char*
BAMParameter_as_string(struct BAMParameter* bamparameter)
{
	return bamparameter->string_value;
}

BAMPARAMETER_type
BAMParameter_return_parameter_type(struct BAMParameter* bamparameter)
{
	if (bamparameter->bamparameter_type == INT)
	{
		return INT;
	}
	if (bamparameter->bamparameter_type == STRING)
	{
		return STRING;
	}
	if (bamparameter->bamparameter_type == VECTOR_INT)
	{
		return VECTOR_INT;
	}
	if (bamparameter->bamparameter_type == FLOAT)
	{
		return FLOAT;
	}
	if (bamparameter->bamparameter_type == VECTOR_FLOAT)
	{
		return VECTOR_FLOAT;
	}
	else
	{
		return NO_BAMPARAMETER_TYPE;
	}
}

bam_error
BAMParameter_destroy(const struct BAMParameter* bamparameter)
{
	free((struct BAMParameter*)bamparameter);
	return BAM_OK;
}
