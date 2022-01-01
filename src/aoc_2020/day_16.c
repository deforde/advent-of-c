#include "day_16.h"

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
    uint64_t min;
    uint64_t max;
} range_t;

typedef struct {
    char* name;
    range_t* ranges;
    size_t n_ranges;
} category_t;

typedef struct {
    uint64_t* values;
    size_t n_values;
} ticket_t;

static void extract_category_ranges(const char* const category_ranges_buf, size_t len, category_t** output_categories, size_t* output_n_categories)
{
    memory_reference_t* category_mem_refs = NULL;
    size_t n_categories = 0;
    split_buf_by_sequence(category_ranges_buf, len, "\n", &category_mem_refs, &n_categories);

    category_t* categories = (category_t*)malloc(n_categories * sizeof(category_t));

    for(size_t i = 0; i < n_categories; i++) {
        const char* const category_buf = &category_ranges_buf[category_mem_refs[i].offset];
        const size_t category_len = category_mem_refs[i].size;

        memory_reference_t* category_range_mem_refs = NULL;
        size_t n_categoriy_ranges = 0;
        split_buf_by_sequence(category_buf, category_len, ": ", &category_range_mem_refs, &n_categoriy_ranges);

        char* category_name = malloc(category_range_mem_refs[0].size + 1);
        category_name[category_range_mem_refs[0].size] = 0;
        memcpy(category_name, category_buf, category_range_mem_refs[0].size);

        const char* const ranges_buf = &category_buf[category_range_mem_refs[1].offset];
        const size_t ranges_len = category_range_mem_refs[1].size;

        memory_reference_t* range_mem_refs = NULL;
        size_t n_ranges = 0;
        split_buf_by_sequence(ranges_buf, ranges_len, " or ", &range_mem_refs, &n_ranges);

        range_t* ranges = malloc(n_ranges * sizeof(range_t));
        for(size_t j = 0; j < n_ranges; j++) {
            const char* const range_buf = &ranges_buf[range_mem_refs[j].offset];
            const size_t range_len = range_mem_refs[j].size;

            memory_reference_t* value_mem_refs = NULL;
            size_t n_values = 0;
            split_buf_by_sequence(range_buf, range_len, "-", &value_mem_refs, &n_values);

            assert(n_values == 2);

            char* min_val_str = malloc(value_mem_refs[0].size + 1);
            min_val_str[value_mem_refs[0].size] = 0;
            memcpy(min_val_str, &range_buf[value_mem_refs[0].offset], value_mem_refs[0].size);
            const uint64_t min_val = atoll(min_val_str);
            free(min_val_str);

            char* max_val_str = malloc(value_mem_refs[1].size + 1);
            max_val_str[value_mem_refs[1].size] = 0;
            memcpy(max_val_str, &range_buf[value_mem_refs[1].offset], value_mem_refs[1].size);
            const uint64_t max_val = atoll(max_val_str);
            free(max_val_str);

            free(value_mem_refs);

            ranges[j].min = min_val;
            ranges[j].max = max_val;
        }

        free(range_mem_refs);
        free(category_range_mem_refs);

        categories[i].name = category_name;
        categories[i].ranges = ranges;
        categories[i].n_ranges = n_ranges;
    }

    free(category_mem_refs);

    *output_categories = categories;
    *output_n_categories = n_categories;
}

static void extract_ticket(const char* const ticket_buf, size_t len, ticket_t* ticket)
{
    memory_reference_t* value_mem_refs = NULL;
    size_t n_values = 0;
    split_buf_by_sequence(ticket_buf, len, ",", &value_mem_refs, &n_values);

    uint64_t* values = malloc(n_values * sizeof(uint64_t));

    for(size_t i = 0; i < n_values; i++) {
        char* val_str = malloc(value_mem_refs[i].size + 1);
        val_str[value_mem_refs[i].size] = 0;
        memcpy(val_str, &ticket_buf[value_mem_refs[i].offset], value_mem_refs[i].size);
        const uint64_t val = atoll(val_str);
        free(val_str);

        values[i] = val;
    }

    ticket->values = values;
    ticket->n_values = n_values;

    free(value_mem_refs);
}

static void extract_my_ticket(const char* const block, size_t block_size, ticket_t* ticket)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(block, block_size, "\n", &line_mem_refs, &n_lines);
    assert(n_lines == 2);

    const char* const ticket_buf = &block[line_mem_refs[1].offset];
    const size_t ticket_len = line_mem_refs[1].size;

    extract_ticket(ticket_buf, ticket_len, ticket);

    free(line_mem_refs);
}

static void extract_other_tickets(const char* const block, size_t block_size, ticket_t** output_tickets, size_t* output_n_tickets)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(block, block_size, "\n", &line_mem_refs, &n_lines);

    const size_t n_tickets = n_lines - 1;
    ticket_t* tickets = (ticket_t*)malloc(n_tickets * sizeof(ticket_t));

    for(size_t line_idx = 1; line_idx < n_lines; line_idx++) {
        const char* const ticket_buf = &block[line_mem_refs[line_idx].offset];
        const size_t ticket_len = line_mem_refs[line_idx].size;
        extract_ticket(ticket_buf, ticket_len, &tickets[line_idx - 1]);
    }

    *output_tickets = tickets;
    *output_n_tickets = n_tickets;

    free(line_mem_refs);
}

static int64_t solve(const char* const input, size_t size)
{
    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;
    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);

    const char* const block_0_buf = &input[block_mem_refs[0].offset];
    const size_t block_0_len = block_mem_refs[0].size;
    category_t* categories = NULL;
    size_t n_categories = 0;
    extract_category_ranges(block_0_buf, block_0_len, &categories, &n_categories);

    const char* const block_1_buf = &input[block_mem_refs[1].offset];
    const size_t block_1_len = block_mem_refs[1].size;
    ticket_t my_ticket;
    extract_my_ticket(block_1_buf, block_1_len, &my_ticket);

    const char* const block_2_buf = &input[block_mem_refs[2].offset];
    const size_t block_2_len = block_mem_refs[2].size;
    ticket_t* other_tickets = NULL;
    size_t n_other_tickets = 0;
    extract_other_tickets(block_2_buf, block_2_len, &other_tickets, &n_other_tickets);

    uint64_t sum_invalid_values = 0;
    for(size_t ticket_idx = 0; ticket_idx < n_other_tickets; ticket_idx++) {
        ticket_t* ticket = &other_tickets[ticket_idx];

        for(size_t value_idx = 0; value_idx < ticket->n_values; value_idx++) {
            const uint64_t value = ticket->values[value_idx];
            bool is_value_valid = false;

            for(size_t category_idx = 0; category_idx < n_categories; category_idx++) {
                category_t* category = &categories[category_idx];

                for(size_t range_idx = 0; range_idx < category->n_ranges; range_idx++) {
                    const uint64_t min_val = category->ranges[range_idx].min;
                    const uint64_t max_val = category->ranges[range_idx].max;
                    if(value >= min_val && value <= max_val) {
                        is_value_valid = true;
                        break;
                    }
                }

                if(is_value_valid) {
                    break;
                }
            }

            if(!is_value_valid) {
                sum_invalid_values += value;
            }
        }
    }

    free(my_ticket.values);
    for(size_t i = 0; i < n_other_tickets; i++) {
        free(other_tickets[i].values);
    }
    free(other_tickets);
    for(size_t i = 0; i < n_categories; i++) {
        free(categories[i].name);
        free(categories[i].ranges);
    }
    free(categories);
    free(block_mem_refs);

    return sum_invalid_values;
}

void day_16_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_16_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(71, ans);
}

void day_16_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_16_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(20058, ans);
}

void day_16_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_16_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(175594, ans);
}

void day_16_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_16_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(8546398, ans);
}
