cmake_minimum_required(VERSION 3.7.2)
# declare the hello-world CMake project and the languages it is written in (just C)
project(pseL4 C ASM)

# In future tutorials, these setup steps will be replaced with
# sel4_tutorials_setup_roottask_tutorial_environment()
find_package(seL4 REQUIRED)
find_package(elfloader-tool REQUIRED)
find_package(musllibc REQUIRED)
find_package(util_libs REQUIRED)
find_package(seL4_libs REQUIRED)

sel4_import_kernel()
elfloader_import_project()

# This sets up environment build flags and imports musllibc and runtime libraries.
musllibc_setup_build_environment_with_sel4runtime()
sel4_import_libsel4()
util_libs_import_libraries()
sel4_libs_import_libraries()
sel4_tutorials_import_libsel4tutorials()

# Name the executable and list source files required to build it
add_executable(pseL4 src/main.c)

# List of libraries to link with the application.
target_link_libraries(pseL4
    sel4runtime sel4
    muslc utils sel4tutorials
    sel4muslcsys sel4platsupport sel4utils sel4debug)

# Tell the build system that this application is the root task. 
include(rootserver)
DeclareRootserver(pseL4)

include(simulation)
GenerateSimulateScript()

