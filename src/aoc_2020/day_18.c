#include "day_18.h"

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

static int64_t execute_operation(int64_t a, int64_t b, char op)
{
    switch(op) {
        case '*':
            return a * b;
        break;
        case '+':
            return a + b;
        break;
        default:
            assert(false);
        break;
    }
    return 0;
}

static int64_t recursive_parse_expr(const char* const expr, size_t len, size_t* idx, int64_t a, char op)
{
    while(*idx != len) {
        while(isspace(expr[*idx]) != 0) {
            (*idx)++;
        }

        if(expr[*idx] == ')') {
            (*idx)++;
            return a;
        }

        if(expr[*idx] == '(') {
            (*idx)++;
            a = execute_operation(a, recursive_parse_expr(expr, len, idx, 0, '+'), op);
        }
        else if(isdigit(expr[*idx]) != 0) {
            char num_str[128];
            memset(num_str, 0, sizeof(num_str));
            size_t num_ch_idx = 0;

            while(isdigit(expr[*idx]) != 0) {
                num_str[num_ch_idx++] = expr[(*idx)++];
            }

            int64_t b = atoll(num_str);

            a = execute_operation(a, b, op);
        }
        else {
            op = expr[(*idx)++];
        }
    }

    return a;
}

static int64_t evaluate_expression(const char* const expr, size_t len)
{
    size_t idx = 0;
    return recursive_parse_expr(expr, len, &idx, 0, '+');
}

static int64_t collapse_expression(const char* const expr, size_t len, size_t* idx)
{
    int64_t* nums = malloc(len * sizeof(int64_t));
    size_t n_nums = 0;
    char* ops = malloc(len);
    size_t n_ops = 0;

    while(*idx < len && expr[*idx] != ')') {
        if(isspace(expr[*idx]) != 0) {
            (*idx)++;
            continue;
        }

        if(expr[*idx] == '(') {
            (*idx)++;
            const int64_t num = collapse_expression(expr, len, idx);
            nums[n_nums++] = num;
        }
        else if(isdigit(expr[*idx]) != 0) {
            char num_str[128];
            memset(num_str, 0, sizeof(num_str));
            size_t num_ch_idx = 0;

            while(isdigit(expr[*idx]) != 0) {
                num_str[num_ch_idx++] = expr[(*idx)++];
            }

            int64_t num = atoll(num_str);
            nums[n_nums++] = num;
        }
        else {
            ops[n_ops++] = expr[(*idx)++];
        }
    }
    (*idx)++;

    for(size_t op_idx = 0; op_idx < n_ops; op_idx++) {
        if(ops[op_idx] == '+') {
            nums[op_idx + 1] = nums[op_idx] + nums[op_idx + 1];
            nums[op_idx] = 1;
        }
    }

    int64_t ans = 1;
    for(size_t num_idx = 0; num_idx < n_nums; num_idx++) {
        ans *= nums[num_idx];
    }

    free(nums);
    free(ops);

    return ans;
}

static int64_t solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    int64_t sum = 0;
    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const expr = &input[line_mem_refs[line_idx].offset];
        const size_t len = line_mem_refs[line_idx].size;
        const int64_t ans = evaluate_expression(expr, len);
        sum += ans;
    }

    free(line_mem_refs);

    return sum;
}

static int64_t solve_part_2(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    int64_t sum = 0;
    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const expr = &input[line_mem_refs[line_idx].offset];
        const size_t len = line_mem_refs[line_idx].size;
        size_t idx = 0;
        const int64_t ans = collapse_expression(expr, len, &idx);
        sum += ans;
    }

    free(line_mem_refs);

    return sum;
}

void day_18_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_18_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(26335, ans);
}

void day_18_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_18_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(131076645626, ans);
}

void day_18_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_18_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(693891, ans);
}

void day_18_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_18_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(109418509151782, ans);
}
