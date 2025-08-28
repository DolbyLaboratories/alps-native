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

/** @addtogroup DLB_BITBUF_SRC */
/*@{*/
/**
  @file
  @brief  Bit Buffer Management - Write Functions
*/

#include "stdio.h"
#include "assert.h"
#include "dlb_bitbuf_write.h"
#include "bitbuf_pvt.h"

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_write( dlb_bitbuf_handle p_bitbuf      
                 ,unsigned int      data         
                 ,unsigned int      n           
                 )
{
    DLB_BITBUF_DATATYPE *p_data;
    unsigned int         bit;

    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 16);            /* Check max pattern size */
    assert(data >> (n-1) <= 1);     /* Check for out-of-pattern bits */

    p_data  = p_bitbuf->p_cur;
    bit     = DLB_BITBUF_WIDTH - p_bitbuf->bit_pos;

    /* Enough bits left in buffer? */
    if ((long) n > p_bitbuf->bits_left)
    {
        return 1;
    }
    /* Exit early for trivial case */
    if (n == 0)         return 0;
    
    /* Update bits_left */
    p_bitbuf->bits_left -= n;

    /* Write to the end of the current word */
    while (n >= bit)
    {
        int bs_word; 

        bs_word  = *p_data;
        bs_word &= ~BIT_MASK(bit);
        bs_word |= (data >> (n - bit) & BIT_MASK(bit));

        *p_data++ = (DLB_BITBUF_DATATYPE) bs_word;
        n -= bit;
        bit = DLB_BITBUF_WIDTH;
    }

    /* Write remaing bits */
    if (n > 0)
    {
        int mask = BIT_MASK(n) << (bit - n);

        *p_data  = (DLB_BITBUF_DATATYPE) (*p_data & ~mask);
        *p_data |= (DLB_BITBUF_DATATYPE) (data << (bit - n)); 
        bit -= n;
    }

    /* Update remaining states */
    p_bitbuf->p_cur   = p_data;
    p_bitbuf->bit_pos = DLB_BITBUF_WIDTH - bit;

    return 0;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_write_long( dlb_bitbuf_handle p_bitbuf      
                      ,unsigned long     data         
                      ,unsigned int      n           
                      )
{ 
    DLB_BITBUF_DATATYPE *p_data;
    unsigned int         bit;

    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32);            /* Check max pattern size */ 
    assert(data >> (n-1) <= 1);     /* Check for out-of-pattern bits */
    
    p_data  = p_bitbuf->p_cur;
    bit     = DLB_BITBUF_WIDTH - p_bitbuf->bit_pos;

    /* Enough bits left in buffer? */
    if ((long) n > p_bitbuf->bits_left)
    {
        return 1;
    }
    /* Exit early for trivial case */
    if (n == 0)         return 0;
    
    /* Update bits_left */
    p_bitbuf->bits_left -= n;

    /* Write to the end of the current word */
    while (n >= bit)
    {
        long bs_word; 

        bs_word  = *p_data;
        bs_word &= ~BIT_MASK(bit);
        bs_word |= (data >> (n - bit) & BIT_MASK(bit));

        *p_data++  = (unsigned char) bs_word;

        n  -= bit;
        bit = DLB_BITBUF_WIDTH;
    }

    /* Write remaing bits */
    if (n > 0)
    {
        int mask = BIT_MASK(n) << (bit - n);

        *p_data  = (DLB_BITBUF_DATATYPE) (*p_data & ~mask);
        *p_data |= (DLB_BITBUF_DATATYPE) (data << (bit - n)); 

        bit -= n;
    }

    /* Update remaining states */
    p_bitbuf->p_cur   = p_data;
    p_bitbuf->bit_pos = DLB_BITBUF_WIDTH - bit;

    return 0;
}
/*@}*/
