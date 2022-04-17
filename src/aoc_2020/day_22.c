#include "day_22.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

#define MAX_NUM_CARDS 1024

typedef struct {
    size_t cards[MAX_NUM_CARDS];
    size_t n_cards;
} hand_t;

static size_t play_game(hand_t hands[2])
{
    // size_t round = 1;
    while(hands[0].n_cards && hands[1].n_cards) {
        hand_t* p1_hand = &hands[0];
        hand_t* p2_hand = &hands[1];

        const size_t p1_card = p1_hand->cards[0];
        const size_t p2_card = p2_hand->cards[0];

        memmove(p1_hand->cards, &p1_hand->cards[1], (p1_hand->n_cards - 1) * sizeof(*p1_hand->cards));
        memmove(p2_hand->cards, &p2_hand->cards[1], (p2_hand->n_cards - 1) * sizeof(*p2_hand->cards));

        p1_hand->n_cards--;
        p2_hand->n_cards--;

        hand_t* winner = (p1_card > p2_card) ? p1_hand : p2_hand;
        const size_t max = (p1_card > p2_card) ? p1_card : p2_card;
        const size_t min = (p1_card > p2_card) ? p2_card : p1_card;
        winner->cards[winner->n_cards++] = max;
        winner->cards[winner->n_cards++] = min;

        // printf("Round #%lu\n", round++);
        // printf("Player 1: ");
        // for(size_t i = 0; i < p1_hand->n_cards; ++i) {
        //     printf("%lu ", p1_hand->cards[i]);
        // }
        // printf("\n");
        // printf("Player 2: ");
        // for(size_t i = 0; i < p2_hand->n_cards; ++i) {
        //     printf("%lu ", p2_hand->cards[i]);
        // }
        // printf("\n\n");
    }
    hand_t* winner = hands[0].n_cards ? &hands[0] : &hands[1];
    size_t score = 0;
    for(size_t i = 0; i < winner->n_cards; ++i) {
        score += (winner->n_cards - i) * winner->cards[i];
    }
    return score;
}

static size_t solve_part_1(const char* const input, size_t size)
{
    hand_t hands[2] = {0};

    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;
    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);
    assert(n_blocks == 2);

    for(size_t block_idx = 0; block_idx < n_blocks; ++block_idx) {
        hand_t* hand = &hands[block_idx];

        const char* const block = &input[block_mem_refs[block_idx].offset];
        const size_t block_len = block_mem_refs[block_idx].size;

        memory_reference_t* line_mem_refs = NULL;
        size_t n_lines = 0;
        split_buf_by_sequence(block, block_len, "\n", &line_mem_refs, &n_lines);
        assert(n_lines > 1);

        for(size_t line_idx = 1; line_idx < n_lines; ++line_idx) {
            const char* const line = &block[line_mem_refs[line_idx].offset];
            const size_t line_len = line_mem_refs[line_idx].size;

            char temp[128] = {0};
            memcpy(temp, line, line_len);
            hand->cards[hand->n_cards++] = atoi(temp);
        }

        free(line_mem_refs);
    }

    free(block_mem_refs);

    const size_t score = play_game(hands);

    return score;
}

void day_22_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_22_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(306, ans);
}

void day_22_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_22_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(306, ans);
}

