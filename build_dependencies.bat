@echo off
setlocal

REM build_dependencies.bat - Build dependencies for project on Windows
REM Builds Catch2 for specified toolset

REM Check if Visual Studio environment is already set
IF NOT DEFINED VCINSTALLDIR (
    echo Calling vcvarsall.bat for x64...
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
    if errorlevel 1 (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
    )
    if errorlevel 1 (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    )
) ELSE (
    echo Visual Studio environment already configured.
)

:: ================================================
:: Check environment
:: ================================================
if "%Dependencies_ROOT%"=="" (
    echo ERROR: Environment variable Dependencies_ROOT is not set.
    exit /b 1
)

:: ================================================
:: Toolsets and configurations
:: ================================================
set DEP_DIR=%CD%\NativeDeps

:: ================================================
:: Check which toolchain to build based on arguments
:: ================================================
set TOOLSET=%1
set CLEAN_BUILD=%2
if "%TOOLSET%"=="" (
    echo ERROR: Toolset must be specified as first argument
    echo Usage: %0 [msvc^|clangcl^|msvc-asan] [--clean]
    exit /b 1
)

:: ================================================
:: Build dependencies for specified toolset
:: ================================================
call :BuildOne %TOOLSET% %CLEAN_BUILD%

goto :EOF

:: ================================================
:: Function: BuildOne <toolset> [clean]
:: Builds configurations based on toolset type
:: ================================================
:BuildOne
setlocal
set TOOLSET=%1
set CLEAN_BUILD=%2

echo.
echo ================================================
echo Building dependencies for %TOOLSET%
echo ================================================

REM Set configurations based on toolset
if "%TOOLSET%"=="msvc-asan" (
    set CONFIGURATIONS=Asan
    set CONFIG_DESC=Asan only
) else (
    set CONFIGURATIONS=Debug Release
    set CONFIG_DESC=Debug + Release
)

echo Building %CONFIG_DESC% configurations for %TOOLSET%

REM Set install prefix and build directory
set INSTALL_PREFIX=%Dependencies_ROOT%\windows\%TOOLSET%\install
set BUILD_DIR=%Dependencies_ROOT%\windows\%TOOLSET%\cache

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
pushd "%BUILD_DIR%"

REM Configure CMake based on toolset
echo -- Configuring Dependencies for %TOOLSET% --

if "%TOOLSET%"=="msvc-asan" (
    REM MSVC ASan configuration
    call cmake "%DEP_DIR%" -G "Ninja Multi-Config" ^
        -DCMAKE_INSTALL_PREFIX="%INSTALL_PREFIX%" ^
        -DCMAKE_CONFIGURATION_TYPES="Asan;Release;Debug" ^
        -DCMAKE_CXX_FLAGS="/EHsc /MD /Zi /fsanitize=address" ^
        -DCMAKE_C_FLAGS="/fsanitize=address" ^
        -DCMAKE_EXE_LINKER_FLAGS="/INFERASANLIBS" ^
        -DCMAKE_SHARED_LINKER_FLAGS="/INFERASANLIBS"
) else (
    REM Standard MSVC or CLANGCL configuration - Debug + Release
    if "%TOOLSET%"=="clangcl" (
        call cmake "%DEP_DIR%" -G "Ninja Multi-Config" ^
            -DCMAKE_INSTALL_PREFIX="%INSTALL_PREFIX%" ^
            -DCMAKE_C_COMPILER=clang-cl ^
            -DCMAKE_CXX_COMPILER=clang-cl ^
            -DCMAKE_CONFIGURATION_TYPES="Debug;Release"
    ) else (
        call cmake "%DEP_DIR%" -G "Ninja Multi-Config" ^
            -DCMAKE_INSTALL_PREFIX="%INSTALL_PREFIX%" ^
            -DCMAKE_CONFIGURATION_TYPES="Debug;Release"
    )
)

if errorlevel 1 (
    echo ERROR: Configure failed for %TOOLSET%
    popd
    exit /b 1
)

REM Build each configuration
for %%C in (%CONFIGURATIONS%) do (
    echo.
    echo -- Building %%C configuration --
    call cmake --build . --config %%C --parallel 8 || (popd & exit /b 1)

    echo -- Installing %%C configuration --
    call cmake --install . --config %%C || (popd & exit /b 1)
)

popd

REM Clean build directory if requested
if "%CLEAN_BUILD%"=="--clean" (
    echo.
    echo -- Cleaning build directory for %TOOLSET% --
    if exist "%BUILD_DIR%" (
        rmdir /s /q "%BUILD_DIR%"
        echo Build directory removed, keeping only install directory
    ) else (
        echo Build directory does not exist
    )
)

echo Finished %TOOLSET%
endlocal
goto :EOF
