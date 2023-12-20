cmake_minimum_required(VERSION 3.22)
project(Lab4 C)

set(CMAKE_C_STANDARD 23)

add_executable(T1 T1/main.c structs.c structs.h)
add_executable(T2 T2/main.c structs.c structs.h)
add_executable(T5 T5/main.c T5/expression_process.c T5/expression_process.h T5/structs.h T5/structs.c structs.c structs.h)
add_executable(T7 T7/main.c structs.c structs.h)

target_link_libraries(T2 m)
target_link_libraries(T5 m)