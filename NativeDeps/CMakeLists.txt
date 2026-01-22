cmake_minimum_required(VERSION 3.22...3.28)

project(Dependencies LANGUAGES C CXX)

# Add CMake module path for functionalities
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../CMake")

# Require ${PROJECT_NAME}_ROOT environment variable (for reference in messages)
# CMAKE_INSTALL_PREFIX should be passed from command line (e.g., by dependencies.sh)
if(DEFINED ENV{${PROJECT_NAME}_ROOT})
  set(${PROJECT_NAME}_ROOT "$ENV{${PROJECT_NAME}_ROOT}")
  message(STATUS "${PROJECT_NAME}_ROOT: ${${PROJECT_NAME}_ROOT}")
  message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
else()
  message(FATAL_ERROR "${PROJECT_NAME}_ROOT environment variable not set. "
                      "Set it to the directory where dependencies should be built/installed.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(ExternalProject)
include(spsSBOM)

# Control verbosity of ExternalProject builds
option(DEPS_VERBOSE "Enable verbose output for dependency builds" OFF)
if(NOT DEPS_VERBOSE)
  set(EP_LOG_LEVEL OFF)
else()
  set(EP_LOG_LEVEL ON)
endif()

# Version definitions (from versions.txt or fallback)
sps_get_version(CATCH2_VERSION "3.5.2")

# Build options
option(BUILD_CATCH2 "Build and deploy Catch2 test framework" ON)

# Build shared libraries (reduces binary size but requires .so at runtime)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)

# Forward ASan flags to ExternalProject builds if they are set
set(ASAN_CMAKE_ARGS "")
if(CMAKE_CXX_FLAGS MATCHES "/fsanitize=address" OR CMAKE_C_FLAGS MATCHES "/fsanitize=address")
  message(STATUS "ASan flags detected - forwarding to ExternalProject builds")
  list(APPEND ASAN_CMAKE_ARGS
    "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
    "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
    "-DCMAKE_CONFIGURATION_TYPES=Asan;Release;Debug"
  )
  if(CMAKE_EXE_LINKER_FLAGS)
    list(APPEND ASAN_CMAKE_ARGS "-DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}")
  endif()
  if(CMAKE_SHARED_LINKER_FLAGS)
    list(APPEND ASAN_CMAKE_ARGS "-DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}")
  endif()
endif()

#------------------------------------------------------------------------------
# Catch2 - Testing framework
#------------------------------------------------------------------------------
if(BUILD_CATCH2)
  message(STATUS "Will build Catch2 ${CATCH2_VERSION}")
  ExternalProject_Add(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v${CATCH2_VERSION}
    GIT_SHALLOW    TRUE
    UPDATE_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
      -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCATCH_INSTALL_DOCS=OFF
      ${ASAN_CMAKE_ARGS}
    LOG_BUILD ${EP_LOG_LEVEL}
    LOG_CONFIGURE ${EP_LOG_LEVEL}
    LOG_INSTALL ${EP_LOG_LEVEL}
  )
endif()
