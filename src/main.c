#include <unity.h>

#include "aoc_2020/day_01.h"
#include "aoc_2020/day_02.h"
#include "aoc_2020/day_03.h"
#include "aoc_2020/day_04.h"
#include "aoc_2020/day_05.h"
#include "aoc_2020/day_06.h"
#include "aoc_2020/day_07.h"
#include "aoc_2020/day_08.h"
#include "aoc_2020/day_09.h"
#include "aoc_2020/day_10.h"

void setUp(void)
{
}

void tearDown(void)
{
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(day_01_part_1_example);
    RUN_TEST(day_01_part_1_problem);
    RUN_TEST(day_01_part_2_example);
    RUN_TEST(day_01_part_2_problem);

    RUN_TEST(day_02_part_1_example);
    RUN_TEST(day_02_part_1_problem);
    RUN_TEST(day_02_part_2_example);
    RUN_TEST(day_02_part_2_problem);

    RUN_TEST(day_03_part_1_example);
    RUN_TEST(day_03_part_1_problem);
    RUN_TEST(day_03_part_2_example);
    RUN_TEST(day_03_part_2_problem);

    RUN_TEST(day_04_part_1_example);
    RUN_TEST(day_04_part_1_problem);
    RUN_TEST(day_04_part_2_example_1);
    RUN_TEST(day_04_part_2_example_2);
    RUN_TEST(day_04_part_2_problem);

    RUN_TEST(day_05_part_1_example);
    RUN_TEST(day_05_part_1_problem);
    RUN_TEST(day_05_part_2_problem);

    RUN_TEST(day_06_part_1_example);
    RUN_TEST(day_06_part_1_problem);
    RUN_TEST(day_06_part_2_example);
    RUN_TEST(day_06_part_2_problem);

    RUN_TEST(day_07_part_1_example);
    RUN_TEST(day_07_part_1_problem);
    RUN_TEST(day_07_part_2_example);
    RUN_TEST(day_07_part_2_problem);

    RUN_TEST(day_08_part_1_example);
    RUN_TEST(day_08_part_1_problem);
    RUN_TEST(day_08_part_2_example);
    RUN_TEST(day_08_part_2_problem);

    RUN_TEST(day_09_part_1_example);
    RUN_TEST(day_09_part_1_problem);
    RUN_TEST(day_09_part_2_example);
    RUN_TEST(day_09_part_2_problem);

    RUN_TEST(day_10_part_1_example);
    RUN_TEST(day_10_part_1_problem);
    RUN_TEST(day_10_part_2_example);
    RUN_TEST(day_10_part_2_problem);

    return UNITY_END();
}
