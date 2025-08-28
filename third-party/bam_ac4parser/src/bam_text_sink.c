#include <stdlib.h>
#include <bam.h>
#include <bam_vector.h>
#include <bam_map.h>
#include <bam_text_sink.h>
#include <stdio.h>
#include <bam_parameter.h>
#include <string.h>
#include <assert.h>

typedef struct
TextSink
{
	struct BAMSink super;
	unsigned int indent;
} TextSink;

static void
print_vector_float(struct BAMVector* bamvector)
{
	int i;

	printf("[");
	for (i = 0; i < BAMVector_size(bamvector); i++)
	{
		printf("%lf", BAMVector_data_float(bamvector)[i]);
		if (i < BAMVector_size(bamvector) - 1)
		{
			printf(", ");
		}
	}
	printf("]");
}

static void
print_vector_int(struct BAMVector* bamvector)
{
	int i;

	printf("[");
	for (i = 0; i < BAMVector_size(bamvector); i++)
	{
		printf("%ld", BAMVector_data_int(bamvector)[i]);
		if (i < BAMVector_size(bamvector) - 1)
		{
			printf(", ");
		}
	}
	printf("]");
}

static void
print_vector_vector_int(struct BAMVector* bamvector)
{
	int i;

	printf("[");
	for (i = 0; i < BAMVector_size(bamvector); i++)
	{
		print_vector_int(BAMVector_data_vector_int(bamvector)[i]);
		if (i < BAMVector_size(bamvector) - 1)
		{
			printf(", ");
		}
	}
	printf("]");
}

static void
print_vector_vector_vector_int(struct BAMVector* bamvector)
{
	int i;

	printf("[");
	for (i = 0; i < BAMVector_size(bamvector); i++)
	{
		print_vector_vector_int(BAMVector_data_vector_vector_int(bamvector)[i]);
		if (i < BAMVector_size(bamvector) - 1)
		{
			printf(", ");
		}
	}
	printf("]");
}

static void
print_map_int_int(struct BAMMap* bammap)
{
	int i;

	printf("{");
	for (i = 0; i < BAMVector_size(BAMMap_get_values(bammap)); i++)
	{
		printf("%ld: %ld", BAMVector_data_int(BAMMap_get_keys(bammap))[i], BAMVector_data_int(BAMMap_get_values(bammap))[i]);
		if (i < BAMVector_size(BAMMap_get_values(bammap)) - 1)
		{
			printf(", ");
		}

	}
	printf("}");
}

static void
print_map_int_map_int_vector_int(struct BAMMap* bammap)
{
	int i;

	printf("{");
	for (i = 0; i < BAMVector_size(BAMMap_get_values(bammap)); i++)
	{
		printf("%ld: ", BAMVector_data_int(BAMMap_get_keys(bammap))[i]);
		print_map_int_int(bammap);
		if (i < BAMVector_size(BAMMap_get_keys(bammap)) - 1)
		{
			printf(", ");
		}
	}
	printf("}");
}

static void
do_indent(struct TextSink* self)
{
	struct TextSink* textsink = (struct TextSink*) self;
	unsigned i;

	for (i = 0; i < textsink->indent; i++)
	{
		printf("    ");
	}
}

static bam_error
TextSink_before_frame(struct BAMSink* self)
{
	return BAM_OK;
}

static bam_error
TextSink_after_frame(struct BAMSink* self)
{
	return BAM_OK;
}

static bam_error
TextSink_before_call(struct BAMSink* self
	, const char* fname
	, const struct BAMVector* args
	, const char* return_var_name)
{

	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);

	if (return_var_name != NULL)
	{
		printf("%s = ", return_var_name);
	}

	printf("%s(", fname);


	unsigned i;
	for (i = 0; i < BAMVector_size(args); i++)
	{
		if (BAMParameter_return_parameter_type(BAMVector_data_parameter(args)[i]) == INT) 
		{
			printf("%ld", BAMParameter_as_int(BAMVector_data_parameter(args)[i]));
		}

		else if (BAMParameter_return_parameter_type(BAMVector_data_parameter(args)[i]) == STRING) 
		{
			printf("%s", BAMParameter_as_string(BAMVector_data_parameter(args)[i]));
		}

		else if (BAMParameter_return_parameter_type(BAMVector_data_parameter(args)[i]) == VECTOR_INT) 
		{
			print_vector_int(BAMParameter_as_vector_int(BAMVector_data_parameter(args)[i]));
		}

		else if (BAMParameter_return_parameter_type(BAMVector_data_parameter(args)[i]) == FLOAT) 
		{
			printf("%f", BAMParameter_as_float(BAMVector_data_parameter(args)[i]));
		}

		else if (BAMParameter_return_parameter_type(BAMVector_data_parameter(args)[i]) == VECTOR_FLOAT) 
		{
			print_vector_float(BAMParameter_as_vector_int(BAMVector_data_parameter(args)[i]));
		}
		else
		{
			printf("):\n");

			textsink->indent += 1;

			return BAM_OK;
		}

		if (i < BAMVector_size(args) - 1)
		{
			printf(", ");
		}
	}

	printf("):\n");
	textsink->indent += 1;

	return BAM_OK;
}

static bam_error
TextSink_write_uint(struct BAMSink* self,
	const char* varname,
	unsigned n,
	BAMint val)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s: %ld (0x%lx)\n", varname, val, val);

	return BAM_OK;
}

static bam_error
TextSink_write_align(struct BAMSink* self
	, unsigned num
	, unsigned value)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("byte_align(%d): %d\n", num, value);

	return BAM_OK;
}


static bam_error
TextSink_after_call_int(struct BAMSink* self,
	const char* fname,
	const char* return_var_name,
	BAMint return_value)
{
	struct TextSink* textsink = (struct TextSink*) self;
	textsink->indent = textsink->indent - 1;

	do_indent(textsink);
	printf("%s = %ld\n", return_var_name, return_value);

	return BAM_OK;
}

static bam_error
TextSink_after_call_float(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, BAMfloat return_value)
{
	struct TextSink* textsink = (struct TextSink*) self;
	textsink->indent = textsink->indent - 1;

	do_indent(textsink);
	printf("%s = %f\n", return_var_name, return_value);

	return BAM_OK;
}

static bam_error
TextSink_after_call_string(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, const char* string_name)
{
	struct TextSink* textsink = (struct TextSink*) self;
	textsink->indent = textsink->indent - 1;

	do_indent(textsink);
	printf("%s = %s\n", return_var_name, string_name);

	return BAM_OK;
}


static bam_error
TextSink_after_call_void(struct BAMSink* self,
	const char* fname)
{
	struct TextSink* textsink = (struct TextSink*) self;
	textsink->indent = textsink->indent - 1;

	return BAM_OK;
}

static bam_error
TextSink_after_call_vector_vector_vector_int(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, struct BAMVector* bamvector)
{
	struct TextSink* textsink = (struct TextSink*) self;
	textsink->indent = textsink->indent - 1;
	do_indent(textsink);

	printf("%s = ", return_var_name);
	print_vector_vector_vector_int(bamvector);
	printf("\n");

	return BAM_OK;
}

static bam_error
TextSink_after_call_vector_vector_int(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, struct BAMVector* bamvector)
{
	struct TextSink* textsink = (struct TextSink*) self;
	textsink->indent -= 1;

	do_indent(textsink);
	printf("%s = ", return_var_name);
	print_vector_vector_int(bamvector);
	printf("\n");

	return BAM_OK;
}

static bam_error
TextSink_after_call_vector_int(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, struct BAMVector* bamvector)
{
	struct TextSink* textsink = (struct TextSink*) self;
	textsink->indent = textsink->indent - 1;

	do_indent(textsink);
	printf("%s = ", return_var_name);
	print_vector_int(bamvector);
	printf("\n");

	return BAM_OK;
}

static bam_error
TextSink_after_call_map_int_int(struct BAMSink* self
	, const char* fname
	, const char* return_var_name
	, struct BAMMap* bammap)
{
	struct TextSink* textsink = (struct TextSink*) self;

	textsink->indent = textsink->indent - 1;

	do_indent(textsink);
	printf("%s = ", return_var_name);
	print_map_int_int(bammap);
	printf("\n");

	return BAM_OK;
}

static bam_error
TextSink_after_assign_int(struct BAMSink* self,
	const char* varname,
	BAMint val)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = %ld\n", varname, val);

	return BAM_OK;
}

static bam_error
TextSink_after_assign_string(struct BAMSink* self
	, const char* vname
	, const char* value)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = %s\n", vname, value);

	return BAM_OK;
}

static bam_error
TextSink_after_assign_vector_float(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	struct TextSink* textsink = (struct TextSink*) self;
	do_indent(textsink);

	printf("%s = ", varname);
	print_vector_float(bamvector);
	printf("\n");

	return BAM_OK;
}


static bam_error
TextSink_after_assign_vector_int(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	struct TextSink* textsink = (struct TextSink*) self;
	do_indent(textsink);

	printf("%s = ", varname);
	print_vector_int(bamvector);
	printf("\n");

	return BAM_OK;
}

static bam_error
TextSink_after_assign_vector_string(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = ..vs.\n", varname);
	return BAM_OK;
}

static bam_error
TextSink_after_assign_vector_vector_vector_int(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = ", varname);
	print_vector_vector_vector_int(bamvector);
	printf("\n");

	return BAM_OK;
}


static bam_error
TextSink_after_assign_vector_vector_int(struct BAMSink* self
	, const char* varname
	, struct BAMVector* bamvector)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = ", varname);
	print_vector_vector_int(bamvector);
	printf("\n");

	return BAM_OK;
}


static bam_error
TextSink_after_assign_map_int_int(struct BAMSink* self
	, const char* varname
	, struct BAMMap* bammap)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = ", varname);
	print_map_int_int(bammap);
	printf("\n");

	return BAM_OK;
}

static bam_error
TextSink_after_assign_map_int_vector_int(struct BAMSink* self
	, const char* name
	, struct BAMMap* bammap)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = ..mivi.\n", name);

	return BAM_OK;
}

static bam_error
TextSink_after_assign_map_vector_int_int(struct BAMSink* self
	, const char* name
	, struct BAMMap* bammap)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = ..mivi.\n", name);

	return BAM_OK;
}

static bam_error
TextSink_after_assign_map_int_map_int_vector_int(struct BAMSink* self
	, const char* varname
	, struct BAMMap* bammap)
{
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s = ", varname);
	print_map_int_map_int_vector_int(bammap);
	printf("\n");

	return BAM_OK;
}

static bam_error
TextSink_position(struct BAMSink* self
	, const char* varname
	, BAMint value)
{
	TextSink_after_assign_int(self, varname, value);

	return BAM_OK;
}

static bam_error
TextSink_after_push(struct BAMSink* self
	, unsigned num
	, BAMint value)
{
	return BAM_OK;
}

static void
print_as_binary(BAMint code)
{
	BAMint binaryNumber = 0;
	int remainder;
	int i = 1;

	while (code != 0)
	{
		remainder = code % 2;
		code /= 2;
		binaryNumber += remainder * i;
		i *= 10;
	}
	printf("%ld", binaryNumber);
}

static bam_error
TextSink_write_code(struct BAMSink* self,
	const char* code_name,
	const char* value_name,
	BAMint code,
	unsigned length,
	BAMint value)
{
	bam_error err;
	struct TextSink* textsink = (struct TextSink*) self;

	do_indent(textsink);
	printf("%s: 0b", code_name);
	print_as_binary(code);
	printf(" (length: %d, value: %ld)\n", length, value);
	
	err = TextSink_after_assign_int(self, value_name, value);
	if (err != BAM_OK)
	{
		return err;
	}

	return BAM_OK;
}

static void
TextSink_destroy(struct BAMSink* self)
{
	free(self);
}

struct BAMSink*
	TextSink_create()
{
	struct TextSink* textsink = malloc(sizeof * textsink);
	textsink->indent = 0;

	if (textsink == NULL)
	{
		free(textsink);
		return NULL;
	}

	BAMSink_init(&textsink->super);
	textsink->super.before_frame = TextSink_before_frame,
	textsink->super.after_frame = TextSink_after_frame,
	textsink->super.before_call = TextSink_before_call;
	textsink->super.write_uint = TextSink_write_uint;
	textsink->super.write_code = TextSink_write_code;
	textsink->super.write_align = TextSink_write_align;
	textsink->super.after_call_int = TextSink_after_call_int;
	textsink->super.after_call_float = TextSink_after_call_float;
	textsink->super.after_call_vector_int = TextSink_after_call_vector_int;
	textsink->super.after_call_vector_vector_int = TextSink_after_call_vector_vector_int;
	textsink->super.after_call_vector_vector_vector_int = TextSink_after_call_vector_vector_vector_int;
	textsink->super.after_call_void = TextSink_after_call_void;
	textsink->super.after_call_string = TextSink_after_call_string;
	textsink->super.after_call_map_int_int = TextSink_after_call_map_int_int;
	textsink->super.after_assign_int = TextSink_after_assign_int;
	textsink->super.after_assign_string = TextSink_after_assign_string;
	textsink->super.after_assign_vector_int = TextSink_after_assign_vector_int;
	textsink->super.after_assign_vector_float = TextSink_after_assign_vector_float;
	textsink->super.after_assign_vector_string = TextSink_after_assign_vector_string;
	textsink->super.after_assign_vector_vector_int = TextSink_after_assign_vector_vector_int;
	textsink->super.after_assign_vector_vector_vector_int = TextSink_after_assign_vector_vector_vector_int;
	textsink->super.after_assign_map_int_int = TextSink_after_assign_map_int_int;
	textsink->super.after_assign_map_vector_int_int = TextSink_after_assign_map_vector_int_int;
	textsink->super.after_assign_map_int_vector_int = TextSink_after_assign_map_int_vector_int;
	textsink->super.after_assign_map_int_map_int_vector_int = TextSink_after_assign_map_int_map_int_vector_int;
	textsink->super.after_push = TextSink_after_push;
	textsink->super.position = TextSink_position;
	textsink->super.destroy = TextSink_destroy;

	return &textsink->super;
};
