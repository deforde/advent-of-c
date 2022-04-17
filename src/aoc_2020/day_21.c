#include "day_21.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

#define MAX_NUM_INGREDIENTS 1024
#define MAX_NUM_ALLERGENS 1024
#define MAX_INGREDIENT_LEN 128
#define MAX_ALLERGEN_LEN 128

typedef struct {
    char list[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN];
    size_t n_ingredients;
} ingredients_t;

typedef struct {
    char allergen[MAX_ALLERGEN_LEN];
    ingredients_t ingredients;
} allergen_t;

static size_t solve_part_1(const char* const input, size_t size)
{
    size_t ans = 0;
    // allergen_t allergens[MAX_NUM_ALLERGENS] = {0};
    // size_t n_allergens = 0;

    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    for(size_t line_idx = 0; line_idx < n_lines; ++line_idx) {
        printf("Line #%lu\n", line_idx);
        const char* const line = &input[line_mem_refs[line_idx].offset];
        const size_t line_len = line_mem_refs[line_idx].size;

        memory_reference_t* section_mem_refs = NULL;
        size_t n_sections = 0;
        split_buf_by_sequence(line, line_len, " (contains ", &section_mem_refs, &n_sections);

        char line_allergens[MAX_NUM_ALLERGENS][MAX_ALLERGEN_LEN] = {0};
        size_t n_line_allergens = 0;

        if(n_sections) {
            if(n_sections > 1) {
                const char* const allergen_section = &line[section_mem_refs[1].offset];
                const size_t allergen_section_len = section_mem_refs[1].size - 1;

                memory_reference_t* allergen_mem_refs = NULL;
                size_t n_allergen_mem_refs = 0;
                split_buf_by_sequence(allergen_section, allergen_section_len, ", ", &allergen_mem_refs, &n_allergen_mem_refs);

                for(size_t allergen_idx = 0; allergen_idx < n_allergen_mem_refs; ++allergen_idx) {
                    const char* const line_allergen = &allergen_section[allergen_mem_refs[allergen_idx].offset];
                    const size_t line_allergen_len = allergen_mem_refs[allergen_idx].size;
                    memcpy(&line_allergens[n_line_allergens++], line_allergen, line_allergen_len);
                    printf("Allergen: %s\n", line_allergens[n_line_allergens - 1]);
                }

                free(allergen_mem_refs);
            }

            const char* const ingredient_section = &line[section_mem_refs[0].offset];
            const size_t ingredient_section_len = section_mem_refs[0].size;

            memory_reference_t* ingredient_mem_refs = NULL;
            size_t n_ingredients = 0;
            split_buf_by_sequence(ingredient_section, ingredient_section_len, " ", &ingredient_mem_refs, &n_ingredients);

            char line_ingredients[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN] = {0};
            size_t n_line_ingredients = 0;

            for(size_t ingredient_idx = 0; ingredient_idx < n_ingredients; ++ingredient_idx) {
                const char* const line_ingredient = &ingredient_section[ingredient_mem_refs[ingredient_idx].offset];
                const size_t line_ingredient_len = ingredient_mem_refs[ingredient_idx].size;
                memcpy(&line_ingredients[n_line_ingredients++], line_ingredient, line_ingredient_len);
                printf("Ingredient: %s\n", line_ingredients[n_line_ingredients - 1]);
            }

            free(ingredient_mem_refs);
        }

        free(section_mem_refs);
    }

    free(line_mem_refs);

    return ans;
}

void day_21_part_1_example()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_21_part_1_example.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(13983397496713, ans);
}

