@echo off
REM Install git hooks for this repository

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "HOOKS_DIR=%SCRIPT_DIR%hooks"
set "GIT_HOOKS_DIR=%SCRIPT_DIR%.git\hooks"

REM Check if we're in a git repository
if not exist "%SCRIPT_DIR%.git" (
    echo Error: Not in a git repository root
    exit /b 1
)

REM Check if hooks directory exists
if not exist "%HOOKS_DIR%" (
    echo Error: hooks directory not found
    exit /b 1
)

echo Installing git hooks...

REM Install pre-commit hook
if exist "%HOOKS_DIR%\pre-commit" (
    copy /Y "%HOOKS_DIR%\pre-commit" "%GIT_HOOKS_DIR%\pre-commit" >nul
    echo   [OK] Installed pre-commit hook
) else (
    echo   [X] pre-commit hook not found in hooks/
)

echo.
echo Git hooks installed successfully!
echo.
echo Note: The pre-commit hook requires clang-format to be installed.
echo       Install it using your package manager if not already available.
