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

static size_t score_hand(hand_t* hand)
{
    size_t score = 0;
    for(size_t i = 0; i < hand->n_cards; ++i) {
        score += (hand->n_cards - i) * hand->cards[i];
    }
    return score;
}

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
    return score_hand(winner);
}

static size_t play_recursive_game(hand_t hands[2], size_t depth)
{
    size_t max_num_historic_hands = 128;
    hand_t* game_history = (hand_t*)malloc(max_num_historic_hands * 2 * sizeof(hand_t));
    size_t n_historic_hands = 0;

    // size_t round = 1;
    while(hands[0].n_cards && hands[1].n_cards) {
        hand_t* p1_hand = &hands[0];
        hand_t* p2_hand = &hands[1];

        // printf("Game #%lu\n", depth + 1);
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

        for(size_t i = 0; i < n_historic_hands; ++i) {
            hand_t* p1_hist_hand = &game_history[i * 2];
            hand_t* p2_hist_hand = &game_history[i * 2 + 1];

            bool match = true;
            match &= p1_hist_hand->n_cards == p1_hand->n_cards;
            if(match) {
                for(size_t j = 0; j < p1_hist_hand->n_cards; ++j) {
                    match &= p1_hand->cards[j] == p1_hist_hand->cards[j];
                    if(!match) {
                        break;
                    }
                }
            }
            match &= p2_hist_hand->n_cards == p2_hand->n_cards;
            if(match) {
                for(size_t j = 0; j < p2_hist_hand->n_cards; ++j) {
                    match &= p2_hand->cards[j] == p2_hist_hand->cards[j];
                    if(!match) {
                        break;
                    }
                }
            }
            if(match) {
                free(game_history);
                if(depth == 0) {
                    return score_hand(p1_hand);
                }
                return 0;
            }
        }
        if(n_historic_hands == max_num_historic_hands) {
            hand_t* temp = (hand_t*)malloc(max_num_historic_hands * 2 * 2 * sizeof(hand_t));
            memcpy(temp, game_history, n_historic_hands * 2 * sizeof(hand_t));
            max_num_historic_hands *= 2;
            free(game_history);
            game_history = temp;
        }
        game_history[n_historic_hands * 2].n_cards = p1_hand->n_cards;
        game_history[n_historic_hands * 2 + 1].n_cards = p2_hand->n_cards;
        memcpy(game_history[n_historic_hands * 2].cards, p1_hand->cards, p1_hand->n_cards * sizeof(*p1_hand->cards));
        memcpy(game_history[n_historic_hands * 2 + 1].cards, p2_hand->cards, p2_hand->n_cards * sizeof(*p2_hand->cards));
        n_historic_hands++;

        const size_t p1_card = p1_hand->cards[0];
        const size_t p2_card = p2_hand->cards[0];

        memmove(p1_hand->cards, &p1_hand->cards[1], (p1_hand->n_cards - 1) * sizeof(*p1_hand->cards));
        memmove(p2_hand->cards, &p2_hand->cards[1], (p2_hand->n_cards - 1) * sizeof(*p2_hand->cards));

        p1_hand->n_cards--;
        p2_hand->n_cards--;

        size_t winner_idx = 0;
        if(p1_hand->n_cards >= p1_card && p2_hand->n_cards >= p2_card) {
            hand_t new_hands[2] = {0};
            hand_t* p1_new_hand = &new_hands[0];
            hand_t* p2_new_hand = &new_hands[1];
            p1_new_hand->n_cards = p1_card;
            p2_new_hand->n_cards = p2_card;
            memcpy(p1_new_hand->cards, p1_hand->cards, p1_new_hand->n_cards * sizeof(*p1_new_hand->cards));
            memcpy(p2_new_hand->cards, p2_hand->cards, p2_new_hand->n_cards * sizeof(*p2_new_hand->cards));
            winner_idx = play_recursive_game(new_hands, depth + 1);
        }
        else {
            winner_idx = (p1_card > p2_card) ? 0 : 1;
        }

        hand_t* winner = &hands[winner_idx];
        winner->cards[winner->n_cards++] = (winner_idx == 0) ? p1_card : p2_card;
        winner->cards[winner->n_cards++] = (winner_idx == 0) ? p2_card : p1_card;
    }

    free(game_history);
    const size_t winner = hands[0].n_cards ? 0 : 1;
    if(depth == 0) {
        return score_hand(&hands[winner]);
    }
    return winner;
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

static size_t solve_part_2(const char* const input, size_t size)
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

    const size_t score = play_recursive_game(hands, 0);

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

    TEST_ASSERT_EQUAL_size_t(32783, ans);
}

void day_22_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_22_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(291, ans);
}

void day_22_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_22_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(33455, ans);
}

