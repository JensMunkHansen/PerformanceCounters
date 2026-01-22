cmake_minimum_required(VERSION 3.22...3.28)

project(Hello
  VERSION 1.0.0
  DESCRIPTION "Template project with Hello library"
  LANGUAGES CXX
)

# Add CMake module path for shared utilities
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Core includes
include(spsCompileCommands)
include(spsInstallDirs)
include(spsTargets)

# Create interface library for compiler flags
add_library(build INTERFACE)

# Compiler/platform-specific flags
include(spsCompilerPlatformFlags)

# AddressSanitizer support (adds Asan configuration type)
include(spsAsan)

# Build options
option(BUILD_TESTING "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Detect CI environment
if(DEFINED ENV{GITHUB_ACTIONS})
  set(${PROJECT_NAME}_CI_BUILD ON)
else()
  set(${PROJECT_NAME}_CI_BUILD OFF)
endif()

# Main library
add_subdirectory(${PROJECT_NAME})

# Tests
if(BUILD_TESTING)
  include(spsTesting)
  add_subdirectory(${PROJECT_NAME}Test)
endif()

# Examples
if(BUILD_EXAMPLES)
  # Set variables needed by Examples
  set(${PROJECT_NAME}_BINARY_DIR "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")
  set(${PROJECT_NAME}_INSTALL_DIR "${CMAKE_BINARY_DIR}/install-test" CACHE INTERNAL "")
  add_subdirectory(Examples)
endif()

# Optional documentation
option(BUILD_DOCUMENTATION "Build API documentation" OFF)
if(BUILD_DOCUMENTATION)
  include(spsDoxygen)
  sps_setup_doxygen(
    TARGET doc
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc"
    INPUT_DIRECTORIES "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}"
  )
endif()
