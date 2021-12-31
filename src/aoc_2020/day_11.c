#include "day_11.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

static void parse_input(const char* const input, size_t size, char** output_grid, size_t* output_n_rows, size_t* output_n_cols)
{
    memory_reference_t* line_mem_ref = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_ref, &n_lines);

    const size_t n_rows = n_lines;
    const size_t n_cols = line_mem_ref[0].size;
    char* grid = malloc(n_rows * n_cols);

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        memcpy(&grid[line_idx * n_cols], &input[line_mem_ref[line_idx].offset], n_cols);
    }

    free(line_mem_ref);

    *output_grid = grid;
    *output_n_rows = n_rows;
    *output_n_cols = n_cols;
}

static uint64_t solve_part_1(const char* const input, size_t size)
{
    char* grid = NULL;
    size_t n_rows = 0;
    size_t n_cols = 0;
    parse_input(input, size, &grid, &n_rows, &n_cols);

    bool change_occurred = true;
    while(change_occurred) {
        change_occurred = false;
        char* new_grid = malloc(n_rows * n_cols);
        memcpy(new_grid, grid, n_rows * n_cols);

        for(size_t row_idx = 0; row_idx < n_rows; row_idx++) {
            for(size_t col_idx = 0; col_idx < n_cols; col_idx++) {
                switch(grid[row_idx * n_cols + col_idx]) {
                    case 'L': {
                        const size_t row_search_min = row_idx == 0 ? row_idx : row_idx - 1;
                        const size_t row_search_max = row_idx == (n_rows - 1) ? row_idx : row_idx + 1;

                        const size_t col_search_min = col_idx == 0 ? col_idx : col_idx - 1;
                        const size_t col_search_max = col_idx == (n_cols - 1) ? col_idx : col_idx + 1;

                        bool any_adjacent_occupied = false;
                        for(size_t other_row_idx = row_search_min; other_row_idx <= row_search_max; other_row_idx++) {
                            for(size_t other_col_idx = col_search_min; other_col_idx <= col_search_max; other_col_idx++) {
                                if(other_row_idx != row_idx || other_col_idx != col_idx) {
                                    if(grid[other_row_idx * n_cols + other_col_idx] == '#') {
                                        any_adjacent_occupied = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if(!any_adjacent_occupied) {
                            new_grid[row_idx * n_cols + col_idx] = '#';
                            change_occurred = true;
                        }

                        break;
                    }
                    case '#': {
                        const size_t row_search_min = row_idx == 0 ? row_idx : row_idx - 1;
                        const size_t row_search_max = row_idx == (n_rows - 1) ? row_idx : row_idx + 1;

                        const size_t col_search_min = col_idx == 0 ? col_idx : col_idx - 1;
                        const size_t col_search_max = col_idx == (n_cols - 1) ? col_idx : col_idx + 1;

                        size_t n_adjacent_occupied = 0;
                        for(size_t other_row_idx = row_search_min; other_row_idx <= row_search_max; other_row_idx++) {
                            for(size_t other_col_idx = col_search_min; other_col_idx <= col_search_max; other_col_idx++) {
                                if(other_row_idx != row_idx || other_col_idx != col_idx) {
                                    if(grid[other_row_idx * n_cols + other_col_idx] == '#') {
                                        n_adjacent_occupied++;
                                    }
                                }
                            }
                        }
                        if(n_adjacent_occupied >= 4) {
                            new_grid[row_idx * n_cols + col_idx] = 'L';
                            change_occurred = true;
                        }

                        break;
                    }
                }
            }
        }

        free(grid);
        grid = new_grid;
    }

    size_t occupied_count = 0;
    for(size_t i = 0; i < n_rows * n_cols; i++) {
        if(grid[i] == '#') {
            occupied_count++;
        }
    }

    free(grid);

    return occupied_count;
}

static bool search_for_occupied_seat(const char* const grid, int64_t n_rows, int64_t n_cols, int64_t row_idx, int64_t col_idx, int64_t d_row, int64_t d_col)
{
    int64_t other_row_idx = row_idx + d_row;
    int64_t other_col_idx = col_idx + d_col;
    while(other_row_idx >= 0 && other_row_idx < n_rows && other_col_idx >= 0 && other_col_idx < n_cols) {
        const char other_seat = grid[other_row_idx * n_cols + other_col_idx];
        if(other_seat != '.') {
            if(other_seat == '#') {
                return true;
            }
            break;
        }
        other_row_idx += d_row;
        other_col_idx += d_col;
    }
    return false;
}

static size_t count_occupied_adjacent_part_2(const char* const grid, size_t n_rows, size_t n_cols, int64_t row_idx, int64_t col_idx)
{
    size_t n_adjacent_occupied = 0;

    // Search up
    if(search_for_occupied_seat(grid, n_rows, n_cols, row_idx, col_idx, -1, 0)) {
        n_adjacent_occupied++;
    }

    // Search down
    if(search_for_occupied_seat(grid, n_rows, n_cols, row_idx, col_idx, 1, 0)) {
        n_adjacent_occupied++;
    }

    // Search left
    if(search_for_occupied_seat(grid, n_rows, n_cols, row_idx, col_idx, 0, -1)) {
        n_adjacent_occupied++;
    }

    // Search right
    if(search_for_occupied_seat(grid, n_rows, n_cols, row_idx, col_idx, 0, 1)) {
        n_adjacent_occupied++;
    }

    // Search NW diag
    if(search_for_occupied_seat(grid, n_rows, n_cols, row_idx, col_idx, -1, -1)) {
        n_adjacent_occupied++;
    }

    // Search NE diag
    if(search_for_occupied_seat(grid, n_rows, n_cols, row_idx, col_idx, -1, 1)) {
        n_adjacent_occupied++;
    }

    // Search SW diag
    if(search_for_occupied_seat(grid, n_rows, n_cols, row_idx, col_idx, 1, -1)) {
        n_adjacent_occupied++;
    }

    // Search SE diag
    if(search_for_occupied_seat(grid, n_rows, n_cols, row_idx, col_idx, 1, 1)) {
        n_adjacent_occupied++;
    }

    return n_adjacent_occupied;
}

// static void print_grid(const char* const grid, size_t n_rows, size_t n_cols)
// {
//     for(size_t row_idx = 0; row_idx < n_rows; row_idx++) {
//         char* str = malloc(n_cols + 1);
//         str[n_cols] = 0;
//         memcpy(str, &grid[row_idx * n_cols], n_cols);
//         printf("%s\n", str);
//     }
//     printf("\n");
// }

static uint64_t solve_part_2(const char* const input, size_t size)
{
    char* grid = NULL;
    size_t n_rows = 0;
    size_t n_cols = 0;
    parse_input(input, size, &grid, &n_rows, &n_cols);

    bool change_occurred = true;
    while(change_occurred) {
        change_occurred = false;
        char* new_grid = malloc(n_rows * n_cols);
        memcpy(new_grid, grid, n_rows * n_cols);

        for(size_t row_idx = 0; row_idx < n_rows; row_idx++) {
            for(size_t col_idx = 0; col_idx < n_cols; col_idx++) {
                switch(grid[row_idx * n_cols + col_idx]) {
                    case 'L': {
                        const size_t n_adjacent_occupied = count_occupied_adjacent_part_2(grid, n_rows, n_cols, row_idx, col_idx);
                        if(n_adjacent_occupied == 0) {
                            new_grid[row_idx * n_cols + col_idx] = '#';
                            change_occurred = true;
                        }
                        break;
                    }
                    case '#': {
                        const size_t n_adjacent_occupied = count_occupied_adjacent_part_2(grid, n_rows, n_cols, row_idx, col_idx);
                        if(n_adjacent_occupied >= 5) {
                            new_grid[row_idx * n_cols + col_idx] = 'L';
                            change_occurred = true;
                        }

                        break;
                    }
                }
            }
        }

        free(grid);
        grid = new_grid;

        //print_grid(grid, n_rows, n_cols);
    }

    size_t occupied_count = 0;
    for(size_t i = 0; i < n_rows * n_cols; i++) {
        if(grid[i] == '#') {
            occupied_count++;
        }
    }

    free(grid);

    return occupied_count;
}

void day_11_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_11_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(37, ans);
}

void day_11_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_11_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(2183, ans);
}

void day_11_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_11_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(26, ans);
}

void day_11_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_11_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(1990, ans);
}
