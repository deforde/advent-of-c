#include "day_20.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

#define DEFAULT_VEC_CAPACITY 100
#define DEFAULT_VEC_GROWTH_FACTOR 2

typedef enum {
    ORIENTATION_STD,
    ORIENTATION_VERT,
    ORIENTATION_HORI,
    ORIENTATION_VERT_HORI,
    ORIENTATION_CLK_NINETY,
    ORIENTATION_ACLK_NINETY,
    ORIENTATION_CLK_NINETY_VERT,
    ORIENTATION_ACLK_NINETY_VERT,
    ORIENTATION_FINAL,
} orientation_t;

typedef struct {
    size_t id;
    char* top;
    char* bottom;
    char* left;
    char* right;
} tile_t;

typedef struct {
    tile_t** grid;
    size_t grid_dim;
} grid_t;

static void reverse_string(char* str)
{
    const size_t len = strlen(str);
    for(size_t i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

static tile_t tile_create(size_t id, const char* const top, const char* const bottom, const char* const left, const char* const right)
{
    const size_t len = strlen(top);

    tile_t tile = {
        .id = id,
        .top = (char*)malloc(len + 1),
        .bottom = (char*)malloc(len + 1),
        .left = (char*)malloc(len + 1),
        .right = (char*)malloc(len + 1),
    };

    tile.top[len] = 0;
    tile.bottom[len] = 0;
    tile.left[len] = 0;
    tile.right[len] = 0;

    memcpy(tile.top, top, len);
    memcpy(tile.bottom, bottom, len);
    memcpy(tile.left, left, len);
    memcpy(tile.right, right, len);

    return tile;
}

static tile_t tile_clone(tile_t tile)
{
    return tile_create(tile.id, tile.top, tile.bottom, tile.left, tile.right);
}

static tile_t tile_rotate(tile_t tile, orientation_t orientation)
{
    tile_t rotated_tile = tile_clone(tile);

    switch(orientation) {
    case ORIENTATION_HORI: {
        char* top = rotated_tile.top;
        rotated_tile.top = rotated_tile.bottom;
        rotated_tile.bottom = top;
        reverse_string(rotated_tile.left);
        reverse_string(rotated_tile.right);
        break;
    }
    case ORIENTATION_VERT: {
        char* left = rotated_tile.left;
        rotated_tile.left = rotated_tile.right;
        rotated_tile.right = left;
        reverse_string(rotated_tile.top);
        reverse_string(rotated_tile.bottom);
        break;
    }
    case ORIENTATION_VERT_HORI: {
        char* top = rotated_tile.top;
        rotated_tile.top = rotated_tile.bottom;
        rotated_tile.bottom = top;
        char* left = rotated_tile.left;
        rotated_tile.left = rotated_tile.right;
        rotated_tile.right = left;
        reverse_string(rotated_tile.top);
        reverse_string(rotated_tile.bottom);
        reverse_string(rotated_tile.left);
        reverse_string(rotated_tile.right);
        break;
    }
    case ORIENTATION_CLK_NINETY: {
        char* top = rotated_tile.top;
        char* bottom = rotated_tile.bottom;
        char* left = rotated_tile.left;
        char* right = rotated_tile.right;

        rotated_tile.top = left;
        rotated_tile.bottom = right;
        rotated_tile.left = bottom;
        rotated_tile.right = top;

        reverse_string(rotated_tile.top);
        reverse_string(rotated_tile.bottom);
        break;
    }
    case ORIENTATION_ACLK_NINETY: {
        char* top = rotated_tile.top;
        char* bottom = rotated_tile.bottom;
        char* left = rotated_tile.left;
        char* right = rotated_tile.right;

        rotated_tile.top = right;
        rotated_tile.bottom = left;
        rotated_tile.left = top;
        rotated_tile.right = bottom;

        reverse_string(rotated_tile.left);
        reverse_string(rotated_tile.right);
        break;
    }
    case ORIENTATION_CLK_NINETY_VERT: {
        char* top = rotated_tile.top;
        char* bottom = rotated_tile.bottom;
        char* left = rotated_tile.left;
        char* right = rotated_tile.right;

        rotated_tile.top = left;
        rotated_tile.bottom = right;
        rotated_tile.left = top;
        rotated_tile.right = bottom;

        break;
    }
    case ORIENTATION_ACLK_NINETY_VERT: {
        char* top = rotated_tile.top;
        char* bottom = rotated_tile.bottom;
        char* left = rotated_tile.left;
        char* right = rotated_tile.right;

        rotated_tile.top = right;
        rotated_tile.bottom = left;
        rotated_tile.left = bottom;
        rotated_tile.right = top;

        reverse_string(rotated_tile.top);
        reverse_string(rotated_tile.bottom);
        reverse_string(rotated_tile.left);
        reverse_string(rotated_tile.right);

        break;
    }
    case ORIENTATION_FINAL:
        assert(false);
    case ORIENTATION_STD:
    default:
        break;
    }

    return rotated_tile;
}

static void tile_destroy(tile_t tile)
{
    free(tile.top);
    free(tile.bottom);
    free(tile.left);
    free(tile.right);
}

static grid_t grid_create(size_t grid_dim)
{
    grid_t grid = {
        .grid = (tile_t**)malloc(grid_dim * sizeof(tile_t*)),
        .grid_dim = grid_dim,
    };
    for(size_t i = 0; i < grid_dim; ++i) {
        grid.grid[i] = (tile_t*)malloc(grid_dim * sizeof(tile_t));
        for(size_t j = 0; j < grid_dim; ++j) {
            grid.grid[i][j].id = 0;
            grid.grid[i][j].top = NULL;
            grid.grid[i][j].bottom = NULL;
            grid.grid[i][j].left = NULL;
            grid.grid[i][j].right = NULL;
        }
    }
    return grid;
}

static void grid_destroy(grid_t grid)
{
    for(size_t i = 0; i < grid.grid_dim; ++i) {
        for(size_t j = 0; j < grid.grid_dim; ++j) {
            tile_destroy(grid.grid[i][j]);
        }
        free(grid.grid[i]);
    }
    free(grid.grid);
}

static grid_t grid_clone(grid_t src_grid)
{
    grid_t dst_grid = grid_create(src_grid.grid_dim);
    for(size_t i = 0; i < src_grid.grid_dim; ++i) {
        for(size_t j = 0; j < src_grid.grid_dim; ++j) {
            if(src_grid.grid[i][j].id != 0) {
                dst_grid.grid[i][j] = tile_clone(src_grid.grid[i][j]);
            }
        }
    }
    return dst_grid;
}

static tile_t* grid_find(const grid_t grid, size_t id)
{
    for(size_t i = 0; i < grid.grid_dim; ++i) {
        for(size_t j = 0; j < grid.grid_dim; ++j) {
            if(grid.grid[i][j].id == id) {
                return &grid.grid[i][j];
            }
        }
    }
    return NULL;
}

static tile_t* process_input(const char* const input, size_t size, size_t* n_tiles)
{
    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;
    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);

    *n_tiles = n_blocks;
    tile_t* tiles = (tile_t*)malloc(*n_tiles * sizeof(tile_t));

    for(size_t block_num = 0; block_num < n_blocks; ++block_num) {
        const char* const block = &input[block_mem_refs[block_num].offset];
        const size_t block_len = block_mem_refs[block_num].size;

        tile_t* tile = &tiles[block_num];

        memory_reference_t* line_mem_refs = NULL;
        size_t n_lines = 0;
        split_buf_by_sequence(block, block_len, "\n", &line_mem_refs, &n_lines);

        size_t id = 0;
        char* top = NULL;
        char* bottom = NULL;
        char* left = (char*)malloc(n_lines);
        left[n_lines - 1] = 0;
        char* right = (char*)malloc(n_lines);
        right[n_lines - 1] = 0;

        {
            const char* const line = &block[line_mem_refs[0].offset];
            const size_t line_len = line_mem_refs[0].size;
            char temp[128] = { 0 };
            memcpy(temp, &line[5], line_len - 6);
            id = atoi(temp);
        }
        {
            const char* const line = &block[line_mem_refs[1].offset];
            const size_t line_len = line_mem_refs[1].size;
            top = (char*)malloc(line_len + 1);
            top[line_len] = 0;
            memcpy(top, line, line_len);
        }
        {
            const char* const line = &block[line_mem_refs[n_lines - 1].offset];
            const size_t line_len = line_mem_refs[n_lines - 1].size;
            bottom = (char*)malloc(line_len + 1);
            bottom[line_len] = 0;
            memcpy(bottom, line, line_len);
        }
        for(size_t line_num = 1; line_num < n_lines; ++line_num) {
            const char* const line = &block[line_mem_refs[line_num].offset];
            const size_t line_len = line_mem_refs[line_num].size;
            left[line_num - 1] = line[0];
            right[line_num - 1] = line[line_len - 1];
        }

        *tile = tile_create(id, top, bottom, left, right);

        free(top);
        free(bottom);
        free(left);
        free(right);
        free(line_mem_refs);
    }

    free(block_mem_refs);

    return tiles;
}

static bool recursive_solve(const tile_t* tiles, size_t n_tiles, grid_t grid, size_t row, size_t col, grid_t* solved_grid)
{
    assert(grid.grid[row][col].id == 0);

    for(size_t tile_idx = 0; tile_idx < n_tiles; ++tile_idx) {
        const tile_t tile = tiles[tile_idx];
        const size_t id = tile.id;
        assert(grid_find(grid, id) == NULL);

        tile_t* tile_above = NULL;
        tile_t* tile_below = NULL;
        tile_t* tile_left = NULL;
        tile_t* tile_right = NULL;
        if(row > 0 && grid.grid[row - 1][col].id != 0) {
            tile_above = &grid.grid[row - 1][col];
        }
        if(row < (grid.grid_dim - 1) && grid.grid[row + 1][col].id != 0) {
            tile_below = &grid.grid[row + 1][col];
        }
        if(col > 0 && grid.grid[row][col - 1].id != 0) {
            tile_left = &grid.grid[row][col - 1];
        }
        if(col < (grid.grid_dim - 1) && grid.grid[row][col + 1].id != 0) {
            tile_right = &grid.grid[row][col + 1];
        }

        for(size_t orientation = 0; orientation < ORIENTATION_FINAL; ++orientation) {
            const tile_t rotated_tile = tile_rotate(tile, orientation);
            bool is_tile_compatible = true;

            if(tile_above && is_tile_compatible) {
                is_tile_compatible &= strncmp(rotated_tile.top, tile_above->bottom, strlen(rotated_tile.top)) == 0;
            }
            if(tile_below && is_tile_compatible) {
                is_tile_compatible &= strncmp(rotated_tile.bottom, tile_below->top, strlen(rotated_tile.bottom)) == 0;
            }
            if(tile_left && is_tile_compatible) {
                is_tile_compatible &= strncmp(rotated_tile.left, tile_left->right, strlen(rotated_tile.left)) == 0;
            }
            if(tile_right && is_tile_compatible) {
                is_tile_compatible &= strncmp(rotated_tile.right, tile_right->left, strlen(rotated_tile.right)) == 0;
            }
            if(!is_tile_compatible) {
                tile_destroy(rotated_tile);
                continue;
            }

            grid_t new_grid = grid_clone(grid);
            new_grid.grid[row][col] = rotated_tile;

            if(row == (new_grid.grid_dim - 1) && col == (new_grid.grid_dim - 1)) {
                *solved_grid = new_grid;
                return true;
            }

            tile_t* remaining_tiles = (tile_t*)malloc(n_tiles * sizeof(tile_t));
            for(size_t i = 0; i < n_tiles; ++i) {
                if(i != tile_idx) {
                    remaining_tiles[i] = tile_clone(tiles[i]);
                }
            }
            remaining_tiles[tile_idx] = remaining_tiles[n_tiles - 1];
            const size_t n_remaining_tiles = n_tiles - 1;

            size_t next_row = row;
            size_t next_col = col + 1;
            if(next_col > (new_grid.grid_dim - 1)) {
                next_col = 0;
                ++next_row;
            }

            const bool solved = recursive_solve(remaining_tiles, n_remaining_tiles, new_grid, next_row, next_col, solved_grid);

            for(size_t i = 0; i < n_remaining_tiles; ++i) {
                tile_destroy(remaining_tiles[i]);
            }
            free(remaining_tiles);
            grid_destroy(new_grid);

            if(solved) {
                return true;
            }
        }
    }

    return false;
}

static size_t solve_part(const char* const input, size_t size)
{
    size_t n_tiles = 0;
    tile_t* tiles = process_input(input, size, &n_tiles);

    const size_t grid_dim = (size_t)sqrtf(n_tiles);
    grid_t grid = grid_create(grid_dim);

    size_t ans = 0;
    grid_t solved_grid;
    if(recursive_solve(tiles, n_tiles, grid, 0, 0, &solved_grid)) {
        ans = solved_grid.grid[0][0].id * solved_grid.grid[grid_dim - 1][0].id * solved_grid.grid[0][grid_dim - 1].id * solved_grid.grid[grid_dim - 1][grid_dim - 1].id;
        grid_destroy(solved_grid);
    }

    grid_destroy(grid);
    for(size_t tile_idx = 0; tile_idx < n_tiles; ++tile_idx) {
        tile_destroy(tiles[tile_idx]);
    }
    free(tiles);

    return ans;
}

void day_20_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_20_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(20899048083289, ans);
}

void day_20_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_20_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(20899048083289, ans);
}

