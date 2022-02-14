#!/bin/sh
#
# Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

# This script is intended to be symlinked into the same location as your root
# CMakeLists.txt file and then invoked from a clean build directory.

set -eu

# Determine path to this script (fast, cheap "dirname").
SCRIPT_PATH=${0%/*}
# Save script name for diagnostic messages (fast, cheap "basename").
SCRIPT_NAME=${0##*/}

# Ensure script path and current working directory are not the same.
if [ "$PWD" = "$SCRIPT_PATH" ]
then
    echo "\"$SCRIPT_NAME\" should not be invoked from top-level directory" >&2
    exit 1
fi

if [ -d "$HOME/.sel4_cache" ]
then
    CACHE_DIR="$HOME/.sel4_cache"
else
    CACHE_DIR="$SCRIPT_PATH/.sel4_cache"
fi

# If we have a CMakeLists.txt in the top level project directory,
# initialize CMake.
cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_PATH"/kernel/gcc.cmake -G Ninja "$@" \
    -DCROSS_COMPILER_PREFIX=x86_64-linux- -DSEL4_CACHE_DIR="$CACHE_DIR" -DTUT_BOARD="pc" -DTUT_ARCH="x86_64" "$SCRIPT_PATH/projects/pseL4_sys"
