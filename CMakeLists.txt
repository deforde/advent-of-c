cmake_minimum_required(VERSION 3.13)
project(advent-of-c C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -pedantic -Werror -Werror=vla)
# add_compile_options(-fsanitize=undefined)

add_executable(advent-of-c "")

add_library(unity STATIC $ENV{TTS_UNITY_DIR}/src/unity.c)
include_directories(unity PUBLIC $ENV{TTS_UNITY_DIR}/src)

add_subdirectory(src)
add_subdirectory(src/aoc_2020)

# target_link_libraries(advent-of-c unity m -fsanitize=undefined)
target_link_libraries(advent-of-c unity m)

