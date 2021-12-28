#include <unity.h>

#include "aoc_2020/day_01.h"
#include "aoc_2020/day_02.h"
#include "aoc_2020/day_03.h"

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

    return UNITY_END();
}
