#include "day_17.h"

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
    int64_t x;
    int64_t y;
    int64_t z;
    int64_t w;
} coord_t;

typedef struct {
    coord_t* coords;
    size_t n_coords;
    size_t capacity;
} field_t;

static field_t init_field()
{
    const size_t initial_capacity = 1000;
    coord_t* coords = (coord_t*)malloc(initial_capacity * sizeof(coord_t*));
    field_t field = {
        .coords = coords,
        .n_coords = 0,
        .capacity = initial_capacity
    };
    return field;
}

static void add_coord(field_t* field, coord_t coord)
{
    if(field->n_coords == field->capacity) {
        field->capacity *= 2;
        coord_t* prev_coords = field->coords;
        coord_t* curr_coords = (coord_t*)malloc(field->capacity * sizeof(coord_t));
        memcpy(curr_coords, prev_coords, field->n_coords * sizeof(coord_t));
        free(prev_coords);
        field->coords = curr_coords;
    }

    field->coords[field->n_coords++] = coord;
}

static void remove_coord(field_t* field, coord_t coord)
{
    for(size_t coord_idx = 0; coord_idx < field->n_coords; coord_idx++) {
        coord_t contained_coord = field->coords[coord_idx];
        if(contained_coord.x == coord.x && contained_coord.y == coord.y && contained_coord.z == coord.z && contained_coord.w == coord.w) {
            field->coords[coord_idx] = field->coords[field->n_coords - 1];
            field->n_coords--;
            break;
        }
    }
}

static void destroy_field(field_t* field)
{
    free(field->coords);
}

static field_t clone_field(field_t* field)
{
    field_t cloned_field = {
        .coords = (coord_t*)malloc(field->capacity * sizeof(coord_t)),
        .n_coords = field->n_coords,
        .capacity = field->capacity
    };
    memcpy(cloned_field.coords, field->coords, field->n_coords * sizeof(coord_t));
    return cloned_field;
}

static bool is_coord_contained(field_t* field, coord_t coord)
{
    for(size_t coord_idx = 0; coord_idx < field->n_coords; coord_idx++) {
        coord_t contained_coord = field->coords[coord_idx];
        if(contained_coord.x == coord.x && contained_coord.y == coord.y && contained_coord.z == coord.z && contained_coord.w == coord.w) {
            return true;
        }
    }
    return false;
}

static void get_adjacent_coords_3d(coord_t origin, coord_t** output_adj_coords, size_t* output_n_adjacent_coords)
{
    size_t total_n_adjacent_coords = powf(3, 3) - 1;
    coord_t* adj_coords = (coord_t*)malloc(total_n_adjacent_coords * sizeof(coord_t));
    size_t n_adjacent_coords = 0;

    for(int64_t x = origin.x - 1; x < origin.x + 2; x++) {
        for(int64_t y = origin.y - 1; y < origin.y + 2; y++) {
            for(int64_t z = origin.z - 1; z < origin.z + 2; z++) {
                if(x != origin.x || y != origin.y || z != origin.z) {
                    adj_coords[n_adjacent_coords].x = x;
                    adj_coords[n_adjacent_coords].y = y;
                    adj_coords[n_adjacent_coords].z = z;
                    adj_coords[n_adjacent_coords].w = 0;
                    n_adjacent_coords++;
                }
            }
        }
    }
    assert(n_adjacent_coords == total_n_adjacent_coords);

    *output_adj_coords = adj_coords;
    *output_n_adjacent_coords = n_adjacent_coords;
}

static void get_adjacent_coords_4d(coord_t origin, coord_t** output_adj_coords, size_t* output_n_adjacent_coords)
{
    size_t total_n_adjacent_coords = powf(3, 4) - 1;
    coord_t* adj_coords = (coord_t*)malloc(total_n_adjacent_coords * sizeof(coord_t));
    size_t n_adjacent_coords = 0;

    for(int64_t x = origin.x - 1; x < origin.x + 2; x++) {
        for(int64_t y = origin.y - 1; y < origin.y + 2; y++) {
            for(int64_t z = origin.z - 1; z < origin.z + 2; z++) {
                for(int64_t w = origin.w - 1; w < origin.w + 2; w++) {
                    if(x != origin.x || y != origin.y || z != origin.z || w != origin.w) {
                        adj_coords[n_adjacent_coords].x = x;
                        adj_coords[n_adjacent_coords].y = y;
                        adj_coords[n_adjacent_coords].z = z;
                        adj_coords[n_adjacent_coords].w = w;
                        n_adjacent_coords++;
                    }
                }
            }
        }
    }
    assert(n_adjacent_coords == total_n_adjacent_coords);

    *output_adj_coords = adj_coords;
    *output_n_adjacent_coords = n_adjacent_coords;
}

static void get_adjacent_coords(coord_t origin, size_t n_dims, coord_t** output_adj_coords, size_t* output_n_adjacent_coords)
{
    switch(n_dims) {
        case 3:
            get_adjacent_coords_3d(origin, output_adj_coords, output_n_adjacent_coords);
        break;
        case 4:
            get_adjacent_coords_4d(origin, output_adj_coords, output_n_adjacent_coords);
        break;
        default:
        assert(false);
        break;
    }
}

static size_t count_active(field_t* field, coord_t* coords, size_t n_coords)
{
    size_t n_active = 0;
    for(size_t idx = 0; idx < n_coords; idx++) {
        if(is_coord_contained(field, coords[idx])) {
            n_active++;
        }
    }
    return n_active;
}

static size_t count_active_adjacent(field_t* field, coord_t coord, size_t n_dims)
{
    coord_t* adj_coords = NULL;
    size_t n_adj_coords = 0;
    get_adjacent_coords(coord, n_dims, &adj_coords, &n_adj_coords);

    const size_t n_active_adj = count_active(field, adj_coords, n_adj_coords);

    free(adj_coords);

    return n_active_adj;
}

static int64_t solve(const char* const input, size_t size, size_t n_dims)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    field_t field = init_field();

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const line_buf = &input[line_mem_refs[line_idx].offset];
        const size_t line_len = line_mem_refs[line_idx].size;

        for(size_t pos = 0; pos < line_len; pos++) {
            if(line_buf[pos] == '#') {
                coord_t coord = {
                    .x = line_idx,
                    .y = pos,
                    .z = 0,
                    .w = 0
                };
                add_coord(&field, coord);
            }
        }
    }

    for(size_t iteration = 0; iteration < 6; iteration++) {
        field_t new_field = clone_field(&field);

        for(size_t coord_idx = 0; coord_idx < field.n_coords; coord_idx++) {
            coord_t this_coord = field.coords[coord_idx];

            coord_t* adj_coords = NULL;
            size_t n_adj_coords = 0;
            get_adjacent_coords(this_coord, n_dims, &adj_coords, &n_adj_coords);

            const size_t n_active_adj_coords = count_active(&field, adj_coords, n_adj_coords);

            if(n_active_adj_coords != 2 && n_active_adj_coords != 3) {
                remove_coord(&new_field, this_coord);
            }

            for(size_t adj_coord_idx = 0; adj_coord_idx < n_adj_coords; adj_coord_idx++) {
                if(!is_coord_contained(&field, adj_coords[adj_coord_idx]) && !is_coord_contained(&new_field, adj_coords[adj_coord_idx])) {
                    size_t n_active_second_adj_coords = count_active_adjacent(&field, adj_coords[adj_coord_idx], n_dims);

                    if(n_active_second_adj_coords == 3) {
                        add_coord(&new_field, adj_coords[adj_coord_idx]);
                    }
                }
            }

            free(adj_coords);
        }

        destroy_field(&field);
        field = new_field;
    }

    size_t n_active_coords = field.n_coords;

    destroy_field(&field);
    free(line_mem_refs);

    return n_active_coords;
}

void day_17_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_17_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve(input, size, 3);


    free(input);

    TEST_ASSERT_EQUAL_UINT64(112, ans);
}

void day_17_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_17_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve(input, size, 3);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(240, ans);
}

void day_17_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_17_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve(input, size, 4);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(848, ans);
}

void day_17_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_17_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve(input, size, 4);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(1180, ans);
}
