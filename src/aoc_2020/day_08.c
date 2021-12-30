#include "day_08.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

static void execute_instruction(const char* const buf, size_t size, int64_t* acc, size_t* idx)
{
    if(memcmp(buf, "acc", 3) == 0) {
        char* val_str = malloc(size - 3);
        val_str[size - 3 - 1] = 0;
        memcpy(val_str, &buf[4], size - 3 - 1);

        *acc += atoi(val_str);
        *idx += 1;

        free(val_str);
    }
    else if(memcmp(buf, "jmp", 3) == 0) {
        char* val_str = malloc(size - 3);
        val_str[size - 3 - 1] = 0;
        memcpy(val_str, &buf[4], size - 3 - 1);

        *idx += atoi(val_str);

        free(val_str);
    }
    else if(memcmp(buf, "nop", 3) == 0) {
        *idx += 1;
    }
    else {
        assert(false);
    }
}

static bool execute_program(const char* const input, size_t size, int64_t* ans)
{
    *ans = 0;

    memory_reference_t* line_mem_ref = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_ref, &n_lines);

    size_t* executed_indices = (size_t*)malloc(n_lines * sizeof(size_t));
    size_t n_executed_indices = 0;

    bool early_termination = false;
    int64_t acc = 0;
    for(size_t line_idx = 0; line_idx < n_lines;) {
        bool new_index = true;
        for(size_t i = 0; i < n_executed_indices; i++) {
            if(executed_indices[i] == line_idx) {
                new_index = false;
                break;
            }
        }
        if(!new_index) {
            early_termination = true;
            break;
        }
        executed_indices[n_executed_indices++] = line_idx;

        const char* const line = &input[line_mem_ref[line_idx].offset];
        const size_t line_len = line_mem_ref[line_idx].size;

        execute_instruction(line, line_len, &acc, &line_idx);
    }

    free(executed_indices);
    free(line_mem_ref);

    *ans = acc;

    return early_termination;
}

static bool solve_part_1(const char* const input, size_t size, int64_t* ans)
{
    return execute_program(input, size, ans);
}

static int64_t solve_part_2(const char* const input, size_t size)
{
    memory_reference_t* line_mem_ref = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_ref, &n_lines);

    int64_t acc = 0;

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const line = &input[line_mem_ref[line_idx].offset];

        if(memcmp(line, "jmp", 3) == 0) {
            char* modified_prog = malloc(size);
            memcpy(modified_prog, input, size);
            memcpy(&modified_prog[line_mem_ref[line_idx].offset], "nop", 3);

            acc = 0;
            const bool solution_found = !execute_program(modified_prog, size, &acc);
            free(modified_prog);
            if(solution_found)
                break;
        }
        else if(memcmp(line, "nop", 3) == 0) {
            char* modified_prog = malloc(size);
            memcpy(modified_prog, input, size);
            memcpy(&modified_prog[line_mem_ref[line_idx].offset], "jmp", 3);

            acc = 0;
            const bool solution_found = !execute_program(modified_prog, size, &acc);
            free(modified_prog);
            if(solution_found)
                break;
        }
    }

    free(line_mem_ref);

    return acc;
}

void day_08_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_08_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    int64_t ans = 0;
    solve_part_1(input, size, &ans);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(5, ans);
}

void day_08_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_08_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    int64_t ans = 0;
    solve_part_1(input, size, &ans);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(1753, ans);
}

void day_08_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_08_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(8, ans);
}

void day_08_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_08_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(733, ans);
}
