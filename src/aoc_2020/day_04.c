#include "day_04.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

#define PASSPORT_MAX_VAL_LEN 128

typedef struct {
    char byr[PASSPORT_MAX_VAL_LEN];
    char iyr[PASSPORT_MAX_VAL_LEN];
    char eyr[PASSPORT_MAX_VAL_LEN];
    char hgt[PASSPORT_MAX_VAL_LEN];
    char hcl[PASSPORT_MAX_VAL_LEN];
    char ecl[PASSPORT_MAX_VAL_LEN];
    char pid[PASSPORT_MAX_VAL_LEN];
    char cid[PASSPORT_MAX_VAL_LEN];
} passport_t;

static void populate_passport_kvpair(passport_t* const passport, const char* const key, size_t key_len, const char* const val, size_t val_len)
{
    assert(key_len == 3);
    assert(val_len <= PASSPORT_MAX_VAL_LEN);

    if(memcmp(key, "byr", 3) == 0) {
        memcpy(passport->byr, val, val_len);
    }
    else if(memcmp(key, "iyr", 3) == 0) {
        memcpy(passport->iyr, val, val_len);
    }
    else if(memcmp(key, "eyr", 3) == 0) {
        memcpy(passport->eyr, val, val_len);
    }
    else if(memcmp(key, "hgt", 3) == 0) {
        memcpy(passport->hgt, val, val_len);
    }
    else if(memcmp(key, "hcl", 3) == 0) {
        memcpy(passport->hcl, val, val_len);
    }
    else if(memcmp(key, "ecl", 3) == 0) {
        memcpy(passport->ecl, val, val_len);
    }
    else if(memcmp(key, "pid", 3) == 0) {
        memcpy(passport->pid, val, val_len);
    }
    else if(memcmp(key, "cid", 3) == 0) {
        memcpy(passport->cid, val, val_len);
    }
}

static bool is_passport_fields_valid(const passport_t* const passport)
{
    return passport->byr[0] != 0 &&
        passport->iyr[0] != 0 &&
        passport->eyr[0] != 0 &&
        passport->hgt[0] != 0 &&
        passport->hcl[0] != 0 &&
        passport->ecl[0] != 0 &&
        passport->pid[0] != 0;
}

static bool is_passport_data_valid(const passport_t* const passport)
{
    if(!is_passport_fields_valid(passport)) {
        return false;
    }

    bool result = true;

    result &= passport->byr[4] == 0;
    if(!result) {
        return result;
    }
    const uint32_t byr = atoi(passport->byr);
    result &= byr >= 1920 && byr <= 2002;
    if(!result) {
        return result;
    }

    result &= passport->iyr[4] == 0;
    if(!result) {
        return result;
    }
    const uint32_t iyr = atoi(passport->iyr);
    result &= iyr >= 2010 && iyr <= 2020;
    if(!result) {
        return result;
    }

    result &= passport->eyr[4] == 0;
    if(!result) {
        return result;
    }
    const uint32_t eyr = atoi(passport->eyr);
    result &= eyr >= 2020 && eyr <= 2030;
    if(!result) {
        return result;
    }

    result &= passport->hcl[0] == '#';
    if(!result) {
        return result;
    }
    result &= passport->hcl[7] == 0;
    if(!result) {
        return result;
    }
    for(size_t i = 1; i < 7; i++) {
        result &= isxdigit(passport->hcl[i]) != 0;
        if(!result) {
            return result;
        }
    }

    result &= passport->ecl[3] == 0;
    if(!result) {
        return result;
    }
    result &= memcmp(passport->ecl, "amb", 3) == 0 ||
        memcmp(passport->ecl, "blu", 3) == 0 ||
        memcmp(passport->ecl, "brn", 3) == 0 ||
        memcmp(passport->ecl, "gry", 3) == 0 ||
        memcmp(passport->ecl, "grn", 3) == 0 ||
        memcmp(passport->ecl, "hzl", 3) == 0 ||
        memcmp(passport->ecl, "oth", 3) == 0;
    if(!result) {
        return result;
    }

    result &= passport->pid[9] == 0;
    if(!result) {
        return result;
    }
    for(size_t i = 0; i < 9; i++) {
        result &= isdigit(passport->pid[i]) != 0;
        if(!result) {
            return result;
        }
    }

    result &= isdigit(passport->hgt[0]) != 0;
    if(!result) {
        return result;
    }
    size_t unit_pos = 0;
    for(size_t i = 1; i < PASSPORT_MAX_VAL_LEN; i++) {
        if(passport->hgt[i] == 0) {
            break;
        }
        if(!isdigit(passport->hgt[i]) != 0) {
            unit_pos = i;
            break;
        }
    }
    result &= memcmp(&passport->hgt[unit_pos], "cm", 2) == 0 ||
        memcmp(&passport->hgt[unit_pos], "in", 2) == 0;
    if(!result) {
        return result;
    }
    result &= passport->hgt[unit_pos + 2] == 0;
    if(!result) {
        return result;
    }

    return result;
}

static void parse_input(const char* const input, size_t size, passport_t** passports_ref, size_t* n_passports)
{
    memory_reference_t* passport_mem_refs = NULL;
    size_t n_passport_mem_refs = 0;
    split_buf_by_sequence(input, size, "\n\n", &passport_mem_refs, &n_passport_mem_refs);

    passport_t* passports = (passport_t*)malloc(n_passport_mem_refs * sizeof(passport_t));
    memset(passports, 0, n_passport_mem_refs * sizeof(passport_t));

    for(size_t passport_mem_ref_idx = 0; passport_mem_ref_idx < n_passport_mem_refs; passport_mem_ref_idx++) {
        const size_t passport_offset = passport_mem_refs[passport_mem_ref_idx].offset;
        const size_t passport_size = passport_mem_refs[passport_mem_ref_idx].size;

        memory_reference_t* field_mem_refs = NULL;
        size_t n_field_mem_refs = 0;
        split_buf_by_whitespace(&input[passport_offset], passport_size, &field_mem_refs, &n_field_mem_refs);
        for(size_t field_mem_ref_idx = 0; field_mem_ref_idx < n_field_mem_refs; field_mem_ref_idx++) {
            const size_t field_offset = field_mem_refs[field_mem_ref_idx].offset + passport_offset;
            const size_t field_size = field_mem_refs[field_mem_ref_idx].size;

            memory_reference_t* kvpair_mem_refs = NULL;
            size_t n_kvpair_mem_refs = 0;
            split_buf_by_sequence(&input[field_offset], field_size, ":", &kvpair_mem_refs, &n_kvpair_mem_refs);
            assert(n_kvpair_mem_refs == 2);

            const char* const key = &input[field_offset + kvpair_mem_refs[0].offset];
            const size_t key_len = kvpair_mem_refs[0].size;
            const char* const val = &input[field_offset + kvpair_mem_refs[1].offset];
            const size_t val_len = kvpair_mem_refs[1].size;
            populate_passport_kvpair(&passports[passport_mem_ref_idx], key, key_len, val, val_len);

            free(kvpair_mem_refs);
        }

        free(field_mem_refs);
    }

    free(passport_mem_refs);

    *passports_ref = passports;
    *n_passports = n_passport_mem_refs;
}

static size_t solve_part_1(const char* const input, size_t size)
{
    passport_t* passports = NULL;
    size_t n_passports = 0;
    parse_input(input, size, &passports, &n_passports);

    size_t n_valid_passports = 0;
    for(size_t i = 0; i < n_passports; i++) {
        if(is_passport_fields_valid(&passports[i])) {
            n_valid_passports++;
        }
    }

    free(passports);

    return n_valid_passports;
}

static size_t solve_part_2(const char* const input, size_t size)
{
    passport_t* passports = NULL;
    size_t n_passports = 0;
    parse_input(input, size, &passports, &n_passports);

    size_t n_valid_passports = 0;
    for(size_t i = 0; i < n_passports; i++) {
        if(is_passport_data_valid(&passports[i])) {
            n_valid_passports++;
        }
    }

    free(passports);

    return n_valid_passports;
}

void day_04_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_04_part_1_example.txt", &input, &size);
    assert(success);

    const size_t ans = solve_part_1(input, size);
    TEST_ASSERT_EQUAL_UINT64(2, ans);

    free(input);
}

void day_04_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_04_part_1_input.txt", &input, &size);
    assert(success);

    const size_t ans = solve_part_1(input, size);
    TEST_ASSERT_EQUAL_UINT64(245, ans);

    free(input);
}

void day_04_part_2_example_1()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_04_part_2_example_1.txt", &input, &size);
    assert(success);

    const size_t ans = solve_part_2(input, size);
    TEST_ASSERT_EQUAL_UINT64(0, ans);

    free(input);
}

void day_04_part_2_example_2()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_04_part_2_example_2.txt", &input, &size);
    assert(success);

    const size_t ans = solve_part_2(input, size);
    TEST_ASSERT_EQUAL_UINT64(4, ans);

    free(input);
}

void day_04_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_04_part_1_input.txt", &input, &size);
    assert(success);

    const size_t ans = solve_part_2(input, size);
    TEST_ASSERT_EQUAL_UINT64(245, ans);

    free(input);
}
