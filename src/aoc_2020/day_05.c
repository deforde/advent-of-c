#include "day_05.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

static void get_seat_coords(const char* const boarding_pass, size_t len, uint64_t* row, uint64_t* col)
{
    *row = 0;
    *col = 0;
    assert(len == 10);

    uint64_t min_row = 0;
    uint64_t max_row = 127;
    for(size_t i = 0; i < 7; i++) {
        if(boarding_pass[i] == 'F') {
            max_row = (max_row - min_row) / 2 + min_row;
        }
        else {
            min_row = (max_row - min_row) / 2 + min_row + 1;
        }
    }

    uint64_t min_col = 0;
    uint64_t max_col = 7;
    for(size_t i = 7; i < 10; i++) {
        if(boarding_pass[i] == 'L') {
            max_col = (max_col - min_col) / 2 + min_col;
        }
        else {
            min_col = (max_col - min_col) / 2 + min_col + 1;
        }
    }

    *row = min_row;
    *col = min_col;
}

static uint64_t solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* boarding_pass_mem_refs = NULL;
    size_t n_boarding_pass_mem_refs = 0;

    split_buf_by_whitespace(input, size, &boarding_pass_mem_refs, &n_boarding_pass_mem_refs);

    uint64_t max_seat_id = 0;
    for(size_t boarding_pass_idx = 0; boarding_pass_idx < n_boarding_pass_mem_refs; boarding_pass_idx++) {
        const char* const boarding_pass = &input[boarding_pass_mem_refs[boarding_pass_idx].offset];
        const size_t boarding_pass_len = boarding_pass_mem_refs[boarding_pass_idx].size;

        uint64_t row = 0;
        uint64_t col = 0;
        get_seat_coords(boarding_pass, boarding_pass_len, &row, &col);

        const uint64_t seat_id = 8 * row + col;
        max_seat_id = seat_id > max_seat_id ? seat_id : max_seat_id;
    }

    free(boarding_pass_mem_refs);

    return max_seat_id;
}

static uint64_t solve_part_2(const char* const input, size_t size)
{
    memory_reference_t* boarding_pass_mem_refs = NULL;
    size_t n_boarding_pass_mem_refs = 0;

    split_buf_by_whitespace(input, size, &boarding_pass_mem_refs, &n_boarding_pass_mem_refs);

    uint32_t* grid = (uint32_t*)malloc(128 * 8 * sizeof(uint32_t));
    memset(grid, 0, 128 * 8 * sizeof(uint32_t));

    for(size_t boarding_pass_idx = 0; boarding_pass_idx < n_boarding_pass_mem_refs; boarding_pass_idx++) {
        const char* const boarding_pass = &input[boarding_pass_mem_refs[boarding_pass_idx].offset];
        const size_t boarding_pass_len = boarding_pass_mem_refs[boarding_pass_idx].size;

        uint64_t row = 0;
        uint64_t col = 0;
        get_seat_coords(boarding_pass, boarding_pass_len, &row, &col);

        grid[row * 8 + col] = 1;
    }

    uint64_t seat_id = 0;
    for(size_t row = 0; row < 128; row++) {
        for(size_t col = 0; col < 8; col++) {
            if((row == 0 && col == 0) || (row == 127 && col == 7)) {
                continue;
            }

            const size_t current_index = row * 8 + col;
            if(grid[current_index] == 0 && grid[current_index - 1] == 1 && grid[current_index + 1] == 1) {
                seat_id = current_index;
                break;
            }
        }
        if(seat_id != 0) {
            break;
        }
    }

    free(grid);
    free(boarding_pass_mem_refs);

    return seat_id;
}

void day_05_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_05_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_1(input, size);
    TEST_ASSERT_EQUAL_UINT64(820, ans);

    free(input);
}

void day_05_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_05_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_1(input, size);
    TEST_ASSERT_EQUAL_UINT64(933, ans);

    free(input);
}

void day_05_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_05_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_2(input, size);
    TEST_ASSERT_EQUAL_UINT64(711, ans);

    free(input);
}
