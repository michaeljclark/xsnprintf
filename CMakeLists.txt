cmake_minimum_required (VERSION 3.12)

project(xsnprintf)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CheckCCompilerFlag)
check_c_compiler_flag(-Wa,-mbranches-within-32B-boundaries has_branch_32b)
check_c_compiler_flag(-march=x86-64-v3 has_arch_x86_64_v3)
check_c_compiler_flag(-g has_debug)
check_c_compiler_flag(-O3 has_o3)

if(has_branch_32b)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-mbranches-within-32B-boundaries")
endif()

if(has_arch_x86_64_v3)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=x86-64-v3")
endif()

if (has_o3)
set(CMAKE_C_FLAGS_RELEASE "-O3")
endif()

if (has_debug AND has_o3)
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -g")
endif()

add_library(xsnprintf STATIC xsnprintf.c)
add_executable(xsnprintf_test xsnprintf_test.c)
add_executable(xsnprintf_bench xsnprintf_bench.c)
target_link_libraries(xsnprintf_test xsnprintf)
target_link_libraries(xsnprintf_bench xsnprintf)
