#ifndef UTILITY_H
#define UTILITY_H

#include <stdbool.h>
#include <stddef.h>

bool read_file_into_buf(const char* filename, char** buf, size_t* size);

#endif //UTILITY_H
