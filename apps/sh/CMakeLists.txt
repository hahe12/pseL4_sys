cmake_minimum_required(VERSION 3.7.2)

project(sh C)

add_executable(sh src/main.c)

target_link_libraries(sh sel4runtime sel4 muslc sel4muslcsys utils c)

set_property(GLOBAL APPEND PROPERTY apps_property "$<TARGET_FILE:sh>")