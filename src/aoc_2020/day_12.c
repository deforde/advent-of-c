#include "day_12.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

static void process_rotation_part_1(char rot, int64_t deg, char* heading)
{
    switch(rot) {
        case 'R': {
            for(int64_t i = 0; i < deg; i += 90) {
                switch(*heading) {
                    case 'E': {
                        *heading = 'S';
                        break;
                    }
                    case 'S': {
                        *heading = 'W';
                        break;
                    }
                    case 'W': {
                        *heading = 'N';
                        break;
                    }
                    case 'N': {
                        *heading = 'E';
                        break;
                    }
                }
            }
            break;
        }
        case 'L': {
            for(int64_t i = 0; i < deg; i += 90) {
                switch(*heading) {
                    case 'E': {
                        *heading = 'N';
                        break;
                    }
                    case 'S': {
                        *heading = 'E';
                        break;
                    }
                    case 'W': {
                        *heading = 'S';
                        break;
                    }
                    case 'N': {
                        *heading = 'W';
                        break;
                    }
                }
            }
            break;
        }
    }
}

static void process_instruction_part_1(const char* const instr, size_t len, int64_t* x, int64_t* y, char* h)
{
    const char l = instr[0];

    char* v_str = malloc(len);
    v_str[len - 1] = 0;
    memcpy(v_str, &instr[1], len - 1);
    const int64_t v = atoi(v_str);
    free(v_str);

    switch(l) {
        case 'E': {
            *x += v;
            break;
        }
        case 'N': {
            *y += v;
            break;
        }
        case 'W': {
            *x -= v;
            break;
        }
        case 'S': {
            *y -= v;
            break;
        }
        case 'F': {
            switch(*h) {
                case 'E': {
                    *x += v;
                    break;
                }
                case 'S': {
                    *y -= v;
                    break;
                }
                case 'W': {
                    *x -= v;
                    break;
                }
                case 'N': {
                    *y += v;
                    break;
                }
            }
            break;
        }
        default: {
            process_rotation_part_1(l, v, h);
            break;
        }
    }
}

static void process_rotation_part_2(char rot, int64_t deg, int64_t* w_x, int64_t* w_y)
{
    switch(rot) {
        case 'R': {
            for(int64_t i = 0; i < deg; i += 90) {
                const int64_t d_x = *w_y;
                const int64_t d_y = -1 * *w_x;
                *w_x = d_x;
                *w_y = d_y;
            }
            break;
        }
        case 'L': {
            for(int64_t i = 0; i < deg; i += 90) {
                const int64_t d_x = -1 * *w_y;
                const int64_t d_y = *w_x;
                *w_x = d_x;
                *w_y = d_y;
            }
            break;
        }
    }
}

static void process_instruction_part_2(const char* const instr, size_t len, int64_t* x, int64_t* y, int64_t* w_x, int64_t* w_y)
{
    const char l = instr[0];

    char* v_str = malloc(len);
    v_str[len - 1] = 0;
    memcpy(v_str, &instr[1], len - 1);
    const int64_t v = atoi(v_str);
    free(v_str);

    switch(l) {
        case 'E': {
            *w_x += v;
            break;
        }
        case 'N': {
            *w_y += v;
            break;
        }
        case 'W': {
            *w_x -= v;
            break;
        }
        case 'S': {
            *w_y -= v;
            break;
        }
        case 'F': {
            for(int64_t i = 0; i < v; i++) {
                *x += *w_x;
                *y += *w_y;
            }
            break;
        }
        default: {
            process_rotation_part_2(l, v, w_x, w_y);
            break;
        }
    }
}

static uint64_t solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    int64_t x = 0;
    int64_t y = 0;
    char h = 'E';
    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const instr = &input[line_mem_refs[line_idx].offset];
        const size_t len = line_mem_refs[line_idx].size;

        process_instruction_part_1(instr, len, &x, &y, &h);
    }

    free(line_mem_refs);

    return labs(x) + labs(y);
}

static uint64_t solve_part_2(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    int64_t x = 0;
    int64_t y = 0;
    int64_t w_x = 10;
    int64_t w_y = 1;
    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const instr = &input[line_mem_refs[line_idx].offset];
        const size_t len = line_mem_refs[line_idx].size;

        process_instruction_part_2(instr, len, &x, &y, &w_x, &w_y);
    }

    free(line_mem_refs);

    return labs(x) + labs(y);
}

void day_12_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_12_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(25, ans);
}

void day_12_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_12_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(1177, ans);
}

void day_12_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_12_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(286, ans);
}

void day_12_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_12_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(46530, ans);
}
