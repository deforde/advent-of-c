#include "day_19.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

typedef struct {
    size_t* data;
    size_t len;
} vec_t;

typedef struct {
    vec_t* data;
    size_t len;
} super_vec_t;

typedef struct {
    super_vec_t children;
    int32_t val;
} node_t;

static bool recursive_solve(const node_t* const nodes, const char* const buf, size_t buf_size, size_t* pos, size_t node_idx)
{
    assert(*pos < buf_size);
    const node_t* const node = &nodes[node_idx];
    if(node->val != -1) {
        const bool match_found = node->val == buf[*pos];
        if(match_found) {
            (*pos)++;
        }
        return match_found;
    }
    for(size_t sup_idx = 0; sup_idx < node->children.len; ++sup_idx) {
        const vec_t* const child_set = &node->children.data[sup_idx];
        bool match_found = true;
        size_t temp_pos = *pos;
        for(size_t sub_idx = 0; sub_idx < child_set->len; ++sub_idx) {
            match_found &= recursive_solve(nodes, buf, buf_size, &temp_pos, child_set->data[sub_idx]);
            if(!match_found) {
                break;
            }
            if(temp_pos == buf_size && sub_idx < (child_set->len - 1)) {
                match_found = false;
                break;
            }
        }
        if(match_found) {
            *pos = temp_pos;
            return true;
        }
    }
    return false;
}

static void free_nodes(node_t* nodes, size_t n_nodes)
{
    for(size_t node_idx = 0; node_idx < n_nodes; ++node_idx) {
        node_t* node = &nodes[node_idx];
        for(size_t sup_idx = 0; sup_idx < node->children.len; ++sup_idx) {
            free(node->children.data[sup_idx].data);
        }
        free(node->children.data);
    }
    free(nodes);
}

static void clone_nodes(node_t* input_nodes, size_t n_input_nodes, node_t** output_nodes, size_t* n_output_nodes)
{
    *output_nodes = (node_t*)malloc(n_input_nodes * sizeof(node_t));
    *n_output_nodes = n_input_nodes;

    for(size_t node_idx = 0; node_idx < n_input_nodes; ++node_idx) {
        node_t* node = &input_nodes[node_idx];

        (*output_nodes)[node_idx].val = node->val;
        (*output_nodes)[node_idx].children.len = node->children.len;
        (*output_nodes)[node_idx].children.data = (vec_t*)malloc(node->children.len * sizeof(vec_t));

        for(size_t sup_idx = 0; sup_idx < node->children.len; ++sup_idx) {
            (*output_nodes)[node_idx].children.data[sup_idx].len = node->children.data[sup_idx].len;
            (*output_nodes)[node_idx].children.data[sup_idx].data = (size_t*)malloc(node->children.data[sup_idx].len * sizeof(size_t));
            memcpy((*output_nodes)[node_idx].children.data[sup_idx].data, node->children.data[sup_idx].data, node->children.data[sup_idx].len * sizeof(size_t));
        }
    }
}

static bool process_rules(const char* const input, size_t size, node_t** output_nodes, size_t* n_output_nodes)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    size_t largest_node_idx = 0;
    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const rule = &input[line_mem_refs[line_idx].offset];

        const char* rule_content = strchr(rule, ':');
        if(!rule_content) {
            continue;
        }
        char temp[128] = {0};
        memcpy(temp, rule, rule_content - rule);
        const size_t node_idx = atoi(temp);
        largest_node_idx = node_idx > largest_node_idx ? node_idx : largest_node_idx;
    }
    assert(largest_node_idx >= (n_lines - 1));

    const size_t n_nodes = (largest_node_idx + 1);
    node_t* nodes = (node_t*)malloc(n_nodes * sizeof(node_t));
    memset(nodes, 0, n_nodes * sizeof(node_t));

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const rule = &input[line_mem_refs[line_idx].offset];
        const size_t len = line_mem_refs[line_idx].size;

        const char* rule_content = strchr(rule, ':');
        if(!rule_content) {
            continue;
        }
        char temp[128] = {0};
        memcpy(temp, rule, rule_content - rule);
        const size_t node_idx = atoi(temp);
        rule_content += 2;

        assert(node_idx < n_nodes);
        node_t* node = &nodes[node_idx];
        node->children.data = NULL;
        node->children.len = 0;
        node->val = -1;

        memory_reference_t* rule_portion_mem_refs = NULL;
        size_t n_rule_portions = 0;
        split_buf_by_sequence(rule_content, len - (rule_content - rule), "|", &rule_portion_mem_refs, &n_rule_portions);
        assert(n_rule_portions > 0);

        if(n_rule_portions == 1 && rule_content[0] == '"') {
            node->val = (int32_t)rule_content[1];
        }
        else {
            node->children.data = (vec_t*)malloc(n_rule_portions * sizeof(vec_t));
            node->children.len = n_rule_portions;

            for(size_t rule_portion_idx = 0; rule_portion_idx < n_rule_portions; ++rule_portion_idx) {
                const char* rule_portion = &rule_content[rule_portion_mem_refs[rule_portion_idx].offset];
                size_t rule_portion_len = rule_portion_mem_refs[rule_portion_idx].size;
                if(rule_portion[0] == ' ') {
                    ++rule_portion;
                    --rule_portion_len;
                }

                memory_reference_t* node_indices_mem_refs = NULL;
                size_t n_node_indices = 0;
                split_buf_by_sequence(rule_portion, rule_portion_len, " ", &node_indices_mem_refs, &n_node_indices);
                node->children.data[rule_portion_idx].data = (size_t*)malloc(n_node_indices * sizeof(size_t));
                node->children.data[rule_portion_idx].len = n_node_indices;

                for(size_t node_index_idx = 0; node_index_idx < n_node_indices; ++node_index_idx) {
                    const char* const node_index_string = &rule_portion[node_indices_mem_refs[node_index_idx].offset];
                    const size_t node_index_string_len = node_indices_mem_refs[node_index_idx].size;

                    char temp[128] = {0};
                    memcpy(temp, node_index_string, node_index_string_len);

                    node->children.data[rule_portion_idx].data[node_index_idx] = atoi(temp);
                }

                free(node_indices_mem_refs);
            }
        }

        free(rule_portion_mem_refs);
    }

    free(line_mem_refs);

    *output_nodes = nodes;
    *n_output_nodes = n_nodes;

    return true;
}

static size_t process_test_block(const char* const input, size_t size, node_t* nodes)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    size_t valid_test_string_count = 0;

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const test_string = &input[line_mem_refs[line_idx].offset];
        const size_t test_string_len = line_mem_refs[line_idx].size;

        char temp[256] = {0};
        memcpy(temp, test_string, test_string_len);

        size_t pos = 0;
        if(recursive_solve(nodes, temp, test_string_len, &pos, 0) && pos == test_string_len) {
            ++valid_test_string_count;
        }
    }

    free(line_mem_refs);

    return valid_test_string_count;
}

static size_t process_test_block_2(const char* const input, size_t size, node_t* nodes, size_t n_nodes)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    size_t valid_test_string_count = 0;

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const test_string = &input[line_mem_refs[line_idx].offset];
        const size_t test_string_len = line_mem_refs[line_idx].size;

        char temp[256] = {0};
        memcpy(temp, test_string, test_string_len);

        node_t* local_nodes = NULL;
        size_t n_local_nodes = 0;
        clone_nodes(nodes, n_nodes, &local_nodes, &n_local_nodes);

        for(size_t add_rule_8 = 0; add_rule_8 < 10; ++add_rule_8) {
            for(size_t add_rule_11 = 0; add_rule_11 < 10; ++add_rule_11) {
                free(local_nodes[0].children.data[0].data);
                local_nodes[0].children.data[0].len = 3 + add_rule_8 + add_rule_11 * 2;
                local_nodes[0].children.data[0].data = (size_t*)malloc(local_nodes[0].children.data[0].len * sizeof(size_t));
                local_nodes[0].children.data[0].data[0] = 42;
                local_nodes[0].children.data[0].data[add_rule_8 + 1] = 42;
                local_nodes[0].children.data[0].data[local_nodes[0].children.data[0].len - 1] = 31;

                for(size_t i = 0; i < add_rule_8; ++i) {
                    local_nodes[0].children.data[0].data[1 + i] = 42;
                }
                for(size_t i = 0; i < add_rule_11; ++i) {
                    local_nodes[0].children.data[0].data[add_rule_8 + 2 + i] = 42;
                    local_nodes[0].children.data[0].data[local_nodes[0].children.data[0].len - 2 - i] = 31;
                }

                // printf("Trying rule 0: [ ");
                // for(size_t i = 0; i < local_nodes[0].children.data[0].len; ++i) {
                //     printf("%lu ", local_nodes[0].children.data[0].data[i]);
                // }
                // printf("]\n");
                size_t pos = 0;
                if(recursive_solve(local_nodes, temp, test_string_len, &pos, 0) && pos == test_string_len) {
                    ++valid_test_string_count;
                    goto search_complete;
                }
            }
        }

search_complete:
        free_nodes(local_nodes, n_local_nodes);
    }

    free(line_mem_refs);

    return valid_test_string_count;
}

static size_t solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;
    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);

    const char* const rule_block = &input[block_mem_refs[0].offset];
    const size_t rule_block_len = block_mem_refs[0].size;

    const char* const test_block = &input[block_mem_refs[1].offset];
    const size_t test_block_len = block_mem_refs[1].size;

    node_t* nodes = NULL;
    size_t n_nodes = 0;
    process_rules(rule_block, rule_block_len, &nodes, &n_nodes);

    const size_t valid_test_string_count = process_test_block(test_block, test_block_len, nodes);

    free_nodes(nodes, n_nodes);
    free(block_mem_refs);

    return valid_test_string_count;
}

static size_t solve_part_2(const char* const input, size_t size)
{
    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;
    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);

    const char* const rule_block = &input[block_mem_refs[0].offset];
    const size_t rule_block_len = block_mem_refs[0].size;

    const char* const test_block = &input[block_mem_refs[1].offset];
    const size_t test_block_len = block_mem_refs[1].size;

    node_t* nodes = NULL;
    size_t n_nodes = 0;
    process_rules(rule_block, rule_block_len, &nodes, &n_nodes);

    nodes[0].children.data[0].len = 3;
    free(nodes[0].children.data[0].data);
    nodes[0].children.data[0].data = (size_t*)malloc(nodes[0].children.data[0].len * sizeof(size_t));
    nodes[0].children.data[0].data[0] = 42;
    nodes[0].children.data[0].data[1] = 42;
    nodes[0].children.data[0].data[2] = 31;

    const size_t valid_test_string_count = process_test_block_2(test_block, test_block_len, nodes, n_nodes);

    free_nodes(nodes, n_nodes);
    free(block_mem_refs);

    return valid_test_string_count;
}

void day_19_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_19_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(2, ans);
}

void day_19_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_19_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(272, ans);
}

void day_19_part_2_example_1()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_19_part_2_example_1.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(3, ans);
}

void day_19_part_2_example_2()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_19_part_2_example_2.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(12, ans);
}

void day_19_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_19_part_2_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(272, ans);
}

