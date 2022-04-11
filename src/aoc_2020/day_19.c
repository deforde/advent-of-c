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

static bool recursive_solve(const node_t* const nodes, const char* const buf, size_t buf_size, size_t pos, size_t node_idx)
{
    const node_t* const node = &nodes[node_idx];
    if(node->val != -1) {
        return node->val == buf[pos];
    }
    for(size_t sup_idx = 0; sup_idx < node->children.len; ++sup_idx) {
        const vec_t* const child_set = &node->children.data[sup_idx];
        bool match_found = true;
        size_t temp_pos = pos;
        for(size_t sub_idx = 0; sub_idx < child_set->len; ++sub_idx) {
            match_found &= recursive_solve(nodes, buf, buf_size, temp_pos, child_set->data[sub_idx]);
            if(!match_found) {
                break;
            }
            ++temp_pos;
        }
        if(match_found) {
            return true;
        }
    }
    return false;
}

static void solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    const size_t n_nodes = n_lines;
    node_t* nodes = (node_t*)malloc(n_nodes * sizeof(node_t));
    memset(nodes, 0, n_nodes * sizeof(node_t));

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const rule = &input[line_mem_refs[line_idx].offset];
        const size_t len = line_mem_refs[line_idx].size;

        node_t* node = &nodes[line_idx];
        node->children.data = NULL;
        node->children.len = 0;
        node->val = -1;

        const char* rule_content = strchr(rule, ':');
        if(!rule_content) {
            continue;
        }
        rule_content += 2;

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
                    char temp[2] = {0};
                    temp[0] = rule_portion[node_indices_mem_refs[node_index_idx].offset];
                    node->children.data[rule_portion_idx].data[node_index_idx] = atoi(temp);
                }
            }
        }
    }

    free(line_mem_refs);

    {
        const char* const test_string = "aba";
        const bool string_valid = recursive_solve(nodes, test_string, strlen(test_string), 0, 0);
        printf("String: \"%s\" valid? %s\n", test_string, string_valid ? "True" : "False");
    }
    {
        const char* const test_string = "aab";
        const bool string_valid = recursive_solve(nodes, test_string, strlen(test_string), 0, 0);
        printf("String: \"%s\" valid? %s\n", test_string, string_valid ? "True" : "False");
    }
    {
        const char* const test_string = "abb";
        const bool string_valid = recursive_solve(nodes, test_string, strlen(test_string), 0, 0);
        printf("String: \"%s\" valid? %s\n", test_string, string_valid ? "True" : "False");
    }

    for(size_t node_idx = 0; node_idx < n_nodes; ++node_idx) {
        node_t* node = &nodes[node_idx];
        for(size_t sup_idx = 0; sup_idx < node->children.len; ++sup_idx) {
            free(node->children.data[sup_idx].data);
        }
        free(node->children.data);
    }
    free(nodes);
}

void day_19_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_19_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    // const int64_t ans = solve_part_1(input, size);
    solve_part_1(input, size);

    free(input);

    // TEST_ASSERT_EQUAL_UINT64(26335, ans);
}

