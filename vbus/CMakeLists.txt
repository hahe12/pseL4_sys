cmake_minimum_required(VERSION 3.7.2)

project(vbus C)

file(GLOB deps src/*.c)
list(SORT deps)

add_library(vbus STATIC ${deps})

target_include_directories(vbus PUBLIC include)

target_link_libraries(vbus
        sel4_autoconf
        sel4
        muslc
        sel4allocman
        sel4vka
)