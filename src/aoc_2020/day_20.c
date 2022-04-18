#include "day_20.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"
#include "../mat.h"

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
    orientation_t orientation;
} tile_t;

typedef struct {
    tile_t** grid;
    size_t grid_dim;
} grid_t;

typedef struct {
    size_t id;
    mat_t mat;
} cell_t;

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
        .orientation = ORIENTATION_STD,
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
    tile_t cloned_tile = tile_create(tile.id, tile.top, tile.bottom, tile.left, tile.right);
    cloned_tile.orientation = tile.orientation;
    return cloned_tile;
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
        rotated_tile.orientation = ORIENTATION_HORI;
        break;
    }
    case ORIENTATION_VERT: {
        char* left = rotated_tile.left;
        rotated_tile.left = rotated_tile.right;
        rotated_tile.right = left;
        reverse_string(rotated_tile.top);
        reverse_string(rotated_tile.bottom);
        rotated_tile.orientation = ORIENTATION_VERT;
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
        rotated_tile.orientation = ORIENTATION_VERT_HORI;
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

        rotated_tile.orientation = ORIENTATION_CLK_NINETY;
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

        rotated_tile.orientation = ORIENTATION_ACLK_NINETY;
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

        rotated_tile.orientation = ORIENTATION_CLK_NINETY_VERT;
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

        rotated_tile.orientation = ORIENTATION_ACLK_NINETY_VERT;
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
            grid.grid[i][j].orientation = ORIENTATION_STD;
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

static bool get_solved_grid(const char* const input, size_t size, grid_t* output)
{
    size_t n_tiles = 0;
    tile_t* tiles = process_input(input, size, &n_tiles);

    const size_t grid_dim = (size_t)sqrtf(n_tiles);
    grid_t grid = grid_create(grid_dim);

    grid_t solved_grid;
    const bool solution_found = recursive_solve(tiles, n_tiles, grid, 0, 0, &solved_grid);

    if(!solution_found) {
        grid_destroy(solved_grid);
    }
    else {
        *output = solved_grid;
    }
    grid_destroy(grid);
    for(size_t tile_idx = 0; tile_idx < n_tiles; ++tile_idx) {
        tile_destroy(tiles[tile_idx]);
    }
    free(tiles);

    return solution_found;
}

static void get_cells(const char* const input, size_t size, cell_t** cells, size_t* n_cells)
{
    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;
    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);

    *n_cells = n_blocks;
    *cells = (cell_t*)malloc(*n_cells * sizeof(cell_t));

    for(size_t block_idx = 0; block_idx < n_blocks; ++block_idx) {
        const char* const block = &input[block_mem_refs[block_idx].offset];
        const size_t block_len = block_mem_refs[block_idx].size;

        memory_reference_t* line_mem_refs = NULL;
        size_t n_lines = 0;
        split_buf_by_sequence(block, block_len, "\n", &line_mem_refs, &n_lines);

        cell_t* cell = &(*cells)[block_idx];
        cell->mat = mat_create(n_lines - 3);

        {
            const char* const line = &block[line_mem_refs[0].offset];
            const size_t line_len = line_mem_refs[0].size;
            char temp[128] = { 0 };
            memcpy(temp, &line[5], line_len - 6);
            cell->id = atoi(temp);
        }

        for(size_t line_idx = 2; line_idx < n_lines - 1; ++line_idx) {
            const char* const line = &block[line_mem_refs[line_idx].offset];
            const size_t line_len = line_mem_refs[line_idx].size;

            for(size_t i = 1; i < line_len - 1; ++i) {
                cell->mat.data[(line_idx - 2) * cell->mat.dim + i - 1] = line[i] == '#' ? 1 : 0;
            }
        }

        free(line_mem_refs);
    }

    free(block_mem_refs);
}

static bool get_cell(const cell_t* cells, size_t n_cells, size_t id, cell_t* output_cell)
{
    for(size_t cell_idx = 0; cell_idx < n_cells; ++cell_idx) {
        if(cells[cell_idx].id == id) {
            *output_cell = cells[cell_idx];
            return true;
        }
    }
    return false;
}

static size_t solve_part_1(const char* const input, size_t size)
{
    size_t ans = 0;
    grid_t solved_grid;
    if(get_solved_grid(input, size, &solved_grid)) {
        ans = solved_grid.grid[0][0].id *
              solved_grid.grid[solved_grid.grid_dim - 1][0].id *
              solved_grid.grid[0][solved_grid.grid_dim - 1].id *
              solved_grid.grid[solved_grid.grid_dim - 1][solved_grid.grid_dim - 1].id;
        grid_destroy(solved_grid);
    }
    return ans;
}

static void mat_transform(mat_t mat, orientation_t orientation)
{
    switch(orientation) {
        case ORIENTATION_HORI:
            mat_mirror(mat, MIRROR_HORI);
            break;
        case ORIENTATION_VERT:
            mat_mirror(mat, MIRROR_VERT);
            break;
        case ORIENTATION_VERT_HORI:
            mat_mirror(mat, MIRROR_VERT);
            mat_mirror(mat, MIRROR_HORI);
            break;
        case ORIENTATION_CLK_NINETY:
            mat_rotate(mat, ROTATION_CLK_90);
            break;
        case ORIENTATION_ACLK_NINETY:
            mat_rotate(mat, ROTATION_ACLK_90);
            break;
        case ORIENTATION_CLK_NINETY_VERT:
            mat_rotate(mat, ROTATION_CLK_90);
            mat_mirror(mat, MIRROR_VERT);
            break;
        case ORIENTATION_ACLK_NINETY_VERT:
            mat_rotate(mat, ROTATION_ACLK_90);
            mat_mirror(mat, MIRROR_VERT);
            break;
        case ORIENTATION_FINAL:
            assert(false);
        case ORIENTATION_STD:
        default:
            break;
    }
}

static mat_t create_lake(grid_t solved_grid, cell_t* cells, size_t n_cells)
{
    mat_t lake = mat_create(cells[0].mat.dim * solved_grid.grid_dim);

    for(size_t i = 0; i < solved_grid.grid_dim; ++i) {
        for(size_t j = 0; j < solved_grid.grid_dim; ++j) {
            const size_t id = solved_grid.grid[i][j].id;
            cell_t this_cell;
            const bool __attribute__((unused)) cell_found = get_cell(cells, n_cells, id, &this_cell);
            assert(cell_found);

            const orientation_t orientation = solved_grid.grid[i][j].orientation;
            mat_t final_mat = mat_clone(this_cell.mat);
            mat_transform(final_mat, orientation);

            const size_t lake_row_offset = i * final_mat.dim;
            const size_t lake_col_offset = j * final_mat.dim;
            for(size_t row = 0; row < final_mat.dim; ++row) {
                memcpy(
                    &lake.data[(row + lake_row_offset) * lake.dim + lake_col_offset],
                    &final_mat.data[row * final_mat.dim],
                    final_mat.dim * sizeof(mat_dtype_t)
                );
            }
            mat_destroy(final_mat);
        }
    }

    return lake;
}

static size_t solve_part_2(const char* const input, size_t size)
{
    size_t non_monster_high_bits = 0;

    grid_t solved_grid;
    if(get_solved_grid(input, size, &solved_grid)) {
        cell_t* cells = NULL;
        size_t n_cells = 0;
        get_cells(input, size, &cells, &n_cells);

        mat_t lake = create_lake(solved_grid, cells, n_cells);

        size_t monster_bit_len = 20;
        uint32_t monster[3] = {
            2,
            549255,
            299592,
        };
        const size_t n_monster_high_bits = 15;
        non_monster_high_bits = 0;

        for(size_t orientation = 0; orientation < ORIENTATION_FINAL; ++orientation) {
            size_t match_count = 0;
            mat_t trans_lake = mat_clone(lake);
            mat_transform(trans_lake, orientation);
            for(size_t row = 0; row < trans_lake.dim - 2; ++row) {
                for(size_t offset = 0; offset < trans_lake.dim - monster_bit_len + 1; ++offset) {
                    bool match_found = true;
                    for(size_t monst_num_idx = 0; monst_num_idx < 3; ++monst_num_idx) {
                        uint32_t num = 0;
                        for(size_t col = trans_lake.dim - monster_bit_len - offset; col < trans_lake.dim - offset; ++col) {
                            const uint8_t val = trans_lake.data[(row + monst_num_idx) * trans_lake.dim + col];
                            num = (num << 1) | val;
                        }
                        if(monster[monst_num_idx] != (monster[monst_num_idx] & num)) {
                            match_found = false;
                            break;
                        }
                    }
                    if(match_found) {
                        match_count++;
                    }
                }
            }
            if(match_count) {
                for(size_t i = 0; i < trans_lake.dim * trans_lake.dim; ++i) {
                    non_monster_high_bits += trans_lake.data[i];
                }
                non_monster_high_bits -= match_count * n_monster_high_bits;
                mat_destroy(trans_lake);
                break;
            }
            mat_destroy(trans_lake);
        }

        mat_destroy(lake);
        for(size_t cell_idx = 0; cell_idx < n_cells; ++cell_idx) {
            mat_destroy(cells[cell_idx].mat);
        }
        free(cells);
        grid_destroy(solved_grid);
    }

    return non_monster_high_bits;
}

void day_20_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_20_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(20899048083289, ans);
}

void day_20_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_20_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(13983397496713, ans);
}

void day_20_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_20_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(273, ans);
}

void day_20_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_20_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(2424, ans);
}

