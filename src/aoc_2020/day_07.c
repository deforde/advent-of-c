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

static size_t solve_part_1(const char* const input, size_t size)
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
        //printf("container_bag_str = %s\n", container_bag_str);
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
            //printf("num_str = %s\n", num_str);

            char* bag_str = malloc(contained_bag_portions_mem_ref[1].size + contained_bag_portions_mem_ref[2].size + 2);
            bag_str[contained_bag_portions_mem_ref[1].size + contained_bag_portions_mem_ref[2].size + 1] = 0;
            memcpy(bag_str, &contained_bag[contained_bag_portions_mem_ref[1].offset], contained_bag_portions_mem_ref[1].size + contained_bag_portions_mem_ref[2].size + 1);
            //printf("bag_str = %s\n", bag_str);

            rules[line_idx].contained_bags[contained_bag_idx].bag_type = bag_str;
            rules[line_idx].contained_bags[contained_bag_idx].count = atoi(num_str);

            //free(bag_str);
            free(num_str);
            free(contained_bag_portions_mem_ref);
        }

        //free(container_bag_str);

        free(contained_bags_mem_ref);
        free(line_segment_mem_ref);
    }

    free(line_mem_ref);

    char* initial_bag = "shiny gold";
    size_t initial_bag_index = SIZE_MAX;
    for(size_t i = 0; i < n_rules; i++) {
        bag_rule_t* rule = &rules[i];
        if(strcmp(rule->bag_type, initial_bag) == 0) {
            initial_bag_index = i;
            break;
        }
    }
    assert(initial_bag_index != SIZE_MAX);

    size_t* search_rules = (size_t*)malloc(n_rules * sizeof(size_t));
    search_rules[0] = initial_bag_index;
    size_t n_search_rules = 1;

    size_t* searched_rules = (size_t*)malloc(n_rules * sizeof(size_t));
    searched_rules[0] = initial_bag_index;
    size_t n_searched_rules = 1;

    size_t container_bag_count = 0;
    while(n_search_rules > 0) {
        size_t* temp_search_rules = (size_t*)malloc(n_rules * sizeof(size_t));
        size_t n_temp_search_rules = 0;

        for(size_t k = 0; k < n_search_rules; k++) {
            bag_rule_t* search_rule = &rules[search_rules[k]];

            for(size_t i = 0; i < n_rules; i++) {
                bag_rule_t* rule = &rules[i];
                for(size_t j = 0; j < rule->n_contained_bags; j++) {
                    bag_count_t* contained_bag = &rule->contained_bags[j];
                    if(strcmp(search_rule->bag_type, contained_bag->bag_type) == 0) {
                        bool new_index = true;
                        for(size_t h = 0; h < n_searched_rules; h++) {
                            if(searched_rules[h] == i) {
                                new_index = false;
                                break;
                            }
                        }
                        if(new_index) {
                            searched_rules[n_searched_rules++] = i;
                            //printf("search_rule = %s, contained_bag = %s, container_bag = %s\n", search_rule->bag_type, contained_bag->bag_type, rule->bag_type);
                            temp_search_rules[n_temp_search_rules++] = i;
                            container_bag_count++;
                        }
                        break;
                    }
                }
            }
        }

        free(search_rules);
        search_rules = temp_search_rules;
        n_search_rules = n_temp_search_rules;
    }

    free(searched_rules);
    free(search_rules);

    for(size_t i = 0; i < n_rules; i++) {
        free(rules[i].bag_type);
        for(size_t j = 0; j < rules[i].n_contained_bags; j++) {
            free(rules[i].contained_bags[j].bag_type);
        }
        free(rules[i].contained_bags);
    }
    free(rules);

    return container_bag_count;
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

}

void day_07_part_2_problem()
{

}
