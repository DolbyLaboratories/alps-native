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

/**
 * \file
 * \brief Various macros used throughout the library.
 */

#ifndef DLB_ALPS_NATIVE_UTILS_UTILS_H
#define DLB_ALPS_NATIVE_UTILS_UTILS_H

#include "types.h"

/**
 * \brief Sets <c>ret</c> variable and jumps to the <c>bail</c> label.
 * \param[in] ret_val value to set <c>ret</c> variable to
 */
#define BAIL(ret_val)    \
    {                    \
        ret = (ret_val); \
        goto bail;       \
    }

/**
 * \brief Checks the value of the expression and if it's not equal to \ref ALPS_RET_OK jumps to the <c>bail</c> label.
 * \param[in] expr expression to check the value of
 */
#define CHECK_RET_AND_BAIL(expr) \
    {                            \
        ret = (expr);            \
        if (ret != ALPS_RET_OK)  \
            goto bail;           \
    }

/**
 * \brief Checks the value of the expression and if it's not <c>true</c> sets the <c>ret</c> variable and jumps to
 * the <c>bail</c> label.
 * \param[in] expr expression to check the value of
 * \param[in] ret_val value to set <c>ret</c> variable to
 */
#define CHECK_EXPR_AND_BAIL(expr, ret_val) \
    {                                      \
        if (expr)                          \
        {                                  \
            ret = ret_val;                 \
            goto bail;                     \
        }                                  \
    }

#endif /* DLB_ALPS_NATIVE_UTILS_UTILS_H */
