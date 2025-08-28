#ifndef DLB_ALPS_NATIVE_UTILS_TYPES_H
#define DLB_ALPS_NATIVE_UTILS_TYPES_H

#define ALPS_INVALID_PRES_ID ((int)(-1))

typedef enum
{
    ALPS_RET_OK = 0,
    ALPS_RET_E_UNDEFINED,
    ALPS_RET_E_INVALID_ARG,
    ALPS_RET_E_BUFF_TOO_SMALL,
    ALPS_RET_E_PARSE,
    ALPS_RET_E_NEXT_SEGMENT,
    ALPS_RET_E_NO_MOVIE_INFO,
    ALPS_RET_E_PRES_ID_NOT_FOUND
} alps_ret;

typedef struct alps_presentation_t
{
    int presentation_id;
    char *label;
    char *language;
} alps_presentation;

#endif /* DLB_ALPS_NATIVE_UTILS_TYPES_H */
