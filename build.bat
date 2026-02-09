@echo off
REM ============================================================================
REM Hywel Star Video Player - Windows Build Script
REM ============================================================================
REM
REM This script builds the Hywel Star Video Player for Windows using CMake
REM
REM Prerequisites:
REM   - CMake 3.16 or later
REM   - Qt 6.10 or later
REM   - GStreamer 1.22.12
REM   - Visual Studio 2019 or later (MSVC compiler)
REM
REM Usage:
REM   build.bat [Debug|Release]
REM
REM ============================================================================

setlocal enabledelayedexpansion

REM Color codes for output
set "GREEN=[92m"
set "YELLOW=[93m"
set "RED=[91m"
set "RESET=[0m"

REM Parse command line arguments
set BUILD_TYPE=Release
if not "%1"=="" (
    set BUILD_TYPE=%1
)

echo.
echo ============================================================================
echo Hywel Star Video Player - Windows Build Script
echo ============================================================================
echo.
echo Build Type: %BUILD_TYPE%
echo.

REM Check if CMake is installed
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: CMake not found. Please install CMake 3.16 or later.
    exit /b 1
)

echo CMake found:
cmake --version | findstr /R "cmake version"
echo.

REM Check if Qt is installed
if not exist "F:\QT\6.10" (
    echo Warning: Qt 6.10 not found at F:\QT\6.10
    echo Please ensure Qt 6.10 is installed at this location.
    echo.
)

REM Check GStreamer environment
echo Checking GStreamer installation...
if defined GSTREAMER_1_0_ROOT_MSVC_X86_64 (
    echo GStreamer root: %GSTREAMER_1_0_ROOT_MSVC_X86_64%
) else if exist "C:\gstreamer\1.0\msvc_x86_64" (
    echo GStreamer found at: C:\gstreamer\1.0\msvc_x86_64
    set "GSTREAMER_1_0_ROOT_MSVC_X86_64=C:\gstreamer\1.0\msvc_x86_64"
) else if exist "C:\Program Files\gstreamer\1.0\msvc_x86_64" (
    echo GStreamer found at: C:\Program Files\gstreamer\1.0\msvc_x86_64
    set "GSTREAMER_1_0_ROOT_MSVC_X86_64=C:\Program Files\gstreamer\1.0\msvc_x86_64"
) else (
    echo Warning: GStreamer not found. Please install GStreamer 1.22.12
    echo Download from: https://gstreamer.freedesktop.org/download/
    echo.
)

REM Create build directory
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

REM Run CMake configuration
echo.
echo Running CMake configuration...
echo.

cmake -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_PREFIX_PATH="F:\QT\6.10\msvc2019_64" ^
    ..

if %errorlevel% neq 0 (
    echo.
    echo Error: CMake configuration failed.
    cd ..
    exit /b 1
)

REM Build project
echo.
echo Building project...
echo.

cmake --build . --config %BUILD_TYPE% --parallel

if %errorlevel% neq 0 (
    echo.
    echo Error: Build failed.
    cd ..
    exit /b 1
)

echo.
echo ============================================================================
echo Build completed successfully!
echo ============================================================================
echo.
echo Executable location:
echo   %CD%\%BUILD_TYPE%\bin\HywelStarPlayer.exe
echo.
echo To run the application:
echo   %CD%\%BUILD_TYPE%\bin\HywelStarPlayer.exe
echo.

cd ..

exit /b 0
