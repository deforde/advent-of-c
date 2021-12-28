#ifndef UTILITY_H
#define UTILITY_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    size_t offset;
    size_t size;
} memory_reference_t;

bool read_file_into_buf(const char* filename, char** buf, size_t* size);

void split_buf_by_sequence(const char* buf, size_t size, const char* sequence, memory_reference_t** mem_refs, size_t* n_mem_refs);

void split_buf_by_whitespace(const char* buf, size_t size, memory_reference_t** mem_refs, size_t* n_mem_refs);

#endif //UTILITY_H
