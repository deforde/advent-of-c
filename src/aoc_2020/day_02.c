#include "day_02.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

static size_t solve_part_1(const char* const input, size_t size)
{
    size_t valid_pswd_count = 0;
    size_t line_start_idx = 0;

    for(size_t i = 0; i < size; i++) {
        if(input[i] == '\n') {
            size_t offset = line_start_idx;

            char min_count_str[4];
            memset(min_count_str, 0, sizeof(min_count_str));
            for(size_t j = offset; j < i; j++) {
                if(input[j] == '-') {
                    offset = j + 1;
                    break;
                }
                assert(j - offset < sizeof(min_count_str));
                min_count_str[j - offset] = input[j];
            }
            const size_t min_count = atoi(min_count_str);

            char max_count_str[4];
            memset(max_count_str, 0, sizeof(max_count_str));
            for(size_t j = offset; j < i; j++) {
                if(input[j] == ' ') {
                    offset = j + 1;
                    break;
                }
                assert(j - offset < sizeof(max_count_str));
                max_count_str[j - offset] = input[j];
            }
            const size_t max_count = atoi(max_count_str);

            const char letter_of_interest = input[offset];
            offset += 3;

            size_t count = 0;
            for(size_t j = offset; j < i; j++) {
                if(input[j] == letter_of_interest) {
                    count++;
                }
            }

            if(count >= min_count && count <= max_count) {
                valid_pswd_count++;
            }

            line_start_idx = i + 1;
        }
    }

    return valid_pswd_count;
}

static size_t solve_part_2(const char* const input, size_t size)
{
    size_t valid_pswd_count = 0;
    size_t line_start_idx = 0;

    for(size_t i = 0; i < size; i++) {
        if(input[i] == '\n') {
            size_t offset = line_start_idx;

            char pos_1_str[4];
            memset(pos_1_str, 0, sizeof(pos_1_str));
            for(size_t j = offset; j < i; j++) {
                if(input[j] == '-') {
                    offset = j + 1;
                    break;
                }
                assert(j - offset < sizeof(pos_1_str));
                pos_1_str[j - offset] = input[j];
            }
            const size_t pos_1 = atoi(pos_1_str) - 1;

            char pos_2_str[4];
            memset(pos_2_str, 0, sizeof(pos_2_str));
            for(size_t j = offset; j < i; j++) {
                if(input[j] == ' ') {
                    offset = j + 1;
                    break;
                }
                assert(j - offset < sizeof(pos_2_str));
                pos_2_str[j - offset] = input[j];
            }
            const size_t pos_2 = atoi(pos_2_str) - 1;

            const char letter_of_interest = input[offset];
            offset += 3;

            size_t count = 0;
            if(input[offset + pos_1] == letter_of_interest) {
                count++;
            }
            if(input[offset + pos_2] == letter_of_interest) {
                count++;
            }

            if(count == 1) {
                valid_pswd_count++;
            }

            line_start_idx = i + 1;
        }
    }

    return valid_pswd_count;
}

void day_02_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_02_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);
    TEST_ASSERT_EQUAL_UINT64(2, ans);

    free(input);
}

void day_02_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_02_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);
    TEST_ASSERT_EQUAL_UINT64(483, ans);

    free(input);
}

void day_02_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_02_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);
    TEST_ASSERT_EQUAL_UINT64(1, ans);

    free(input);
}

void day_02_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_02_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);
    TEST_ASSERT_EQUAL_UINT64(482, ans);

    free(input);
}
