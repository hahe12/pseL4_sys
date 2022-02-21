cmake_minimum_required(VERSION 3.7.2)

project(init C)

add_executable(init src/main.c)

target_link_libraries(init sel4runtime sel4 muslc utils
c)

set_property(GLOBAL APPEND PROPERTY apps_property "$<TARGET_FILE:init>")