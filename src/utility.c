#include "utility.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void split_buf_by_sequence(const char* const buf, size_t size, const char* const sequence, memory_reference_t** mem_refs, size_t* n_mem_refs)
{
    const size_t seq_len = strlen(sequence);

    bool data_portion_encountered = false;
    size_t num_segments = 0;
    for(size_t i = 0; i < size; i++) {
        if(i <= (size - seq_len) && memcmp(&buf[i], sequence, seq_len) == 0) {
            num_segments++;
            data_portion_encountered = false;
            i += seq_len - 1;
        }
        else {
            data_portion_encountered = true;
        }

        if(i == size - 1 && data_portion_encountered) {
            num_segments++;
        }
    }

    memory_reference_t* temp = (memory_reference_t*)malloc(num_segments * sizeof(memory_reference_t));
    memset(temp, 0, num_segments * sizeof(memory_reference_t));
    size_t temp_idx = 0;

    size_t ref_offset = 0;
    size_t ref_size = 0;
    for(size_t i = 0; i < size; i++) {
        if(i <= (size - seq_len) && memcmp(&buf[i], sequence, seq_len) == 0) {
            temp[temp_idx].offset = ref_offset;
            temp[temp_idx].size = ref_size;
            temp_idx++;

            i += seq_len - 1;
            ref_offset = i + 1;
            ref_size = 0;
        }
        else {
            ref_size += 1;
        }

        if(i == size - 1 && ref_size > 0) {
            temp[temp_idx].offset = ref_offset;
            temp[temp_idx].size = ref_size;
            temp_idx++;
        }
    }
    assert(temp_idx == num_segments);

    *mem_refs = temp;
    *n_mem_refs = num_segments;
}

void split_buf_by_whitespace(const char* buf, size_t size, memory_reference_t** mem_refs, size_t* n_mem_refs)
{
    size_t num_segments = 0;
    bool accumulating_data = false;
    for(size_t i = 0; i < size; i++) {
        if(isspace(buf[i]) != 0 && accumulating_data) {
            num_segments++;
            accumulating_data = false;
        }
        else {
            accumulating_data = true;
        }

        if(i == size - 1 && accumulating_data) {
            num_segments++;
        }
    }

    memory_reference_t* temp = (memory_reference_t*)malloc(num_segments * sizeof(memory_reference_t));
    memset(temp, 0, num_segments * sizeof(memory_reference_t));
    size_t temp_idx = 0;

    size_t ref_offset = 0;
    size_t ref_size = 0;
    for(size_t i = 0; i < size; i++) {
        if(isspace(buf[i])) {
            if(ref_size > 0) {
                temp[temp_idx].offset = ref_offset;
                temp[temp_idx].size = ref_size;
                temp_idx++;
            }

            ref_offset = i + 1;
            ref_size = 0;
        }
        else {
            ref_size += 1;
        }

        if(i == size - 1 && ref_size > 0) {
            temp[temp_idx].offset = ref_offset;
            temp[temp_idx].size = ref_size;
            temp_idx++;
        }
    }
    assert(temp_idx == num_segments);

    *mem_refs = temp;
    *n_mem_refs = num_segments;
}
