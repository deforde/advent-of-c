#include "utility.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

bool read_file_into_buf(const char* const filename, char** buf, size_t* size)
{
    *buf = NULL;
    *size = 0;
    FILE* file = NULL;
    bool success = false;

    file = fopen(filename, "r");

    if(file == NULL) {
        return false;
    }

    fseek(file, 0, SEEK_END);
    const size_t temp_buf_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* const temp_buf = malloc(temp_buf_size);

    const size_t read_size = fread(temp_buf, 1, temp_buf_size, file);
    if(read_size == temp_buf_size) {
        success = true;
        *buf = temp_buf;
        *size = temp_buf_size;
    }
    else {
        free(temp_buf);
    }

    fclose(file);

    return success;
}
