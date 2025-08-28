#ifndef DLB_ALPS_NATIVE_UTILS_UTILS_H
#define DLB_ALPS_NATIVE_UTILS_UTILS_H

#include "types.h"

#define BAIL(ret_val)  \
    {                  \
        ret = (ret_val); \
        goto bail;     \
    }

#define CHECK_RET_AND_BAIL(expr) \
    {                            \
        ret = (expr);            \
        if (ret != ALPS_RET_OK)  \
            goto bail;           \
    }

#define CHECK_EXPR_AND_BAIL(expr, ret_val) \
    {                                      \
        if (expr)                          \
        {                                  \
            ret = ret_val;                 \
            goto bail;                     \
        }                                  \
    }

#endif /* DLB_ALPS_NATIVE_UTILS_UTILS_H */
