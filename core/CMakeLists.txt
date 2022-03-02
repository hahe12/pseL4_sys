cmake_minimum_required(VERSION 3.7.2)
# declare the hello-world CMake project and the languages it is written in (just C)
project(core C ASM)

#list of apps to include in the cpio archive
get_property(cpio_apps GLOBAL PROPERTY apps_property)
include(cpio)
MakeCPIO(archive.o "${cpio_apps}")

file(GLOB deps src/*.c src/*/*.c src/*/*/*.c)
list(SORT deps)

add_executable(core ${deps} archive.o)
target_include_directories(core PRIVATE include)

# List of libraries to link with the application.
target_link_libraries(core
    muslc 
    sel4runtime 
    sel4muslcsys
    sel4platsupport 
    sel4allocman
    cpio
    sel4vka
    utils
)

# Tell the build system that this application is the root task. 
include(rootserver)
DeclareRootserver(core)

