#include "day_07.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

static void parse_input(const char* const input, size_t size, uint64_t** output, size_t* output_len)
{
    memory_reference_t* line_mem_ref = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_ref, &n_lines);

    uint64_t* nums = (uint64_t*)malloc(n_lines * sizeof(uint64_t));

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        char* num_str = malloc(line_mem_ref[line_idx].size + 1);
        num_str[line_mem_ref[line_idx].size] = 0;
        memcpy(num_str, &input[line_mem_ref[line_idx].offset], line_mem_ref[line_idx].size);
        nums[line_idx] = atoi(num_str);
        free(num_str);
    }

    free(line_mem_ref);

    *output = nums;
    *output_len = n_lines;
}

static bool sum_of_two(uint64_t sum, const uint64_t* const nums, size_t len)
{
    for(size_t i = 0; i < len - 1; i++) {
        const uint64_t n = nums[i];
        const uint64_t m = sum - n;
        for(size_t j = i + 1; j < len; j++) {
            if(nums[j] == m) {
                return true;
            }
        }
    }
    return false;
}

static uint64_t find_invalid_num(const uint64_t* const nums, size_t len, size_t preamble_len)
{
    for(size_t i = preamble_len; i < len; i++) {
        if(!sum_of_two(nums[i], &nums[i - preamble_len], preamble_len)) {
            return nums[i];
        }
    }
    return 0;
}

static uint64_t solve_part_1(const char* const input, size_t size, size_t preamble_len)
{
    uint64_t* nums = NULL;
    size_t n_nums = 0;
    parse_input(input, size, &nums, &n_nums);

    const uint64_t invalid_num = find_invalid_num(nums, n_nums, preamble_len);

    free(nums);

    return invalid_num;
}

static uint64_t solve_part_2(const char* const input, size_t size, size_t preamble_len)
{
    uint64_t* nums = NULL;
    size_t n_nums = 0;
    parse_input(input, size, &nums, &n_nums);

    const uint64_t invalid_num = find_invalid_num(nums, n_nums, preamble_len);

    size_t start_idx = 0;
    size_t end_idx = 0;
    uint64_t sum = 0;

    for(size_t i = 0; i < n_nums; i++) {
        const uint64_t n = nums[i];

        if(sum < invalid_num) {
            end_idx = i;
            sum += n;
        }

        while(sum > invalid_num) {
            sum -= nums[start_idx++];
        }

        if(sum == invalid_num && (end_idx - start_idx + 1) > 1) {
            break;
        }
    }

    uint64_t min = UINT64_MAX;
    uint64_t max = 0;
    for(size_t i = start_idx; i <= end_idx; i++) {
        const uint64_t n = nums[i];

        min = n < min ? n : min;
        max = n > max ? n : max;
    }

    free(nums);

    return min + max;
}

void day_09_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_09_part_1_example.txt", &input, &size);
    assert(success);

    const int64_t ans = solve_part_1(input, size, 5);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(127, ans);
}

void day_09_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_09_part_1_input.txt", &input, &size);
    assert(success);

    const int64_t ans = solve_part_1(input, size, 25);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(177777905, ans);
}

void day_09_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_09_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_2(input, size, 5);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(62, ans);
}

void day_09_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_09_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_2(input, size, 25);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(23463012, ans);
}
