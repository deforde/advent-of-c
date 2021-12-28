#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unity.h>

#include "../utility.h"

bool product_of_sum(uint64_t sum, const uint64_t* const nums, size_t len, size_t n, uint64_t* product)
{
    *product = 1;
    for(size_t i = 0; i <= (len - n); ++i) {
        if(n > 1) {
            if(product_of_sum(sum - nums[i], &nums[i + 1], len - (i + 1), n - 1, product)) {
                *product *= nums[i];
                return true;
            }
        }
        else if(nums[i] == sum) {
            *product *= nums[i];
            return true;
        }
    }
    return false;
}

uint64_t solve(const char* const input, size_t size, uint64_t target_sum, size_t n)
{
    uint64_t ans = 0;

    size_t num_values = 0;
    for(size_t i = 0; i < size; ++i) {
        if(input[i] == '\n') {
            num_values++;
        }
    }

    uint64_t* nums = (uint64_t*)malloc(num_values * sizeof(uint64_t));

    const char* current = input;
    char* end = NULL;
    const int base = 10;

    size_t nums_idx = 0;
    for(unsigned long i = strtoul(current, &end, base); current != end; i = strtoul(current, &end, base)) {
        assert(nums_idx < num_values);
        nums[nums_idx++] = i;
        current = end;
    }
    assert(nums_idx == num_values);

    product_of_sum(target_sum, nums, num_values, n, &ans);

    free(nums);

    return ans;
}

void day_01_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_01_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve(input, size, 2020, 2);

    TEST_ASSERT_EQUAL_UINT64(514579, ans);

    free(input);
}

void day_01_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_01_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve(input, size, 2020, 2);

    TEST_ASSERT_EQUAL_UINT64(744475, ans);

    free(input);
}

void day_01_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_01_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve(input, size, 2020, 3);

    TEST_ASSERT_EQUAL_UINT64(241861950, ans);

    free(input);
}

void day_01_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_01_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve(input, size, 2020, 3);

    TEST_ASSERT_EQUAL_UINT64(70276940, ans);

    free(input);
}
