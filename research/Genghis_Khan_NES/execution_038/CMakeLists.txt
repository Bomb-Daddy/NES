cmake_minimum_required(VERSION 3.16)
project(gk1_stage_window C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_executable(test_gk1_stage_window
    test_gk1_stage_window.c gk1_stage_window.c
    ../execution_034/gk1_tactical_slice.c
    ../execution_035/gk1_native_ram_view.c)
enable_testing()
add_test(NAME stage_window COMMAND test_gk1_stage_window)
