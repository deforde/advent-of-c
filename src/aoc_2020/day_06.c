#include "day_06.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

static size_t count_unique_chars(const char* const buf, size_t len)
{
    char* unique_chars = malloc(len);
    memset(unique_chars, 0, len);
    size_t num_unique_chars = 0;

    for(size_t i = 0; i < len; i++) {
        if(isalpha(buf[i])) {
            bool char_unique = true;
            for(size_t j = 0; j < num_unique_chars; j++) {
                if(buf[i] == unique_chars[j]) {
                    char_unique = false;
                    break;
                }
            }
            if(char_unique) {
                unique_chars[num_unique_chars++] = buf[i];
            }
        }
    }

    return num_unique_chars;
}

static size_t count_unanimous_answers(const char* const buf, size_t len)
{
    memory_reference_t* participant_answers_mem_ref = NULL;
    size_t n_participants = 0;

    split_buf_by_whitespace(buf, len, &participant_answers_mem_ref, &n_participants);

    typedef struct {
        char ans;
        size_t count;
    } answer_t;

    answer_t* answers = (answer_t*)malloc(len * sizeof(answer_t));
    memset(answers, 0, len * sizeof(answer_t));
    size_t n_answers = 0;

    for(size_t participant_idx = 0; participant_idx < n_participants; participant_idx++) {
        const char* const participant_answers = &buf[participant_answers_mem_ref[participant_idx].offset];
        size_t n_participant_answers = participant_answers_mem_ref[participant_idx].size;

        for(size_t participant_answer_idx = 0; participant_answer_idx < n_participant_answers; participant_answer_idx++) {
            bool answer_located = false;
            for(size_t answer_idx = 0; answer_idx < n_answers; answer_idx++) {
                if(answers[answer_idx].ans == participant_answers[participant_answer_idx]) {
                    answer_located = true;
                    answers[answer_idx].count++;
                    break;
                }
            }
            if(!answer_located) {
                answers[n_answers].ans = participant_answers[participant_answer_idx];
                answers[n_answers].count = 1;
                n_answers++;
            }
        }
    }

    size_t count = 0;
    for(size_t answer_idx = 0; answer_idx < n_answers; answer_idx++) {
        if(answers[answer_idx].count == n_participants) {
            count++;
        }
    }

    free(participant_answers_mem_ref);

    return count;
}

static size_t solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;

    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);

    size_t sum = 0;
    for(size_t block_idx = 0; block_idx < n_blocks; block_idx++) {
        const size_t count = count_unique_chars(&input[block_mem_refs[block_idx].offset], block_mem_refs[block_idx].size);
        sum += count;
    }

    free(block_mem_refs);

    return sum;
}

static size_t solve_part_2(const char* const input, size_t size)
{
    memory_reference_t* block_mem_refs = NULL;
    size_t n_blocks = 0;

    split_buf_by_sequence(input, size, "\n\n", &block_mem_refs, &n_blocks);

    size_t sum = 0;
    for(size_t block_idx = 0; block_idx < n_blocks; block_idx++) {
        const size_t count = count_unanimous_answers(&input[block_mem_refs[block_idx].offset], block_mem_refs[block_idx].size);
        sum += count;
    }

    free(block_mem_refs);

    return sum;
}

void day_06_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_06_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_1(input, size);
    TEST_ASSERT_EQUAL_UINT64(11, ans);

    free(input);
}

void day_06_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_06_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_1(input, size);
    TEST_ASSERT_EQUAL_UINT64(6437, ans);

    free(input);
}

void day_06_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_06_part_1_example.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_2(input, size);
    TEST_ASSERT_EQUAL_UINT64(6, ans);

    free(input);
}

void day_06_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_06_part_1_input.txt", &input, &size);
    assert(success);

    const uint64_t ans = solve_part_2(input, size);
    TEST_ASSERT_EQUAL_UINT64(3229, ans);

    free(input);
}
