# CMakeCPPTemplate

A C++ project template with CMake, Catch2 testing, and CI/CD support for Linux and Windows.

## Using This Template

### Option 1: GitHub UI
Click "Use this template" button on GitHub to create a new repository.

### Option 2: GitHub CLI
```bash
gh repo create MyNewProject --template JensMunkHansen/CMakeCPPTemplate --public --clone
cd MyNewProject
```

### After Creating Your Project
1. Rename `Hello` folder to your project name
2. Rename `HelloTest` folder to `${YourProject}Test`
3. Update `project(Hello ...)` in `CMakeLists.txt` to your project name
4. Update source files (`Hello.h`, `Hello.cpp`, `HelloTest.cpp`) with your project name
5. Update the namespace in source files if desired
6. Update `Examples/Usage/` to use your new library name

## Features

- CMake build system with presets (Ninja Multi-Config)
- Catch2 testing framework
- Support for both static and shared library builds
- AddressSanitizer (Asan) support
- CI/CD with GitHub Actions (Linux + Windows)
- Automatic dependency caching
- Examples with runtime DLL copying on Windows

## Project Structure

```
.
├── CMakeLists.txt          # Main CMake configuration
├── CMakePresets.json       # CMake presets for different build configurations
├── versions.txt            # Dependency versions
├── dependencies.sh         # Linux dependency builder
├── build_dependencies.bat  # Windows dependency builder (multi-toolset)
├── Hello/                  # Main library (rename to your project)
│   ├── CMakeLists.txt
│   ├── Hello.h
│   └── Hello.cpp
├── HelloTest/              # Unit tests (rename to ${PROJECT_NAME}Test)
│   ├── CMakeLists.txt
│   └── HelloTest.cpp
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
