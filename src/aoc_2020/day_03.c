#include "day_03.h"

#include <assert.h>
#include <stdlib.h>
#include <unity.h>

#include "../utility.h"

typedef struct {
    size_t dx;
    size_t dy;
} slope_t;

static size_t solve(const char* const input, size_t size, slope_t* slopes, size_t num_slopes)
{
    size_t num_cols = 0;
    for(size_t i = 0; i < size; i++) {
        if(input[i] == '\n') {
            break;
        }
        num_cols++;
    }

    size_t num_rows = 0;
    for(size_t i = 0; i < size; i++) {
        if(input[i] == '\n') {
            num_rows++;
        }
    }

    int32_t* grid = (int32_t*)malloc(num_rows * num_cols * sizeof(int32_t));

    size_t idx = 0;
    for(size_t i = 0; i < size; i++) {
        if(input[i] != '\n') {
            assert(idx < num_rows * num_cols);
            grid[idx++] = input[i] == '#' ? 1 : 0;
        }
    }

    size_t product = 1;

    for(size_t i = 0; i < num_slopes; i++) {
        size_t dx = slopes[i].dx;
        size_t dy = slopes[i].dy;

        size_t x = 0;
        size_t y = 0;
        size_t sum = 0;
        while(y < num_rows) {
            sum += grid[x + y * num_cols];
            x = (x + dx) % num_cols;
            y += dy;
        }

        product *= sum;
    }

    free(grid);

    return product;
}

void day_03_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_03_part_1_example.txt", &input, &size);
    assert(success);

    slope_t slope = {
        .dx = 3,
        .dy = 1
    };
    const size_t ans = solve(input, size, &slope, 1);
    TEST_ASSERT_EQUAL_UINT64(7, ans);

    free(input);
}

void day_03_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_03_part_1_input.txt", &input, &size);
    assert(success);

    slope_t slope = {
        .dx = 3,
        .dy = 1
    };
    const size_t ans = solve(input, size, &slope, 1);
    TEST_ASSERT_EQUAL_UINT64(265, ans);

    free(input);
}

void day_03_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_03_part_1_example.txt", &input, &size);
    assert(success);

    slope_t slopes[5];
    slopes[0].dx = 1;
    slopes[0].dy = 1;
    slopes[1].dx = 3;
    slopes[1].dy = 1;
    slopes[2].dx = 5;
    slopes[2].dy = 1;
    slopes[3].dx = 7;
    slopes[3].dy = 1;
    slopes[4].dx = 1;
    slopes[4].dy = 2;

    const size_t ans = solve(input, size, slopes, 5);
    TEST_ASSERT_EQUAL_UINT64(336, ans);

    free(input);
}

void day_03_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_03_part_1_input.txt", &input, &size);
    assert(success);

    slope_t slopes[5];
    slopes[0].dx = 1;
    slopes[0].dy = 1;
    slopes[1].dx = 3;
    slopes[1].dy = 1;
    slopes[2].dx = 5;
    slopes[2].dy = 1;
    slopes[3].dx = 7;
    slopes[3].dy = 1;
    slopes[4].dx = 1;
    slopes[4].dy = 2;

    const size_t ans = solve(input, size, slopes, 5);
    TEST_ASSERT_EQUAL_UINT64(3154761400, ans);

    free(input);
}
