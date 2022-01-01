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

typedef struct {
    size_t* possible_category_indices;
    size_t n_possible_category_indices;
} pos_category_indices_t;

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

static void extract_data(const char* const input, size_t size, ticket_t* my_ticket, ticket_t** other_tickets, size_t* n_other_tickets, category_t** categories, size_t* n_categories)
{
    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;
    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);

    const char* const block_0_buf = &input[block_mem_refs[0].offset];
    const size_t block_0_len = block_mem_refs[0].size;
    *categories = NULL;
    *n_categories = 0;
    extract_category_ranges(block_0_buf, block_0_len, categories, n_categories);

    const char* const block_1_buf = &input[block_mem_refs[1].offset];
    const size_t block_1_len = block_mem_refs[1].size;
    extract_my_ticket(block_1_buf, block_1_len, my_ticket);

    const char* const block_2_buf = &input[block_mem_refs[2].offset];
    const size_t block_2_len = block_mem_refs[2].size;
    *other_tickets = NULL;
    *n_other_tickets = 0;
    extract_other_tickets(block_2_buf, block_2_len, other_tickets, n_other_tickets);

    free(block_mem_refs);
}

static void remove_invalid_tickets(ticket_t** tickets, size_t* n_tickets, const category_t* const categories, size_t n_categories)
{
    ticket_t* other_tickets = *tickets;
    size_t n_other_tickets = *n_tickets;

    for(size_t ticket_idx = 0; ticket_idx < n_other_tickets;) {
        ticket_t* ticket = &other_tickets[ticket_idx];
        bool is_ticket_valid = true;

        for(size_t value_idx = 0; value_idx < ticket->n_values; value_idx++) {
            const uint64_t value = ticket->values[value_idx];
            bool is_value_valid = false;

            for(size_t category_idx = 0; category_idx < n_categories; category_idx++) {
                const category_t* const category = &categories[category_idx];

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
                is_ticket_valid = false;
                break;
            }
        }

        if(!is_ticket_valid) {
            free(other_tickets[ticket_idx].values);
            other_tickets[ticket_idx].n_values = other_tickets[n_other_tickets - 1].n_values;
            other_tickets[ticket_idx].values = other_tickets[n_other_tickets - 1].values;
            n_other_tickets--;
        }
        else {
            ticket_idx++;
        }
    }

    *n_tickets = n_other_tickets;
}

static int64_t solve_part_1(const char* const input, size_t size)
{
    category_t* categories = NULL;
    size_t n_categories = 0;
    ticket_t my_ticket;
    ticket_t* other_tickets = NULL;
    size_t n_other_tickets = 0;

    extract_data(input, size, &my_ticket, &other_tickets, &n_other_tickets, &categories, &n_categories);

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

    return sum_invalid_values;
}

static void eliminate_possible_categories(uint64_t value, size_t pos, pos_category_indices_t* possible_categories_per_pos, const category_t* categories, size_t n_categories)
{
    for(size_t category_idx = 0; category_idx < n_categories; category_idx++) {
        const category_t* category = &categories[category_idx];
        bool valid_range_found = false;

        for(size_t range_idx = 0; range_idx < category->n_ranges; range_idx++) {
            range_t* range = &category->ranges[range_idx];
            if(value >= range->min && value <= range->max) {
                valid_range_found = true;
                break;
            }
        }

        if(!valid_range_found) {
            pos_category_indices_t* pos_categories = &possible_categories_per_pos[pos];
            for(size_t pos_category_idx = 0; pos_category_idx < pos_categories->n_possible_category_indices; pos_category_idx++) {
                if(pos_categories->possible_category_indices[pos_category_idx] == category_idx) {
                    pos_categories->possible_category_indices[pos_category_idx] = pos_categories->possible_category_indices[pos_categories->n_possible_category_indices - 1];
                    pos_categories->n_possible_category_indices--;
                    break;
                }
            }
        }
    }
}

static bool are_category_ambiguities_eliminated(pos_category_indices_t* possible_categories_per_pos, size_t n_positions)
{
    for(size_t i = 0; i < n_positions; i++) {
        if(possible_categories_per_pos[i].n_possible_category_indices > 1) {
            return false;
        }
    }
    return true;
}

static void disambiguate_possible_categories(pos_category_indices_t* possible_categories_per_pos, size_t n_positions)
{
    while(!are_category_ambiguities_eliminated(possible_categories_per_pos, n_positions)) {
        for(size_t i = 0; i < n_positions; i++) {
            pos_category_indices_t* this_pos_categories = &possible_categories_per_pos[i];
            if(this_pos_categories->n_possible_category_indices == 1) {
                for(size_t j = 0; j < n_positions; j++) {
                    if(i != j && possible_categories_per_pos[j].n_possible_category_indices > 1) {
                        pos_category_indices_t* other_pos_categories = &possible_categories_per_pos[j];
                        for(size_t k = 0; k < other_pos_categories->n_possible_category_indices; k++) {
                            if(other_pos_categories->possible_category_indices[k] == this_pos_categories->possible_category_indices[0]) {
                                other_pos_categories->possible_category_indices[k] = other_pos_categories->possible_category_indices[other_pos_categories->n_possible_category_indices - 1];
                                other_pos_categories->n_possible_category_indices--;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

static int64_t solve_part_2(const char* const input, size_t size)
{
    category_t* categories = NULL;
    size_t n_categories = 0;
    ticket_t my_ticket;
    ticket_t* other_tickets = NULL;
    size_t n_other_tickets = 0;

    extract_data(input, size, &my_ticket, &other_tickets, &n_other_tickets, &categories, &n_categories);

    remove_invalid_tickets(&other_tickets, &n_other_tickets, categories, n_categories);

    pos_category_indices_t* possible_categories_per_pos = (pos_category_indices_t*)malloc(my_ticket.n_values * sizeof(pos_category_indices_t));
    for(size_t i = 0; i < my_ticket.n_values; i++) {
        possible_categories_per_pos[i].n_possible_category_indices = n_categories;
        possible_categories_per_pos[i].possible_category_indices = (size_t*)malloc(n_categories * sizeof(size_t));
        for(size_t j = 0; j < n_categories; j++) {
            possible_categories_per_pos[i].possible_category_indices[j] = j;
        }
    }

    for(size_t pos = 0; pos < my_ticket.n_values; pos++) {
        const uint64_t my_ticket_val = my_ticket.values[pos];
        eliminate_possible_categories(my_ticket_val, pos, possible_categories_per_pos, categories, n_categories);

        for(size_t other_ticket_idx = 0; other_ticket_idx < n_other_tickets; other_ticket_idx++) {
            const uint64_t other_ticket_value = other_tickets[other_ticket_idx].values[pos];
            eliminate_possible_categories(other_ticket_value, pos, possible_categories_per_pos, categories, n_categories);
        }
    }

    disambiguate_possible_categories(possible_categories_per_pos, my_ticket.n_values);

    size_t* category_indices_of_interest = (size_t*)malloc(n_categories * sizeof(size_t));
    size_t n_category_indices_of_interest = 0;
    for(size_t i = 0; i < n_categories; i++) {
        if(strlen(categories[i].name) >= 9 && memcmp(categories[i].name, "departure", 9) == 0) {
            category_indices_of_interest[n_category_indices_of_interest++] = i;
        }
    }
    assert(n_category_indices_of_interest == 6);

    uint64_t product = 1;
    for(size_t i = 0; i < n_category_indices_of_interest; i++) {
        for(size_t j = 0; j < my_ticket.n_values; j++) {
            if(possible_categories_per_pos[j].possible_category_indices[0] == category_indices_of_interest[i]) {
                product *= my_ticket.values[j];
                break;
            }
        }
    }

    free(category_indices_of_interest);
    for(size_t i = 0; i < my_ticket.n_values; i++) {
        free(possible_categories_per_pos[i].possible_category_indices);
    }
    free(possible_categories_per_pos);
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

    return product;
}

void day_16_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_16_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(71, ans);
}

void day_16_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_16_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(20058, ans);
}

void day_16_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_16_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(366871907221, ans);
}
