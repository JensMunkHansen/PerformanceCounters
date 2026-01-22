#!/bin/bash

export CC=gcc
export CXX=g++

cmake --preset linux-gcc
cmake --build build/linux-gcc --config Debug
ctest --test-dir build/linux-gcc -C Debug
