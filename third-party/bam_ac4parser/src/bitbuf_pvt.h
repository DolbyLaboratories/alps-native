/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2007-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @defgroup DLB_BITBUF_SRC Bit Buffer Management Source Code
 *
 * This is a detailed description of the components source code,
 * targeted to facilitate porting efforts of the module. 
 * 
 * If you are solely using the component in your code, please have a
 * look ath the API Reference.
 */
/*@{*/
/**
 * @file
 * @brief  Bit Buffer Management - Private Defines and Declarations
 */

#ifndef DLB_BITBUF_PVT_H
#define DLB_BITBUF_PVT_H

#include "dlb_bitbuf.h"

/* Retrieve log2(DLB_BITBUF_WIDTH) - does anyone know a smarter way? */
#if (DLB_BITBUF_WIDTH == 8)
#define DLB_BITBUF_WIDTH_SHIFT 3 
#else
#if (DLB_BITBUF_WIDTH == 16)
#define DLB_BITBUF_WIDTH_SHIFT 4
#else
#if (DLB_BITBUF_WIDTH == 32)
#define DLB_BITBUF_WIDTH_SHIFT 5
#else /* no match */
#error "Machine type not supported, CHAR_BIT none of 8, 16 or 32!"
#endif /* 32 */
#endif /* 16 */
#endif /* 8 */

#define BIT_MASK(N)        ((1 << (N)) - 1)   /**< 0b1...1 (N 1s) */

#define ASSERT_BITBUF_SANITY(p)                                                     \
do                                                                                  \
{                                                                                   \
    assert((p));                             /* Null pointer check */               \
    assert((p)->p_cur >= (p)->p_base);       /* Current position > buffer begin */  \
    assert((p)->bit_pos < DLB_BITBUF_WIDTH); /* Bit position within current word */ \
} while((void)0, 0)

#ifdef _MSC_VER
#define inline __forceinline
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Blindly skip 'num_bits' forward in p_bitbuf 
 */
void 
skip_bits_forward
    (dlb_bitbuf_handle p_bitbuf
    ,long              num_bits
    );


#ifdef __cplusplus
}
#endif

#endif /* DLB_BITBUF_H */
/*@}*/
