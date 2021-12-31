#include "day_13.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

typedef struct {
    uint64_t id;
    size_t pos;
} bus_t;

static void parse_input(const char* const input, size_t size, uint64_t* output_min_departure_time, bus_t** output_buses, size_t* output_n_buses)
{
    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    char* min_departure_time_str = malloc(line_mem_refs[0].size + 1);
    min_departure_time_str[line_mem_refs[0].size] = 0;
    memcpy(min_departure_time_str, &input[line_mem_refs[0].offset], line_mem_refs[0].size);
    const uint64_t min_departure_time = atoi(min_departure_time_str);
    free(min_departure_time_str);

    const char* const line_2 = &input[line_mem_refs[1].offset];
    memory_reference_t* bus_id_mem_refs = NULL;
    size_t n_bus_id_mem_refs = 0;
    split_buf_by_sequence(line_2, line_mem_refs[1].size, ",", &bus_id_mem_refs, &n_bus_id_mem_refs);

    bus_t* buses = (bus_t*)malloc(n_bus_id_mem_refs * sizeof(bus_t));
    size_t n_buses = 0;
    for(size_t i = 0; i < n_bus_id_mem_refs; i++) {
        const char* const bus_id_buf = &line_2[bus_id_mem_refs[i].offset];
        const size_t bus_id_len = bus_id_mem_refs[i].size;

        if(bus_id_len == 1 && bus_id_buf[0] == 'x') {
            continue;
        }

        char* bus_id_str = malloc(bus_id_len + 1);
        bus_id_str[bus_id_len] = 0;
        memcpy(bus_id_str, bus_id_buf, bus_id_len);
        const uint64_t bus_id = atoi(bus_id_str);
        free(bus_id_str);

        buses[n_buses].id = bus_id;
        buses[n_buses].pos = i;
        n_buses++;
    }

    free(bus_id_mem_refs);
    free(line_mem_refs);

    *output_min_departure_time = min_departure_time;
    *output_buses = buses;
    *output_n_buses = n_buses;
}

static uint64_t solve_part_1(const char* const input, size_t size)
{
    uint64_t min_departure_time = 0;
    bus_t* buses = NULL;
    uint64_t n_buses = 0;
    parse_input(input, size, &min_departure_time, &buses, &n_buses);

    bool bus_found = false;
    uint64_t bus_id = 0;
    uint64_t departure_time = min_departure_time;
    while(!bus_found) {
        for(size_t i = 0; i < n_buses; i++) {
            if(departure_time % buses[i].id == 0) {
                bus_found = true;
                bus_id = buses[i].id;
                break;
            }
        }
        departure_time++;
    }
    departure_time--;

    free(buses);

    return (departure_time - min_departure_time) * bus_id;
}

static uint64_t solve_part_2(const char* const input, size_t size)
{
    uint64_t min_departure_time = 0;
    bus_t* buses = NULL;
    uint64_t n_buses = 0;
    parse_input(input, size, &min_departure_time, &buses, &n_buses);

    uint64_t* rem = (uint64_t*)malloc(n_buses * sizeof(uint64_t));
    uint64_t* pp = (uint64_t*)malloc(n_buses * sizeof(uint64_t));
    uint64_t* inv = (uint64_t*)malloc(n_buses * sizeof(uint64_t));
    uint64_t prod = 1;

    for(size_t i = 0; i < n_buses; i++) {
        rem[i] = (buses[i].id - buses[i].pos % buses[i].id) % buses[i].id;
    }
    for(size_t i = 0; i < n_buses; i++) {
        prod *= buses[i].id;
    }
    for(size_t i = 0; i < n_buses; i++) {
        pp[i] = prod / buses[i].id;
    }
    for(size_t i = 0; i < n_buses; i++) {
        uint64_t n = 1;
        while((pp[i] * n) % buses[i].id != 1) {
            n++;
        }
        inv[i] = n;
    }
    uint64_t x = 0;
    for(size_t i = 0; i < n_buses; i++) {
        x += rem[i] * pp[i] * inv[i];
    }
    x %= prod;

    free(inv);
    free(pp);
    free(rem);
    free(buses);

    return x;
}

void day_13_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_13_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(295, ans);
}

void day_13_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_13_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const int64_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(2406, ans);
}

void day_13_part_2_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_13_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(1068781, ans);
}

void day_13_part_2_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_13_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const uint64_t ans = solve_part_2(input, size);

    free(input);

    TEST_ASSERT_EQUAL_UINT64(46530, ans);
}
