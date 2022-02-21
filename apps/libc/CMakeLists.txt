cmake_minimum_required(VERSION 3.7.2)

project(libc C)

file(GLOB deps src/*.c)
list(SORT deps)

add_library(c STATIC ${deps})

target_include_directories(c PUBLIC include)

target_link_libraries(c
        sel4_autoconf
        sel4
        muslc
        sel4allocman
        sel4vka
        vbus
)