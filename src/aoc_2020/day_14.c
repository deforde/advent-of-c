#include "day_14.h"

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
    uint64_t address;
    uint64_t value;
} mem_register_t;

static void process_mem_instruction(const char* const instr, size_t len, uint64_t* address, uint64_t* value)
{
    char* address_str = malloc(len);
    memset(address_str, 0, len);

    size_t i = 4;
    for(; i < len; i++) {
        if(instr[i] == ']') {
            break;
        }
        address_str[i - 4] = instr[i];
    }

    *address = atoll(address_str);
    free(address_str);

    char* value_str = malloc(len - (i + 4) + 1);
    memset(value_str, 0, len - (i + 4) + 1);
    for(size_t j = i + 4; j < len; j++) {
        value_str[j - (i + 4)] = instr[j];
    }

    *value = atoll(value_str);
    free(value_str);
}

static void process_mask_instruction_part_1(const char* const instr, size_t len, uint64_t* bit_mask_and, uint64_t* bit_mask_or)
{
    uint64_t new_bit_mask_and = 0;
    uint64_t new_bit_mask_or = 0;

    for(size_t i = 7; i < len; i++) {
        new_bit_mask_and <<= 1;
        new_bit_mask_or <<= 1;

        switch(instr[i]) {
            case 'X': {
                new_bit_mask_and |= 1;
                new_bit_mask_or &= (~UINT64_C(1));
                break;
            }
            case '1': {
                new_bit_mask_and |= 1;
                new_bit_mask_or |= 1;
                break;
            }
            case '0': {
                new_bit_mask_and &= (~UINT64_C(1));
                new_bit_mask_or &= (~UINT64_C(1));
                break;
            }
        }
    }

    *bit_mask_and = new_bit_mask_and;
    *bit_mask_or = new_bit_mask_or;
}

static void process_instruction_part_1(const char* const instr, size_t len, uint64_t* bit_mask_and, uint64_t* bit_mask_or, mem_register_t* registers, size_t* n_registers)
{
    if(memcmp(instr, "mem", 3) == 0) {
        uint64_t address = 0;
        uint64_t value = 0;
        process_mem_instruction(instr, len, &address, &value);

        value |= *bit_mask_or;
        value &= *bit_mask_and;

        bool new_address = true;
        for(size_t i = 0; i < *n_registers; i++) {
            if(registers[i].address == address) {
                new_address = false;
                registers[i].value = value;
            }
        }
        if(new_address) {
            registers[*n_registers].address = address;
            registers[*n_registers].value = value;
            *n_registers += 1;
        }
    }
    else {
        process_mask_instruction_part_1(instr, len, bit_mask_and, bit_mask_or);
    }
}

static uint64_t solve_part_1(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    mem_register_t* registers = (mem_register_t*)malloc(n_lines * sizeof(mem_register_t));
    size_t n_registers = 0;

    uint64_t bit_mask_and = 0;
    uint64_t bit_mask_or = 0;

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const instr = &input[line_mem_refs[line_idx].offset];
        const size_t len = line_mem_refs[line_idx].size;
        process_instruction_part_1(instr, len, &bit_mask_and, &bit_mask_or, registers, &n_registers);
    }

    uint64_t sum = 0;
    for(size_t i = 0; i < n_registers; i++) {
        sum += registers[i].value;
    }

    free(registers);
    free(line_mem_refs);

    return sum;
}

static void process_mask_instruction_part_2(const char* const instr, size_t len, uint64_t* bit_mask_or, size_t* wild_bit_indices, size_t* n_wild_bit_indices)
{
    uint64_t new_bit_mask_or = 0;
    memset(wild_bit_indices, 0, *n_wild_bit_indices * sizeof(size_t));
    *n_wild_bit_indices = 0;

    for(size_t i = 7; i < len; i++) {
        new_bit_mask_or <<= 1;

        switch(instr[i]) {
            case 'X': {
                wild_bit_indices[*n_wild_bit_indices] = i - 7;
                *n_wild_bit_indices += 1;
                break;
            }
            case '1': {
                new_bit_mask_or |= 1;
                break;
            }
        }
    }

    *bit_mask_or = new_bit_mask_or;
}

static void build_addresses(uint64_t address, size_t* wild_bit_indices, size_t* n_wild_bit_indices, size_t bit_idx, uint64_t* addresses, size_t* n_addresses)
{
    if(bit_idx == *n_wild_bit_indices) {
        addresses[*n_addresses] = address;
        *n_addresses += 1;
        return;
    }

    const size_t idx = 35 - wild_bit_indices[bit_idx];

    uint64_t new_address = address | (UINT64_C(1) << idx);
    build_addresses(new_address, wild_bit_indices, n_wild_bit_indices, bit_idx + 1, addresses, n_addresses);

    new_address = address & (~(UINT64_C(1) << idx));
    build_addresses(new_address, wild_bit_indices, n_wild_bit_indices, bit_idx + 1, addresses, n_addresses);
}

static void process_instruction_part_2(const char* const instr, size_t len, uint64_t* bit_mask_or, mem_register_t* registers, size_t* n_registers, size_t* wild_bit_indices, size_t* n_wild_bit_indices)
{
    if(memcmp(instr, "mem", 3) == 0) {
        uint64_t address = 0;
        uint64_t value = 0;
        process_mem_instruction(instr, len, &address, &value);

        address |= *bit_mask_or;

        uint64_t* addresses = (uint64_t*)malloc((size_t)(powf(2, *n_wild_bit_indices) + 0.5) * sizeof(uint64_t));
        size_t n_addresses = 0;

        build_addresses(address, wild_bit_indices, n_wild_bit_indices, 0, addresses, &n_addresses);

        for(size_t i = 0; i < n_addresses; i++) {
            address = addresses[i];

            bool new_address = true;
            for(size_t i = 0; i < *n_registers; i++) {
                if(registers[i].address == address) {
                    new_address = false;
                    registers[i].value = value;
                }
            }
            if(new_address) {
                registers[*n_registers].address = address;
                registers[*n_registers].value = value;
                *n_registers += 1;
            }
        }

        free(addresses);
    }
    else {
        process_mask_instruction_part_2(instr, len, bit_mask_or, wild_bit_indices, n_wild_bit_indices);
    }
}

static uint64_t solve_part_2(const char* const input, size_t size)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    mem_register_t* registers = (mem_register_t*)malloc(n_lines * 1000 * sizeof(mem_register_t));
    size_t n_registers = 0;

    size_t* wild_bit_indices = (size_t*)malloc(36 * sizeof(size_t));
    size_t n_wild_bit_indices = 0;

    uint64_t bit_mask_or = 0;

    for(size_t line_idx = 0; line_idx < n_lines; line_idx++) {
        const char* const instr = &input[line_mem_refs[line_idx].offset];
        const size_t len = line_mem_refs[line_idx].size;
        process_instruction_part_2(instr, len, &bit_mask_or, registers, &n_registers, wild_bit_indices, &n_wild_bit_indices);
    }

    uint64_t sum = 0;
    for(size_t i = 0; i < n_registers; i++) {
        sum += registers[i].value;
    }

    free(wild_bit_indices);
    free(registers);
    free(line_mem_refs);

    return sum;
}

void day_14_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_14_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(165, ans);
}

void day_14_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_14_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(13496669152158, ans);
}

void day_14_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_14_part_2_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(208, ans);
}

void day_14_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_14_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(3278997609887, ans);
}
