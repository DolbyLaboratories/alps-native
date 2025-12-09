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
