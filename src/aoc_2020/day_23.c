#include "day_23.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

#define CUP_SET_LEN 10

static void process_input(const char* const input, size_t size, size_t cups[CUP_SET_LEN], size_t* n_cups)
{
    for(size_t i = 0; i < size; ++i) {
        if(isspace(input[i]) != 0) {
            break;
        }
        char temp[2] = {0};
        temp[0] = input[i];
        cups[(*n_cups)++] = atoi(temp);
    }
}

static size_t solve_part_1(const char* const input, size_t size, size_t n_moves)
{
    size_t cups[CUP_SET_LEN] = {0};
    size_t n_cups = 0;
    process_input(input, size, cups, &n_cups);
    const size_t total_num_cups = n_cups;

    size_t current_cup_idx = 0;
    const size_t n_cups_selected_per_move = 3;
    for(size_t i = 0; i < n_moves; ++i) {
        // printf("Move #%lu\n", i + 1);
        // printf("cups: ");
        // for(size_t j = 0; j < n_cups; ++j) {
        //     printf("%lu ", cups[j]);
        // }
        // printf("\n");

        size_t selected_cups[CUP_SET_LEN] = {0};
        size_t n_selected_cups = 0;

        size_t selected_cup_idx = (current_cup_idx + 1) % n_cups;
        for(size_t j = 0; j < n_cups_selected_per_move; ++j) {
            selected_cups[n_selected_cups++] = cups[selected_cup_idx];
            if(selected_cup_idx != n_cups - 1) {
                memmove(&cups[selected_cup_idx],
                        &cups[selected_cup_idx + 1],
                        (n_cups - selected_cup_idx - 1) * sizeof(size_t));
                if(selected_cup_idx < current_cup_idx) {
                    current_cup_idx--;
                }
            }
            n_cups--;
            selected_cup_idx %= n_cups;
        }

        const size_t next_cup = cups[(current_cup_idx + 1) % n_cups];

        size_t destination_cup = cups[current_cup_idx];
        bool destination_cup_in_selection = false;
        do {
            destination_cup--;
            if(destination_cup == 0) {
                destination_cup = 9;
            }
            destination_cup_in_selection = false;
            for(size_t j = 0; j < n_selected_cups; ++j) {
                if(destination_cup == selected_cups[j]) {
                    destination_cup_in_selection = true;
                    break;
                }
            }
        } while(destination_cup_in_selection);

        size_t insertion_idx = 0;
        for(size_t j = 0; j < n_cups; ++j) {
            if(cups[j] == destination_cup) {
                insertion_idx = (j + 1) % total_num_cups;
                break;
            }
        }
        if(insertion_idx != (total_num_cups - n_selected_cups)) {
            memmove(&cups[insertion_idx + n_selected_cups], &cups[insertion_idx], (n_cups - insertion_idx) * sizeof(size_t));
        }
        memcpy(&cups[insertion_idx], selected_cups, n_selected_cups * sizeof(size_t));
        n_cups += n_selected_cups;

        for(size_t j = 0; j < n_cups; ++j) {
            if(cups[j] == next_cup) {
                current_cup_idx = j;
                break;
            }
        }
    }

    // printf("cups: ");
    // for(size_t j = 0; j < n_cups; ++j) {
    //     printf("%lu ", cups[j]);
    // }
    // printf("\n");

    size_t ans = 0;
    size_t cup_one_idx = 0;
    for(size_t i = 0; i < n_cups; ++i) {
        if(cups[i] == 1) {
            cup_one_idx = i;
            break;
        }
    }
    for(size_t i = 0; i < (n_cups - 1); ++i) {
        ans *= 10;
        ans += cups[(i + cup_one_idx + 1) % n_cups];
    }
    // printf("ans = %lu\n", ans);

    return ans;
}

void day_23_part_1_example_1()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_23_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size, 10);

    free(input);

    TEST_ASSERT_EQUAL_size_t(92658374, ans);
}

void day_23_part_1_example_2()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_23_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size, 100);

    free(input);

    TEST_ASSERT_EQUAL_size_t(67384529, ans);
}

void day_23_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_23_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size, 100);

    free(input);

    TEST_ASSERT_EQUAL_size_t(27956483, ans);
}

