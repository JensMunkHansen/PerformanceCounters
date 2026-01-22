#!/bin/bash

set -e

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Extract project name from CMakeLists.txt
PROJECT_NAME=$(grep -oP '(?<=^project\()[^[:space:])]+' "$SCRIPT_DIR/CMakeLists.txt" | head -1)
if [ -z "$PROJECT_NAME" ]; then
    echo "ERROR: Could not extract project name from CMakeLists.txt"
    exit 1
fi

# Default compiler
COMPILER_CHOICE="gcc"
KEEP_BUILD=true

# Help function
show_help() {
    cat << EOF
${PROJECT_NAME} Dependencies Builder

Usage: ./dependencies.sh [OPTIONS] [CMAKE_ARGS...]

Options:
  --help, -h            Show this help message
  --compiler=<gcc|clang> Select compiler (default: gcc)
  --delete-build        Delete build directory after install (default: keep)

Optional CMake flags (pass directly):
  -DDEPS_VERBOSE=ON     Show detailed build output
  -DBUILD_SHARED_LIBS=ON  Build shared libraries (.so) instead of static (.a)

Examples:
  ./dependencies.sh
      Build with GCC, default options

  ./dependencies.sh --compiler=clang
      Build with Clang

  ./dependencies.sh --delete-build
      Build and remove build directory after install

EOF
    exit 0
}

# Parse arguments
CMAKE_ARGS=()
while [[ $# -gt 0 ]]; do
    case $1 in
        --help|-h)
            show_help
            ;;
        --compiler)
            COMPILER_CHOICE="$2"
            shift 2
            ;;
        --compiler=*)
            COMPILER_CHOICE="${1#*=}"
            shift
            ;;
        --keep-build)
            KEEP_BUILD=true
            shift
            ;;
        --delete-build)
            KEEP_BUILD=false
            shift
            ;;
        *)
            CMAKE_ARGS+=("$1")
            shift
            ;;
    esac
done

# Set compiler based on choice
case "$COMPILER_CHOICE" in
    gcc)
        CC_COMPILER="gcc"
        CXX_COMPILER="g++"
        ;;
    clang)
        CC_COMPILER="clang"
        CXX_COMPILER="clang++"
        ;;
    *)
        echo "ERROR: Unknown compiler '$COMPILER_CHOICE'. Use 'gcc' or 'clang'."
        exit 1
        ;;
esac

# Detect platform (use CMake-style capitalized platform names)
PLATFORM="$(uname -s)"  # Linux, Darwin, Windows

INSTALL_DIR="${SCRIPT_DIR}/${PLATFORM}/${COMPILER_CHOICE}/install"
BUILD_DIR="${SCRIPT_DIR}/${PLATFORM}/${COMPILER_CHOICE}/build"

# Set project root environment variable (NativeDeps expects Dependencies_ROOT)
export Dependencies_ROOT="${SCRIPT_DIR}"

echo "=============================================="
echo "${PROJECT_NAME} Dependencies Builder"
echo "=============================================="
echo ""
echo "Platform:          $PLATFORM"
echo "Compiler:          $COMPILER_CHOICE ($CC_COMPILER / $CXX_COMPILER)"
echo "Install directory: $INSTALL_DIR"
echo "Build directory:   $BUILD_DIR"
echo ""

# Create directories
mkdir -p "$INSTALL_DIR" "$BUILD_DIR"

# Configure NativeDeps
echo "Configuring dependencies..."
cmake -S "$SCRIPT_DIR/NativeDeps" -B "$BUILD_DIR" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER="${CC_COMPILER}" \
    -DCMAKE_CXX_COMPILER="${CXX_COMPILER}" \
    -DBUILD_CATCH2=ON \
    -DDEPS_VERBOSE=OFF \
    "${CMAKE_ARGS[@]}"

# Build all dependencies
echo ""
echo "Building dependencies..."
cmake --build "$BUILD_DIR" --config Release --parallel

# Clean up build directory to save space
if [ "$KEEP_BUILD" = "false" ]; then
    echo ""
    echo "Cleaning up build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    echo "Build directory removed."
fi

echo ""
echo "=============================================="
echo "Dependencies installed successfully!"
echo "=============================================="
echo ""
echo "To use these dependencies, configure your project with:"
echo "  cmake --preset linux-${COMPILER_CHOICE}"
echo ""
echo "Or set CMAKE_PREFIX_PATH to: $INSTALL_DIR"
echo ""
