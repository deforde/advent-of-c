#include "day_24.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <unity.h>

#include "../utility.h"

static size_t solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    int32_t coords[512][2] = {0};
    size_t n_coords = 0;

    for(size_t line_idx = 0; line_idx < n_lines; ++line_idx) {
        const char* const line = &input[line_mem_refs[line_idx].offset];
        const size_t line_len = line_mem_refs[line_idx].size;

        if(line_len) {
            int32_t x = 0;
            int32_t y = 0;
            for(size_t i = 0; i < line_len;) {
                switch(line[i++]) {
                    case 'e':
                        x += 2;
                        break;
                    case 'w':
                        x -= 2;
                        break;
                    case 'n':
                        y += 1;
                        switch(line[i++]) {
                            case 'e':
                                x += 1;
                                break;
                            case 'w':
                                x -= 1;
                                break;
                            default:
                                assert(false);
                                break;
                        }
                        break;
                    case 's':
                        y -= 1;
                        switch(line[i++]) {
                            case 'e':
                                x += 1;
                                break;
                            case 'w':
                                x -= 1;
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

            bool extant_tile_found = false;
            for(size_t i = 0; i < n_coords; ++i) {
                if(x == coords[i][0] && y == coords[i][1]) {
                    coords[i][0] = coords[n_coords - 1][0];
                    coords[i][1] = coords[n_coords - 1][1];
                    coords[n_coords - 1][0] = 0;
                    coords[n_coords - 1][1] = 0;
                    n_coords--;
                    extant_tile_found = true;
                    // printf("Unflipping previous tile (%lu)\n", n_coords);
                    break;
                }
            }
            if(!extant_tile_found) {
                coords[n_coords][0] = x;
                coords[n_coords][1] = y;
                n_coords++;
                // printf("Flipping new tile (%lu)\n", n_coords);
            }
        }
    }

    free(line_mem_refs);

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

    TEST_ASSERT_EQUAL_size_t(10, ans);
}

