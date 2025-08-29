/***********************************************************************************************************************
 * Copyright (C) 2024 by Dolby International AB.
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
typedef struct
BamSink
{
	struct BAMSink super;
	unsigned int indent;

} BamSink;

static
bam_error
BamSink_before_frame(struct BAMSink* self)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_frame(struct BAMSink* self)
{
	return BAM_OK;
}

static
bam_error
BamSink_before_call(struct BAMSink* self
	, const char* fname
	, const struct BAMVector* args
	, const char* return_var_name)
{

	return BAM_OK;
}

static
bam_error
BamSink_write_uint(struct BAMSink* self,
	const char* varname,
	unsigned n,
	BAMint val)
{

	return BAM_OK;
}

static
bam_error
BamSink_write_align(struct BAMSink* self
	, unsigned num
	, unsigned value)
{

	return BAM_OK;
}


static
bam_error
BamSink_after_call_int(struct BAMSink* self,
	const char* fname,
	const char* return_var_name,
	BAMint return_value)
{

	return BAM_OK;
}

static
bam_error
BamSink_after_call_float(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, BAMfloat return_value)
{

	return BAM_OK;
}

static
bam_error
BamSink_after_call_string(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, const char* string_name)
{
	return BAM_OK;
}


static
bam_error
BamSink_after_call_void(struct BAMSink* self,
	const char* fname)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_call_vector_vector_vector_int(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, struct BAMVector* bamvector)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_call_vector_vector_int(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, struct BAMVector* bamvector)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_call_vector_int(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, struct BAMVector* bamvector)
{

	return BAM_OK;
}

static
bam_error
BamSink_after_call_map_int_int(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, struct BAMMap* bammap)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_assign_int(struct BAMSink* self,
	const char* varname,
	BAMint val)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_assign_string(struct BAMSink* self
	, const char* vname
	, const char* value)
{

	return BAM_OK;
}

static
bam_error
BamSink_after_assign_vector_int(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	return BAM_OK;
}


static bam_error
BamSink_after_assign_vector_float(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_assign_vector_string(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	return BAM_OK;
}



static
bam_error
BamSink_after_assign_vector_vector_int(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	return BAM_OK;
}

static bam_error
BamSink_after_assign_vector_vector_vector_int(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_assign_map_int_int(struct BAMSink* self
	, const char* varname
	, struct BAMMap* bammap)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_assign_map_int_vector_int(struct BAMSink* self
	, const char* name
	, struct BAMMap* bammap)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_assign_map_vector_int_int(struct BAMSink* self
	, const char* name
	, struct BAMMap* bammap)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_assign_map_int_map_int_vector_int(struct BAMSink* self
	, const char* varname
	, struct BAMMap* bammap)
{

	return BAM_OK;
}

static
bam_error
BamSink_position(struct BAMSink* self
	, const char* varname
	, BAMint value)
{
	return BAM_OK;
}

static
bam_error
BamSink_after_push(struct BAMSink* self
	, unsigned num
	, BAMint value)
{
	return BAM_OK;
}

static
bam_error
BamSink_write_code(struct BAMSink* self,
	const char* code_name,
	const char* value_name,
	BAMint code,
	unsigned length,
	BAMint value)
{
	return BAM_OK;
}

void
BAMSink_init (struct BAMSink *self)
{
	/* self->method_with_default_implementation = Base_method_with_default_implementation */
	self->before_frame = BamSink_before_frame; 
	self->after_frame = BamSink_after_frame;
	self->before_call = BamSink_before_call;
	self->write_uint = BamSink_write_uint;
	self->write_code = BamSink_write_code;
	self->write_align = BamSink_write_align;
	self->after_call_int = BamSink_after_call_int;
	self->after_call_float = BamSink_after_call_float;
	self->after_call_vector_int = BamSink_after_call_vector_int;
	self->after_call_vector_vector_int = BamSink_after_call_vector_vector_int;
	self->after_call_vector_vector_vector_int = BamSink_after_call_vector_vector_vector_int;
	self->after_call_void = BamSink_after_call_void;
	self->after_call_string = BamSink_after_call_string;
	self->after_call_map_int_int = BamSink_after_call_map_int_int;
	self->after_assign_int = BamSink_after_assign_int;
	self->after_assign_string = BamSink_after_assign_string;
	self->after_assign_vector_int = BamSink_after_assign_vector_int;
	self->after_assign_vector_float = BamSink_after_assign_vector_float;
	self->after_assign_vector_string = BamSink_after_assign_vector_string;
	self->after_assign_vector_vector_int = BamSink_after_assign_vector_vector_int;
	self->after_assign_vector_vector_vector_int = BamSink_after_assign_vector_vector_vector_int;
	self->after_assign_map_int_int = BamSink_after_assign_map_int_int;
	self->after_assign_map_vector_int_int = BamSink_after_assign_map_vector_int_int;
	self->after_assign_map_int_vector_int = BamSink_after_assign_map_int_vector_int;
	self->after_assign_map_int_map_int_vector_int = BamSink_after_assign_map_int_map_int_vector_int;
	self->after_push = BamSink_after_push;
	self->position = BamSink_position;
}
