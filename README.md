# PerformanceCounters

A C++ library for performance counter utilities with CMake, Catch2 testing, and CI/CD support for Linux and Windows.

## Features

- CMake build system with presets (Ninja Multi-Config)
- Catch2 testing framework
- Support for both static and shared library builds
- AddressSanitizer (Asan) support
- CI/CD with GitHub Actions (Linux + Windows)
- Automatic dependency caching

## Project Structure

```
.
├── CMakeLists.txt          # Main CMake configuration
├── CMakePresets.json       # CMake presets for different build configurations
├── versions.txt            # Dependency versions
├── dependencies.sh         # Linux dependency builder
├── build_dependencies.bat  # Windows dependency builder (multi-toolset)
├── PerformanceCounters/    # Main library
│   ├── CMakeLists.txt
│   ├── PerformanceCounters.h
│   └── PerformanceCounters.cpp
├── PerformanceCountersTest/ # Unit tests
│   ├── CMakeLists.txt
│   └── PerformanceCountersTest.cpp
├── Examples/               # Usage examples
│   └── Usage/
├── NativeDeps/             # Native dependency builder (Catch2)
│   └── CMakeLists.txt
├── CMake/                  # CMake modules (git submodule)
└── .github/                # GitHub Actions CI/CD
    ├── actions/            # Reusable composite actions
    └── workflows/          # CI workflows
```

## Quick Start

### Prerequisites

- CMake 3.22+
- C++17 compiler (GCC, Clang, or MSVC)
- Ninja build system

### Build Dependencies

```bash
# Linux
./dependencies.sh --compiler=gcc

# Windows (builds all toolsets: msvc, clangcl, msvc-asan)
set Dependencies_ROOT=%CD%\deps
build_dependencies.bat msvc
```

### Configure and Build

```bash
# Linux with GCC
cmake --preset linux-gcc
cmake --build build/linux-gcc --config Release

# Linux with Clang
cmake --preset linux-clang
cmake --build build/linux-clang --config Release

# Windows with MSVC
cmake --preset windows-msvc
cmake --build build/windows-msvc --config Release
```

### Run Tests

```bash
# Linux
ctest --preset core-test --test-dir build/linux-gcc -C Release

# Windows
ctest --test-dir build/windows-msvc -C Release --output-on-failure
```

## CI/CD

Both Linux and Windows workflows support:
- Automatic builds on push/PR to main
- Manual workflow dispatch with options:
  - Build mode: single or all configurations
  - Library type: STATIC or SHARED
  - Build type: Debug, Release, or Asan
  - Compiler selection
- Dependency caching for faster builds
- Automatic cancellation of in-progress builds on new push

## License

See LICENSE file.
