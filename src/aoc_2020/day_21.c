#include "day_21.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "../utility.h"

#define MAX_NUM_INGREDIENTS 512
#define MAX_NUM_ALLERGENS 10
#define MAX_INGREDIENT_LEN 20
#define MAX_ALLERGEN_LEN 20

typedef struct {
    char list[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN];
    size_t n_ingredients;
} ingredients_t;

typedef struct {
    char list[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN];
    size_t n_ingredients;
    size_t count[MAX_NUM_INGREDIENTS];
} ingredient_counter_t;

typedef struct {
    char allergen_name[MAX_ALLERGEN_LEN];
    ingredients_t ingredients;
} allergen_t;

static allergen_t allergens[MAX_NUM_ALLERGENS] = {0};
static size_t n_allergens = 0;
static ingredient_counter_t all = {0};
static ingredients_t non_allergenic = {0};

static bool extract_line_data(const char* const line, size_t line_len, char allergens[MAX_NUM_ALLERGENS][MAX_ALLERGEN_LEN], size_t* n_allergens, char ingredients[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN], size_t* n_ingredients)
{
    *n_allergens = 0;
    *n_ingredients = 0;

    memory_reference_t* section_mem_refs = NULL;
    size_t n_sections = 0;
    split_buf_by_sequence(line, line_len, " (contains ", &section_mem_refs, &n_sections);

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
                memcpy(&allergens[(*n_allergens)++], line_allergen, line_allergen_len);
            }

            free(allergen_mem_refs);
        }

        const char* const ingredient_section = &line[section_mem_refs[0].offset];
        const size_t ingredient_section_len = section_mem_refs[0].size;

        memory_reference_t* ingredient_mem_refs = NULL;
        size_t n_ingredient_mem_refs = 0;
        split_buf_by_sequence(ingredient_section, ingredient_section_len, " ", &ingredient_mem_refs, &n_ingredient_mem_refs);

        for(size_t ingredient_idx = 0; ingredient_idx < n_ingredient_mem_refs; ++ingredient_idx) {
            const char* const line_ingredient = &ingredient_section[ingredient_mem_refs[ingredient_idx].offset];
            const size_t line_ingredient_len = ingredient_mem_refs[ingredient_idx].size;
            memcpy(&ingredients[(*n_ingredients)++], line_ingredient, line_ingredient_len);
        }

        free(ingredient_mem_refs);
    }

    free(section_mem_refs);

    return true;
}

static void ingredients_make_unique(ingredients_t* ingredients)
{
    for(size_t i = 0; i < ingredients->n_ingredients - 1; ++i) {
        char* str = ingredients->list[i];
        for(size_t j = i + 1; j < ingredients->n_ingredients; ++j) {
            char* other_str = ingredients->list[j];
            if(strlen(str) == strlen(other_str) && strncmp(str, other_str, strlen(str)) == 0) {
                memset(other_str, 0, strlen(str));
                memcpy(other_str, ingredients->list[ingredients->n_ingredients - 1], strlen(ingredients->list[ingredients->n_ingredients - 1]));
                memset(ingredients->list[ingredients->n_ingredients - 1], 0, strlen(ingredients->list[ingredients->n_ingredients - 1]));
                ingredients->n_ingredients--;
                break;
            }
        }
    }
}

static allergen_t* get_allergen(allergen_t allergens[MAX_NUM_ALLERGENS], size_t* n_allergens, char* allergen_name)
{
    for(size_t allergen_idx = 0; allergen_idx < *n_allergens; ++allergen_idx) {
        if(strlen(allergens[allergen_idx].allergen_name) == strlen(allergen_name) && strncmp(allergens[allergen_idx].allergen_name, allergen_name, MAX_ALLERGEN_LEN) == 0) {
            return &allergens[allergen_idx];
        }
    }
    allergen_t* allergen = &allergens[(*n_allergens)++];
    memcpy(allergen->allergen_name, allergen_name, strlen(allergen_name));
    return allergen;
}

static void filter_ingredients(ingredients_t* ingredients, char list[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN], size_t list_len)
{
    for(size_t i = 0; i < ingredients->n_ingredients;) {
        char* ingredient = ingredients->list[i];
        bool match_found = false;
        for(size_t j = 0; j < list_len; ++j) {
            char* list_ingredient = list[j];
            if(strlen(ingredient) == strlen(list_ingredient) && strncmp(ingredient, list_ingredient, strlen(ingredient)) == 0) {
                match_found = true;
                break;
            }
        }
        if(match_found) {
            i++;
        }
        else {
            memset(ingredient, 0, strlen(ingredient));
            memcpy(ingredient, ingredients->list[ingredients->n_ingredients - 1], strlen(ingredients->list[ingredients->n_ingredients - 1]));
            memset(ingredients->list[ingredients->n_ingredients - 1], 0, strlen(ingredients->list[ingredients->n_ingredients - 1]));
            ingredients->n_ingredients--;
        }
    }
}

static void rev_filter_ingredients(ingredients_t* ingredients, char list[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN], size_t list_len)
{
    for(size_t i = 0; i < ingredients->n_ingredients;) {
        char* ingredient = ingredients->list[i];
        bool match_found = false;
        for(size_t j = 0; j < list_len; ++j) {
            char* list_ingredient = list[j];
            if(strlen(ingredient) == strlen(list_ingredient) && strncmp(ingredient, list_ingredient, strlen(ingredient)) == 0) {
                match_found = true;
                break;
            }
        }
        if(!match_found) {
            i++;
        }
        else {
            memset(ingredient, 0, strlen(ingredient));
            memcpy(ingredient, ingredients->list[ingredients->n_ingredients - 1], strlen(ingredients->list[ingredients->n_ingredients - 1]));
            memset(ingredients->list[ingredients->n_ingredients - 1], 0, strlen(ingredients->list[ingredients->n_ingredients - 1]));
            ingredients->n_ingredients--;
        }
    }
}

static void eliminate_ambiguities(allergen_t allergens[MAX_NUM_ALLERGENS], size_t n_allergens)
{
    bool change_occurred = true;
    while(change_occurred) {
        change_occurred = false;
        for(size_t i = 0; i < n_allergens; ++i) {
            allergen_t* allergen = &allergens[i];
            if(allergen->ingredients.n_ingredients == 1) {
                char* ingredient_name = allergen->ingredients.list[0];
                for(size_t j = 0; j < n_allergens; ++j) {
                    if(i == j) {
                        continue;
                    }
                    allergen_t* other_allergen = &allergens[j];
                    ingredients_t* other_ingredients = &other_allergen->ingredients;
                    for(size_t k = 0; k < other_ingredients->n_ingredients;) {
                        char* other_ingredient_name = other_allergen->ingredients.list[k];
                        const bool match = strlen(ingredient_name) == strlen(other_ingredient_name) && strncmp(ingredient_name, other_ingredient_name, strlen(ingredient_name)) == 0;
                        if(match) {
                            memset(other_ingredient_name, 0, strlen(other_ingredient_name));
                            memcpy(other_ingredient_name, other_ingredients->list[other_ingredients->n_ingredients - 1], strlen(other_ingredients->list[other_ingredients->n_ingredients - 1]));
                            memset(other_ingredients->list[other_ingredients->n_ingredients - 1], 0, strlen(other_ingredients->list[other_ingredients->n_ingredients - 1]));
                            other_ingredients->n_ingredients--;
                            change_occurred = true;
                        }
                        else {
                            k++;
                        }
                    }
                }
            }
        }
    }
}

static void add_ingredients_to_allergen(allergen_t* allergen, char ingredients[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN], size_t n_ingredients)
{
    if(allergen->ingredients.n_ingredients == 0) {
        for(size_t ingredient_idx = 0; ingredient_idx < n_ingredients; ++ingredient_idx) {
            char* ingredient = ingredients[ingredient_idx];
            memcpy(allergen->ingredients.list[allergen->ingredients.n_ingredients++], ingredient, strlen(ingredient));
        }
        ingredients_make_unique(&allergen->ingredients);
    }
    else if(allergen->ingredients.n_ingredients > 1) {
        filter_ingredients(&allergen->ingredients, ingredients, n_ingredients);
    }
}

static size_t solve_part_1(const char* const input, size_t size)
{
    size_t ans = 0;

    memset(allergens, 0, MAX_NUM_ALLERGENS * sizeof(allergen_t));
    n_allergens = 0;
    memset(&all, 0, sizeof(ingredient_counter_t));
    memset(&non_allergenic, 0, sizeof(ingredients_t));

    memory_reference_t* line_mem_refs = NULL;
    size_t n_lines = 0;
    split_buf_by_sequence(input, size, "\n", &line_mem_refs, &n_lines);

    for(size_t line_idx = 0; line_idx < n_lines; ++line_idx) {
        const char* const line = &input[line_mem_refs[line_idx].offset];
        const size_t line_len = line_mem_refs[line_idx].size;
        char line_allergens[MAX_NUM_ALLERGENS][MAX_ALLERGEN_LEN] = {0};
        size_t n_line_allergens = 0;
        char line_ingredients[MAX_NUM_INGREDIENTS][MAX_INGREDIENT_LEN] = {0};
        size_t n_line_ingredients = 0;

        extract_line_data(line, line_len, line_allergens, &n_line_allergens, line_ingredients, &n_line_ingredients);

        for(size_t line_allergen_idx = 0; line_allergen_idx < n_line_allergens; ++line_allergen_idx) {
            char* allergen_name = line_allergens[line_allergen_idx];
            allergen_t* allergen = get_allergen(allergens, &n_allergens, allergen_name);
            add_ingredients_to_allergen(allergen, line_ingredients, n_line_ingredients);
        }

        for(size_t i = 0; i < n_line_ingredients; ++i) {
            char* line_ingredient_name = line_ingredients[i];
            assert(non_allergenic.n_ingredients <= MAX_NUM_INGREDIENTS);
            memcpy(non_allergenic.list[non_allergenic.n_ingredients++], line_ingredient_name, strlen(line_ingredient_name));

            bool extant_ingred_found = false;
            for(size_t j = 0; j < all.n_ingredients; ++j) {
                if(strlen(all.list[j]) == strlen(line_ingredient_name) && strncmp(all.list[j], line_ingredient_name, strlen(line_ingredient_name)) == 0) {
                    extant_ingred_found = true;
                    all.count[j]++;
                    break;
                }
            }
            if(!extant_ingred_found) {
                all.count[all.n_ingredients] = 1;
                memcpy(all.list[all.n_ingredients++], line_ingredient_name, strlen(line_ingredient_name));
            }
        }
        ingredients_make_unique(&non_allergenic);
    }

    eliminate_ambiguities(allergens, n_allergens);
    for(size_t i = 0; i < n_allergens; ++i) {
        assert(allergens[i].ingredients.n_ingredients == 1);
        // printf("%s,%s,\n", allergens[i].allergen_name, allergens[i].ingredients.list[0]);
        rev_filter_ingredients(&non_allergenic, allergens[i].ingredients.list, allergens[i].ingredients.n_ingredients);
    }

    for(size_t i = 0; i < non_allergenic.n_ingredients; ++i) {
        char* non_allergenic_ingredient_name = non_allergenic.list[i];
        for(size_t j = 0; j < all.n_ingredients; ++j) {
            char* ingredient_name = all.list[j];
            if(strlen(ingredient_name) == strlen(non_allergenic_ingredient_name) && strncmp(ingredient_name, non_allergenic_ingredient_name, strlen(non_allergenic_ingredient_name)) == 0) {
                ans += all.count[j];
                break;
            }
        }
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

    TEST_ASSERT_EQUAL_size_t(5, ans);
}

void day_21_part_1_problem()
{
    char* input = NULL;
    size_t size = 0;

    const bool success = read_file_into_buf("../data/day_21_part_1_input.txt", &input, &size);
    TEST_ASSERT_TRUE(success);

    const size_t ans = solve_part_1(input, size);

    free(input);

    TEST_ASSERT_EQUAL_size_t(2595, ans);
}

