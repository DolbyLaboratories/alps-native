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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dlb_alps_native/dlb_alps_native.h"

#define CHECK_RET_AND_EXIT(expr, ...)         \
    {                                     \
        ret = (expr);                     \
        if (ret != ALPS_RET_OK)           \
        {                                 \
            fprintf(stderr, __VA_ARGS__); \
            return ret;                   \
        }                                 \
    }

static alps_ret load_file(char *path, unsigned char **buf, size_t *size)
{
    FILE *file;
    long size_ret;
    size_t bytes_read;
    int ret;

    file = fopen(path, "r");
    if (!file)
    {
        return ALPS_RET_E_UNDEFINED;
    }

    fseek(file, 0L, SEEK_END);
    size_ret = ftell(file);
    if (size_ret == -1L)
    {
        fprintf(stderr, "Failed to read the size of file: %s\n", path);
        return ALPS_RET_E_UNDEFINED;
    }

    *size = (size_t)size_ret;
    rewind(file);

    *buf = malloc(*size);
    if (!(*buf))
    {
        return ALPS_RET_E_UNDEFINED;
    }

    bytes_read = fread(*buf, sizeof(unsigned char), *size, file);
    ret = fclose(file);

    (void)bytes_read;
    (void)ret;

    return ALPS_RET_OK;
}

int main (int argc, char *argv[])
{
    int active_presentation_id = ALPS_INVALID_PRES_ID;
    int ret;
    size_t mem_size = 0;
    void *mem;
    alps_ctx *alps_handle = 0;
    int i;

    printf("ALPS Native %s\n", alps_version());

    active_presentation_id = atoi(argv[argc - 1]);

    ret = alps_query_mem(&mem_size);
    mem = malloc(mem_size);
    CHECK_RET_AND_EXIT(alps_init(&alps_handle, mem), "Failed to initialize ALPS library\n");

    CHECK_RET_AND_EXIT(
        alps_set_active_presentation_id(alps_handle, active_presentation_id), "Failed to set active presentation ID");

    for (i = 1; i < argc - 1; i++)
    {
        char *file_path = argv[i];
        unsigned char *file_buf = NULL;
        size_t file_size = 0;

        printf("Processing file '%s'\n", file_path);
        load_file(file_path, &file_buf, &file_size);
        ret = alps_process_isobmff_segment(alps_handle, file_buf, file_size);
        free(file_buf);
        CHECK_RET_AND_EXIT(ret, "Failed to process file '%s'", file_path);
    }

    return ret;
}
