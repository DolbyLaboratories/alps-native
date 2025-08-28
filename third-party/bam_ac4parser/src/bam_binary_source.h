#ifndef BINARYSOURCE_H_INCLUDED
#define BINARYSOURCE_H_INCLUDED
#include <stddef.h>

/* expects a buffer from the binaryfile and the size in bytes */
struct BAMSource *
BinarySource_create(unsigned char *buffer
                  , size_t _size);

#endif
