cmake_minimum_required(VERSION 3.22)
project(Lab4 C)

set(CMAKE_C_STANDARD 23)

add_executable(T1 T1/main.c structs.c structs.h)
add_executable(T2 T2/main.c structs.c structs.h)

target_link_libraries(T2 m)