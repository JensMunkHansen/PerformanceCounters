#!/bin/bash
# Install git hooks for this repository

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
HOOKS_DIR="$SCRIPT_DIR/hooks"
GIT_HOOKS_DIR="$SCRIPT_DIR/.git/hooks"

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo "Error: Not in a git repository root"
    exit 1
fi

# Check if hooks directory exists
if [ ! -d "$HOOKS_DIR" ]; then
    echo "Error: hooks directory not found"
    exit 1
fi

echo "Installing git hooks..."

# Install pre-commit hook
if [ -f "$HOOKS_DIR/pre-commit" ]; then
    cp "$HOOKS_DIR/pre-commit" "$GIT_HOOKS_DIR/pre-commit"
    chmod +x "$GIT_HOOKS_DIR/pre-commit"
    echo "  ✓ Installed pre-commit hook"
else
    echo "  ✗ pre-commit hook not found in hooks/"
fi

echo ""
echo "Git hooks installed successfully!"
echo ""
echo "Note: The pre-commit hook requires clang-format to be installed."
echo "      Install it using your package manager if not already available."
