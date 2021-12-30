#include "day_07.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"
#include "unity_internals.h"

typedef struct {
    char* bag_type;
    size_t count;
} bag_count_t;

typedef struct {
    char* bag_type;
    bag_count_t* contained_bags;
    size_t n_contained_bags;
} bag_rule_t;

static void extract_rules(const char* const input, size_t size, bag_rule_t** output, size_t* output_len)
{
    memory_reference_t* line_mem_ref = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_ref, &n_lines);

    const size_t n_rules = n_lines;
    bag_rule_t* rules = (bag_rule_t*)malloc(n_rules * sizeof(bag_rule_t));

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const line = &input[line_mem_ref[line_idx].offset];
        const size_t line_size = line_mem_ref[line_idx].size;

        memory_reference_t* line_segment_mem_ref = NULL;
        size_t n_line_segments = 0;
        split_buf_by_sequence(line, line_size, " bags contain ", &line_segment_mem_ref, &n_line_segments);

        assert(n_line_segments == 2);

        const char* const container_bag = &line[line_segment_mem_ref[0].offset];
        const size_t container_bag_len = line_segment_mem_ref[0].size;

        char* container_bag_str = malloc(container_bag_len + 1);
        container_bag_str[container_bag_len] = 0;
        memcpy(container_bag_str, container_bag ,container_bag_len);
        rules[line_idx].bag_type = container_bag_str;

        const char* const contained_bags_list = &line[line_segment_mem_ref[1].offset];
        const size_t contained_bags_list_len = line_segment_mem_ref[1].size;

        memory_reference_t* contained_bags_mem_ref = NULL;
        size_t n_contained_bags = 0;
        split_buf_by_sequence(contained_bags_list, contained_bags_list_len, ", ", &contained_bags_mem_ref, &n_contained_bags);

        rules[line_idx].contained_bags = (bag_count_t*)malloc(n_contained_bags * sizeof(bag_count_t));
        rules[line_idx].n_contained_bags = n_contained_bags;

        for(size_t contained_bag_idx = 0; contained_bag_idx < n_contained_bags; contained_bag_idx++) {
            const char* const contained_bag = &contained_bags_list[contained_bags_mem_ref[contained_bag_idx].offset];
            const size_t contained_bag_len = contained_bags_mem_ref[contained_bag_idx].size;

            if(memcmp(contained_bag, "no other bags.", 14) == 0) {
                rules[line_idx].n_contained_bags = 0;
                continue;
            }

            memory_reference_t* contained_bag_portions_mem_ref = NULL;
            size_t n_contained_bag_portions = 0;
            split_buf_by_whitespace(contained_bag, contained_bag_len, &contained_bag_portions_mem_ref, &n_contained_bag_portions);

            char* num_str = malloc(contained_bag_portions_mem_ref[0].size + 1);
            num_str[contained_bag_portions_mem_ref[0].size] = 0;
            memcpy(num_str, &contained_bag[contained_bag_portions_mem_ref[0].offset], contained_bag_portions_mem_ref[0].size);

            char* bag_str = malloc(contained_bag_portions_mem_ref[1].size + contained_bag_portions_mem_ref[2].size + 2);
            bag_str[contained_bag_portions_mem_ref[1].size + contained_bag_portions_mem_ref[2].size + 1] = 0;
            memcpy(bag_str, &contained_bag[contained_bag_portions_mem_ref[1].offset], contained_bag_portions_mem_ref[1].size + contained_bag_portions_mem_ref[2].size + 1);

            rules[line_idx].contained_bags[contained_bag_idx].bag_type = bag_str;
            rules[line_idx].contained_bags[contained_bag_idx].count = atoi(num_str);

            free(num_str);
            free(contained_bag_portions_mem_ref);
        }

        free(contained_bags_mem_ref);
        free(line_segment_mem_ref);
    }

    free(line_mem_ref);

    *output = rules;
    *output_len = n_rules;
}

static void destroy_bag_rules(bag_rule_t* rules, size_t n_rules)
{
    for(size_t i = 0; i < n_rules; i++) {
        free(rules[i].bag_type);
        for(size_t j = 0; j < rules[i].n_contained_bags; j++) {
            free(rules[i].contained_bags[j].bag_type);
        }
        free(rules[i].contained_bags);
    }
    free(rules);
}

static void find_all_container_bags(bag_rule_t* rules, size_t n_rules, const char* const bag_type, size_t* container_bag_indices, size_t* n_containter_bag_indices)
{
    for(size_t i = 0; i < n_rules; i++) {
        bag_rule_t* rule = &rules[i];
        for(size_t j = 0; j < rule->n_contained_bags; j++) {
            bag_count_t* contained_bag = &rule->contained_bags[j];
            if(strcmp(bag_type, contained_bag->bag_type) == 0) {
                bool new_index = true;
                for(size_t k = 0; k < *n_containter_bag_indices; k++) {
                    if(container_bag_indices[k] == i) {
                        new_index = false;
                        break;
                    }
                }
                if(new_index) {
                    container_bag_indices[*n_containter_bag_indices] = i;
                    *n_containter_bag_indices += 1;

                    find_all_container_bags(rules, n_rules, rule->bag_type, container_bag_indices, n_containter_bag_indices);
                }
            }
        }
    }
}

static size_t solve_part_1(const char* const input, size_t size)
{
    bag_rule_t* rules = NULL;
    size_t n_rules = 0;
    extract_rules(input, size, &rules, &n_rules);

    size_t* container_bag_indices = (size_t*)malloc(n_rules * sizeof(size_t));
    size_t n_container_bag_indices = 0;
    find_all_container_bags(rules, n_rules, "shiny gold", container_bag_indices, &n_container_bag_indices);

    free(container_bag_indices);

    destroy_bag_rules(rules, n_rules);

    return n_container_bag_indices;
}

static size_t recursive_contained_bag_count(bag_rule_t* rules, size_t n_rules, const char* const bag_type)
{
    size_t count = 1;
    for(size_t i = 0; i < n_rules; i++) {
        bag_rule_t* rule = &rules[i];
        if(strcmp(bag_type, rule->bag_type) == 0) {
            for(size_t j = 0; j < rule->n_contained_bags; j++) {
                bag_count_t* contained_bag = &rule->contained_bags[j];
                count += contained_bag->count * recursive_contained_bag_count(rules, n_rules, contained_bag->bag_type);
            }
        }
    }
    return count;
}

static size_t solve_part_2(const char* const input, size_t size)
{
    bag_rule_t* rules = NULL;
    size_t n_rules = 0;
    extract_rules(input, size, &rules, &n_rules);

    const size_t total_contained_bag_count = recursive_contained_bag_count(rules, n_rules, "shiny gold") - 1;

    destroy_bag_rules(rules, n_rules);

    return total_contained_bag_count;
}

void day_07_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_07_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(4, ans);
}

void day_07_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_07_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(192, ans);
}

void day_07_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_07_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(32, ans);
}

void day_07_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_07_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(12128, ans);
}
