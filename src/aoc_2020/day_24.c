#include "day_24.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

#define MAX_NUM_COORDS 8192
#define NUM_ADJACENT_TILES 6

static void process_tile_flip(const char* instr, size_t instr_len, int32_t* x, int32_t* y)
{
    *x = 0;
    *y = 0;
    for(size_t i = 0; i < instr_len;) {
        switch(instr[i++]) {
            case 'e':
                *x += 2;
                break;
            case 'w':
                *x -= 2;
                break;
            case 'n':
                *y += 1;
                switch(instr[i++]) {
                    case 'e':
                        *x += 1;
                        break;
                    case 'w':
                        *x -= 1;
                        break;
                    default:
                        assert(false);
                        break;
                }
                break;
            case 's':
                *y -= 1;
                switch(instr[i++]) {
                    case 'e':
                        *x += 1;
                        break;
                    case 'w':
                        *x -= 1;
                        break;
                    default:
                        assert(false);
                        break;
                }
                break;
            default:
                assert(false);
                break;
        }
        // printf("(%i, %i)\n", x, y);
    }
}

static void get_initial_tile_coords(const char* const input, size_t size, int32_t coords[MAX_NUM_COORDS][2], size_t* n_coords)
{
    *n_coords = 0;
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    for(size_t line_idx = 0; line_idx < n_lines; ++line_idx) {
        const char* const line = &input[line_mem_refs[line_idx].offset];
        const size_t line_len = line_mem_refs[line_idx].size;

        if(line_len) {
            int32_t x = 0;
            int32_t y = 0;
            process_tile_flip(line, line_len, &x, &y);

            bool extant_tile_found = false;
            for(size_t i = 0; i < *n_coords; ++i) {
                if(x == coords[i][0] && y == coords[i][1]) {
                    coords[i][0] = coords[*n_coords - 1][0];
                    coords[i][1] = coords[*n_coords - 1][1];
                    coords[*n_coords - 1][0] = 0;
                    coords[*n_coords - 1][1] = 0;
                    (*n_coords)--;
                    extant_tile_found = true;
                    // printf("Unflipping previous tile (%lu)\n", *n_coords);
                    break;
                }
            }
            if(!extant_tile_found) {
                coords[*n_coords][0] = x;
                coords[*n_coords][1] = y;
                (*n_coords)++;
                // printf("Flipping new tile (%lu)\n", *n_coords);
            }
        }
    }

    free(line_mem_refs);
}

static size_t solve_part_1(const char* const input, size_t size)
{
    int32_t coords[MAX_NUM_COORDS][2] = {0};
    size_t n_coords = 0;

    get_initial_tile_coords(input, size, coords, &n_coords);

    return n_coords;
}

static bool is_contained(int32_t coords[MAX_NUM_COORDS][2], size_t n_coords, int32_t x, int32_t y)
{
    for(size_t i = 0; i < n_coords; ++i) {
        if(x == coords[i][0] && y == coords[i][1]) {
            return true;
        }
    }
    return false;
}

static size_t count_adjacent_tiles(int32_t coords[MAX_NUM_COORDS][2], size_t n_coords, int32_t x, int32_t y)
{
    int32_t adjacent_tiles[NUM_ADJACENT_TILES][2] = {
        {x + 1, y + 1},
        {x + 2, y    },
        {x + 1, y - 1},
        {x - 1, y - 1},
        {x - 2, y    },
        {x - 1, y + 1},
    };

    size_t adjacent_tile_cnt = 0;
    for(size_t k = 0; k < NUM_ADJACENT_TILES; ++k) {
        int32_t xi = adjacent_tiles[k][0];
        int32_t yi = adjacent_tiles[k][1];
        if(is_contained(coords, n_coords, xi, yi)) {
            adjacent_tile_cnt++;
            if(adjacent_tile_cnt > 2) {
                break;
            }
        }
    }

    return adjacent_tile_cnt;
}

static void add_coord(int32_t coords[MAX_NUM_COORDS][2], size_t* n_coords, int32_t x, int32_t y)
{
    if(!is_contained(coords, *n_coords, x, y)) {
        coords[*n_coords][0] = x;
        coords[*n_coords][1] = y;
        assert(*n_coords < MAX_NUM_COORDS);
        (*n_coords)++;
    }
}

static void remove_coord(int32_t coords[MAX_NUM_COORDS][2], size_t* n_coords, int32_t x, int32_t y)
{
    for(size_t i = 0; i < *n_coords; ++i) {
        if(x == coords[i][0] && y == coords[i][1]) {
            coords[i][0] = coords[*n_coords - 1][0];
            coords[i][1] = coords[*n_coords - 1][1];
            coords[*n_coords - 1][0] = 0;
            coords[*n_coords - 1][1] = 0;
            (*n_coords)--;
            return;
        }
    }
}

static size_t solve_part_2(const char* const input, size_t size)
{
    int32_t coords[MAX_NUM_COORDS][2] = {0};
    size_t n_coords = 0;

    get_initial_tile_coords(input, size, coords, &n_coords);

    int32_t new_coords[MAX_NUM_COORDS][2] = {0};
    size_t n_new_coords = 0;
    const size_t n_iterations = 100;

    for(size_t i = 0; i < n_iterations; ++i) {
        memcpy(new_coords, coords, n_coords * 2 * sizeof(int32_t));
        n_new_coords = n_coords;

        for(size_t j = 0; j < n_coords; ++j) {
            const int32_t x = coords[j][0];
            const int32_t y = coords[j][1];

            const size_t adjacent_tile_cnt = count_adjacent_tiles(coords, n_coords, x, y);
            if(adjacent_tile_cnt == 0 || adjacent_tile_cnt > 2) {
                remove_coord(new_coords, &n_new_coords, x, y);
            }

            int32_t adjacent_tiles[NUM_ADJACENT_TILES][2] = {
                {x + 1, y + 1},
                {x + 2, y    },
                {x + 1, y - 1},
                {x - 1, y - 1},
                {x - 2, y    },
                {x - 1, y + 1},
            };
            for(size_t k = 0; k < NUM_ADJACENT_TILES; ++k) {
                int32_t xi = adjacent_tiles[k][0];
                int32_t yi = adjacent_tiles[k][1];
                if(!is_contained(coords, n_coords, xi, yi) &&
                    count_adjacent_tiles(coords, n_coords, xi, yi) == 2) {
                    add_coord(new_coords, &n_new_coords, xi, yi);
                }
            }
        }

        memcpy(coords, new_coords, n_new_coords * 2 * sizeof(int32_t));
        n_coords = n_new_coords;
        n_new_coords = 0;
    }

    return n_coords;
}

void day_24_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_24_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(10, ans);
}

void day_24_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_24_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(427, ans);
}

void day_24_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_24_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(2208, ans);
}

void day_24_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_24_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(3837, ans);
}

