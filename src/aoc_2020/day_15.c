#include "day_15.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unity.h>

#include "../utility.h"

typedef struct {
    int64_t value;
    size_t pos;
    size_t diff;
} turn_t;

static int64_t solve(const char* const input, size_t size, size_t total_n_turns)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    memory_reference_t* value_mem_refs = NULL;
    size_t n_values = 0;
    split_buf_by_sequence(input, size, ",", &value_mem_refs, &n_values);

    turn_t* turns = (turn_t*)malloc(total_n_turns * sizeof(turn_t));
    memset(turns, 0, total_n_turns * sizeof(turn_t));

    int64_t prev_value = 0;

    for(size_t i = 0; i < n_values; i++) {
        const char* const val_buf = &input[line_mem_refs[0].offset + value_mem_refs[i].offset];
        const size_t val_len = value_mem_refs[i].size;

        char* val_str = malloc(val_len + 1);
        val_str[val_len] = 0;
        memcpy(val_str, val_buf, val_len);

        const int64_t val = atoll(val_str);

        free(val_str);

        assert(val < (int64_t)total_n_turns);
        turns[val].value = val;
        turns[val].pos = i;
        turns[val].diff = 0;

        prev_value = val;
    }

    size_t prev_diff = 0;
    for(size_t i = n_values; i < total_n_turns; i++) {
        int64_t new_value = prev_diff;

        if(turns[new_value].value == new_value) {
            turns[new_value].diff = i - turns[new_value].pos;
            turns[new_value].pos = i;
            prev_diff = turns[new_value].diff;
        }
        else {
            turns[new_value].value = new_value;
            turns[new_value].pos = i;
            turns[new_value].diff = 0;
            prev_diff = 0;
        }

        prev_value = new_value;
    }

    free(turns);
    free(value_mem_refs);
    free(line_mem_refs);

    return prev_value;
}

void day_15_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_15_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve(input, size, 2020);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(436, ans);
}

void day_15_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_15_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve(input, size, 2020);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(257, ans);
}

void day_15_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_15_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve(input, size, 30000000);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(175594, ans);
}

void day_15_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_15_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve(input, size, 30000000);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(8546398, ans);
}
