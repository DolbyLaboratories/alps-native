#ifndef BAM_TYPES
#define BAM_TYPES

typedef double BAMfloat;
typedef long BAMint;

typedef enum bam_error
{
    BAM_OK,                     /* no error */
    BAM_ERROR_ILLEGAL_INPUT,    /* invalid input bitstream */
    BAM_ERROR_UNEXPECTED,		
    BAM_BITS_OUT_OF_RANGE,		/* bits out of range in next_frame (Binary/Int- source) */
    BAM_RESERVED_VALUE,			/* reserved value found in the generated code */
    BAM_ARRAY_FULL,				
    BAM_FUNCTION_ERROR,			
    BAM_WRITE_ERROR,
    BAM_HUFFMAN_INVALID_PREFIX,
    BAM_PUSH_ERROR,
    BAM_REALLOC_ERROR,
    BAMMAP_NO_KEY_VALUE,		/* no possible key-value pair found */
	BAM_PREMATURE_EOF_ERROR,	/* Premature EOF, x more bits needed in BinarySource */
    BAM_ASSERTION_ERROR			/* Expected output does not match the result */

} bam_error;

#endif
