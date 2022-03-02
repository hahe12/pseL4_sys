cmake_minimum_required(VERSION 3.7.2)

project(psel4muslcsys C)

file(GLOB deps src/*.c)
list(SORT deps)

add_library(psel4muslcsys STATIC ${deps})

target_include_directories(psel4muslcsys PUBLIC include)

target_link_libraries(psel4muslcsys
        muslc
)