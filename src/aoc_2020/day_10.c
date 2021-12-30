#include "day_10.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

typedef struct {
    uint64_t diff;
    uint64_t count;
} diff_count_t;

static void parse_input(const char* const input, size_t size, uint64_t** output, size_t* output_len)
{
    memory_reference_t* line_mem_ref = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_ref, &n_lines);

    uint64_t* nums = (uint64_t*)malloc((n_lines + 2) * sizeof(uint64_t));
    uint64_t max = 0;

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        char* num_str = malloc(line_mem_ref[line_idx].size + 1);
        num_str[line_mem_ref[line_idx].size] = 0;
        memcpy(num_str, &input[line_mem_ref[line_idx].offset], line_mem_ref[line_idx].size);

        nums[line_idx + 1] = atoi(num_str);
        max = nums[line_idx + 1] > max ? nums[line_idx + 1] : max;

        free(num_str);
    }

    nums[0] = 0;
    nums[n_lines+ 1] = max + 3;

    free(line_mem_ref);

    *output = nums;
    *output_len = n_lines + 2;
}

static void sort(uint64_t* const nums, size_t len)
{
    bool sorted = false;
    while(!sorted) {
        sorted = true;
        for(size_t i = 0; i < len - 1; i++) {
            const uint64_t a = nums[i];
            const uint64_t b = nums[i + 1];
            if(a > b) {
                sorted = false;
                nums[i] = b;
                nums[i + 1] = a;
            }
        }
    }
}

static void get_diff_counts(const uint64_t* const nums, size_t len, diff_count_t** output, size_t* output_len)
{
    diff_count_t* diff_counts = (diff_count_t*)malloc(len * sizeof(diff_count_t));
    size_t n_diff_counts = 0;

    for(size_t i = 0; i < len - 1; i++) {
        const uint64_t m = nums[i];
        const uint64_t n = nums[i + 1];
        const uint64_t diff = n - m;

        bool new_diff = true;
        for(size_t j = 0; j < n_diff_counts; j++) {
            diff_count_t* diff_count = &diff_counts[j];
            if(diff_count->diff == diff) {
                diff_count->count++;
                new_diff = false;
                break;
            }
        }
        if(new_diff) {
            diff_count_t* diff_count = &diff_counts[n_diff_counts++];
            diff_count->diff = diff;
            diff_count->count = 1;
        }
    }

    *output = diff_counts;
    *output_len = n_diff_counts;
}

static uint64_t get_diff_count(uint64_t diff, diff_count_t* diff_counts, size_t len)
{
    for(size_t i = 0; i < len; i++) {
        diff_count_t* diff_count = &diff_counts[i];
        if(diff_count->diff == diff) {
            return diff_count->count;
        }
    }
    return 0;
}

static void count_all_paths(const uint64_t* const nums, size_t len, size_t i, size_t* n_paths)
{
    if(i == (len - 1)) {
        *n_paths += 1;
        return;
    }

    const uint64_t n = nums[i];

    for(size_t j = i + 1; j < len; j++) {
        const uint32_t m = nums[j];
        if((m - n) < 4) {
            count_all_paths(nums, len, j, n_paths);
        }
        else {
            break;
        }
    }
}

static void get_conversion_points(const uint64_t* const nums, size_t len, size_t** output, size_t* output_len)
{
    size_t* conversion_point_indices = (size_t*)malloc(len * sizeof(size_t));
    size_t n_conversion_points = 0;

    for(size_t i = 0; i < len - 1; i ++) {
        const uint64_t diff = nums[i + 1] - nums[i];
        if(diff == 3) {
            conversion_point_indices[n_conversion_points++] = i;
        }
    }

    *output = conversion_point_indices;
    *output_len = n_conversion_points;
}

static uint64_t solve_part_1(const char* const input, size_t size)
{
    uint64_t* nums = NULL;
    size_t n_nums = 0;
    parse_input(input, size, &nums, &n_nums);

    sort(nums, n_nums);

    diff_count_t* diff_counts = NULL;
    size_t n_diff_counts = 0;
    get_diff_counts(nums, n_nums, &diff_counts, &n_diff_counts);

    const uint64_t one_diff_count = get_diff_count(1, diff_counts, n_diff_counts);
    const uint64_t three_diff_count = get_diff_count(3, diff_counts, n_diff_counts);

    free(diff_counts);
    free(nums);

    return one_diff_count * three_diff_count;
}

static uint64_t solve_part_2(const char* const input, size_t size)
{
    uint64_t* nums = NULL;
    size_t n_nums = 0;
    parse_input(input, size, &nums, &n_nums);

    sort(nums, n_nums);

    size_t* conversion_points = NULL;
    size_t n_conversion_points = 0;
    get_conversion_points(nums, n_nums, &conversion_points, &n_conversion_points);

    size_t total_num_paths = 1;
    size_t start_idx = 0;
    for(size_t i = 0; i < n_conversion_points; i++) {
        uint64_t* nums_sub_set = &nums[start_idx];
        size_t sub_set_len = conversion_points[i] - start_idx + 1;

        size_t n_paths = 0;
        count_all_paths(nums_sub_set, sub_set_len, 0, &n_paths);

        total_num_paths *= n_paths;
        start_idx = conversion_points[i];
    }

    free(conversion_points);
    free(nums);

    return total_num_paths;
}

void day_10_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_10_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(220, ans);
}

void day_10_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_10_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(2368, ans);
}

void day_10_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_10_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(19208, ans);
}

void day_10_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_10_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(1727094849536, ans);
}
