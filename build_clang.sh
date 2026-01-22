#!/bin/bash

set -e

# Default to Release if no argument is provided
CONFIG="${1:-Release}"

# Validate input
if [[ "$CONFIG" != "Release" && "$CONFIG" != "Debug" ]]; then
    echo "Usage: $0 [Release|Debug]"
    exit 1
fi

export CC=clang
export CXX=clang++

bear -- sh -c "cmake --preset linux-clang"
bear -- sh -c "cmake --build build/linux-clang --config $CONFIG"
bear -- sh -c "ctest --test-dir build/linux-clang -C $CONFIG"
