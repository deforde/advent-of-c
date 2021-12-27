#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unity.h>

#include "../utility.h"

uint64_t solve(const char* const input, size_t size)
{
    uint64_t ans = 0;
    const uint64_t target_sum = 2020;

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

    for(size_t i = 0; i < num_values - 1; ++i) {
        const uint64_t first_val = nums[i];
        const uint64_t required_val = target_sum - first_val;

        bool match_found = false;
        for(size_t j = i + 1; j < num_values; ++j) {
            const uint64_t second_val = nums[j];
            if(second_val == required_val) {
                ans = first_val * second_val;
                match_found = true;
                break;
            }
        }
        if(match_found) {
            break;
        }
    }

    free(nums);

    return ans;
}

void day_01_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_01_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve(input, size);

    TEST_ASSERT_EQUAL_UINT64(514579, ans);

    free(input);
}

void day_01_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_01_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve(input, size);

    TEST_ASSERT_EQUAL_UINT64(744475, ans);

    free(input);
}
