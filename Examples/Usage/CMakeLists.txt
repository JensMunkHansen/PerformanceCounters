cmake_minimum_required(VERSION 3.25...3.31)

# Note: When customizing this template, change "Hello" to your project name
project(HelloUsage LANGUAGES CXX)

# Add CMake module path for shared utilities
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../CMake)

# Create interface library for compiler flags
add_library(build INTERFACE)

# Compiler/platform-specific flags
include(spsCompilerPlatformFlags)

# AddressSanitizer support (adds Asan configuration type)
include(spsAsan)

# Target utilities (for DLL copying)
include(spsTargets)

# Find the main library
find_package(Hello CONFIG REQUIRED)

# Main usage example
add_executable(HelloUsage main.cpp)
target_link_libraries(HelloUsage PRIVATE Hello::Hello build)

# Copy runtime DLLs on Windows when using shared libraries
sps_copy_runtime_dlls(HelloUsage)
