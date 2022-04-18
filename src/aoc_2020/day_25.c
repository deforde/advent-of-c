#include "day_24.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

static size_t solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    size_t card_pub_key = 0;
    size_t door_pub_key = 0;
    {
        char temp[128] = {0};
        memcpy(temp, &input[line_mem_refs[0].offset], line_mem_refs[0].size);
        card_pub_key = atoi(temp);
    }
    {
        char temp[128] = {0};
        memcpy(temp, &input[line_mem_refs[1].offset], line_mem_refs[1].size);
        door_pub_key = atoi(temp);
    }

    size_t x = card_pub_key;
    size_t card_loop_count = 0;
    while(x != 1) {
        while(x % 7 != 0) {
            x += 20201227;
        }
        x /= 7;
        card_loop_count++;
    }

    size_t encryption_key = 1;
    for(size_t i = 0; i < card_loop_count; ++i) {
        encryption_key = (encryption_key * door_pub_key) % 20201227;
    }

    free(line_mem_refs);

    return encryption_key;
}

void day_25_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_25_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(14897079, ans);
}

void day_25_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_25_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(11288669, ans);
}

