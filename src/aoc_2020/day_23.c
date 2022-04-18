#include "day_23.h"

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

#define NUM_CUPS 9
#define NUM_CUPS_PART_2 1000000
#define SELECTED_CUPS_PER_MOVE 3

typedef struct cup_t {
    size_t val;
    struct cup_t* next;
} cup_t;

static void process_input(const char* const input, size_t size, cup_t* cups, size_t* first_cup_val)
{
    cup_t* prev_cup = NULL;
    cup_t* first_cup = NULL;
    for(size_t i = 0; i < size; ++i) {
        if(isspace(input[i]) != 0) {
            break;
        }
        char temp[2] = {0};
        temp[0] = input[i];
        const size_t val = atoi(temp);
        if(i == 0) {
            first_cup = &cups[val - 1];
            *first_cup_val = val;
        }
        if(prev_cup) {
            prev_cup->next = &cups[val - 1];
        }
        prev_cup = &cups[val - 1];
    }
    prev_cup->next = first_cup;
}

static void process_input_2(const char* const input, size_t size, cup_t* cups, size_t* first_cup_val)
{
    cup_t* prev_cup = NULL;
    cup_t* first_cup = NULL;
    for(size_t i = 0; i < size; ++i) {
        if(isspace(input[i]) != 0) {
            break;
        }
        char temp[2] = {0};
        temp[0] = input[i];
        const size_t val = atoi(temp);
        if(i == 0) {
            first_cup = &cups[val - 1];
            *first_cup_val = val;
        }
        if(prev_cup) {
            prev_cup->next = &cups[val - 1];
        }
        prev_cup = &cups[val - 1];
    }
    for(size_t i = 9; i < NUM_CUPS_PART_2; ++i) {
        if(prev_cup) {
            prev_cup->next = &cups[i];
        }
        prev_cup = &cups[i];
    }
    prev_cup->next = first_cup;
}

static void play_game(cup_t* cups, size_t n_cups, size_t first_cup_val, size_t n_moves)
{
    cup_t* current_cup = &cups[first_cup_val - 1];
    for(size_t i = 0; i < n_moves; ++i) {
        cup_t* selected_cup = current_cup->next;
        cup_t* new_current_cup_connect = selected_cup;
        for(size_t j = 0; j < SELECTED_CUPS_PER_MOVE; ++j) {
            new_current_cup_connect = new_current_cup_connect->next;
        }
        current_cup->next = new_current_cup_connect;

        size_t destination_cup_val = current_cup->val;
        bool destination_cup_in_selection = false;
        do {
            destination_cup_val--;
            if(destination_cup_val == 0) {
                destination_cup_val = n_cups;
            }
            destination_cup_in_selection = false;
            cup_t* this_selected_cup = selected_cup;
            for(size_t j = 0; j < SELECTED_CUPS_PER_MOVE; ++j) {
                if(destination_cup_val == this_selected_cup->val) {
                    destination_cup_in_selection = true;
                    break;
                }
                this_selected_cup = this_selected_cup->next;
            }
        } while(destination_cup_in_selection);
        cup_t* destination_cup = &cups[destination_cup_val - 1];

        cup_t* selection_end_connect = destination_cup->next;
        destination_cup->next = selected_cup;
        cup_t* selection_end_cup = selected_cup;
        for(size_t j = 0; j < SELECTED_CUPS_PER_MOVE - 1; ++j) {
            selection_end_cup = selection_end_cup->next;
        }
        selection_end_cup->next = selection_end_connect;

        current_cup = new_current_cup_connect;
    }
}

static size_t solve_part_1(const char* const input, size_t size, size_t n_moves)
{
    cup_t cups[NUM_CUPS] = {0};
    for(size_t i = 0; i < NUM_CUPS; ++i) {
        cups[i].val = i + 1;
    }
    size_t first_cup_val = 0;
    process_input(input, size, cups, &first_cup_val);

    play_game(cups, NUM_CUPS, first_cup_val, n_moves);

    size_t ans = 0;
    cup_t* cup = cups[0].next;
    for(size_t i = 0; i < (NUM_CUPS - 1); ++i) {
        ans *= 10;
        ans += cup->val;
        cup = cup->next;
    }
    return ans;
}

static size_t solve_part_2(const char* const input, size_t size, size_t n_moves)
{
    cup_t* cups = (cup_t*)malloc(NUM_CUPS_PART_2 * sizeof(cup_t));
    memset(cups, 0, NUM_CUPS_PART_2 * sizeof(cup_t));
    for(size_t i = 0; i < NUM_CUPS_PART_2; ++i) {
        cups[i].val = i + 1;
    }
    size_t first_cup_val = 0;
    process_input_2(input, size, cups, &first_cup_val);

    for(size_t i = 0; i < NUM_CUPS_PART_2; ++i) {
        assert(cups[i].next);
    }

    play_game(cups, NUM_CUPS_PART_2, first_cup_val, n_moves);

    size_t ans = 1;
    cup_t* cup = cups[0].next;
    for(size_t i = 0; i < 2; ++i) {
        ans *= cup->val;
        cup = cup->next;
    }

    free(cups);

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

void day_23_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_23_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size, 10000000);

    free(input);

    TEST_ASSERT_EQUAL_size_t(149245887792, ans);
}

void day_23_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_23_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size, 10000000);

    free(input);

    TEST_ASSERT_EQUAL_size_t(149245887792, ans);
}

