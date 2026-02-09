@echo off
REM ============================================================================
REM Hywel Star Video Player - Android Build Script
REM ============================================================================
REM
REM This script builds the Hywel Star Video Player for Android using CMake
REM
REM Prerequisites:
REM   - CMake 3.16 or later
REM   - Android NDK 27.2.12479018 or later
REM   - Android SDK
REM   - Qt 6.10.0 for Android (arm64-v8a)
REM
REM Environment Variables:
REM   - ANDROID_NDK_ROOT: Path to Android NDK
REM   - ANDROID_SDK_ROOT: Path to Android SDK
REM   - QT_ANDROID_PATH: Path to Qt Android installation
REM
REM Usage:
REM   build_android.bat [arm64-v8a|armv7|x86|x86_64]
REM
REM ============================================================================

setlocal enabledelayedexpansion

REM Parse command line arguments
set ANDROID_ABI=arm64-v8a
if not "%1"=="" (
    set ANDROID_ABI=%1
)

echo.
echo ============================================================================
echo Hywel Star Video Player - Android Build Script
echo ============================================================================
echo.
echo Target ABI: %ANDROID_ABI%
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

REM Check Android NDK
if defined ANDROID_NDK_ROOT (
    echo Android NDK: %ANDROID_NDK_ROOT%
) else if exist "D:\Software\android_sdk\ndk\27.2.12479018" (
    set "ANDROID_NDK_ROOT=D:\Software\android_sdk\ndk\27.2.12479018"
    echo Android NDK found at: %ANDROID_NDK_ROOT%
) else (
    echo Error: Android NDK not found. Please set ANDROID_NDK_ROOT environment variable.
    exit /b 1
)

REM Check Android SDK
if defined ANDROID_SDK_ROOT (
    echo Android SDK: %ANDROID_SDK_ROOT%
) else if exist "D:\Software\android_sdk" (
    set "ANDROID_SDK_ROOT=D:\Software\android_sdk"
    echo Android SDK found at: %ANDROID_SDK_ROOT%
) else (
    echo Error: Android SDK not found. Please set ANDROID_SDK_ROOT environment variable.
    exit /b 1
)

REM Check Qt Android
if defined QT_ANDROID_PATH (
    echo Qt Android: %QT_ANDROID_PATH%
) else if exist "F:\QT\6.10.0\android_%ANDROID_ABI%" (
    set "QT_ANDROID_PATH=F:\QT\6.10.0\android_%ANDROID_ABI%"
    echo Qt Android found at: %QT_ANDROID_PATH%
) else (
    echo Error: Qt Android not found. Please set QT_ANDROID_PATH environment variable.
    exit /b 1
)

REM Create build directory
set BUILD_DIR=build_android_%ANDROID_ABI%
if not exist "%BUILD_DIR%" (
    echo Creating build directory: %BUILD_DIR%
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%"

REM Run CMake configuration
echo.
echo Running CMake configuration for Android...
echo.

cmake -G Ninja ^
    -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK_ROOT%\build\cmake\android.toolchain.cmake" ^
    -DCMAKE_PREFIX_PATH="%QT_ANDROID_PATH%" ^
    -DANDROID_ABI=%ANDROID_ABI% ^
    -DANDROID_PLATFORM=android-30 ^
    -DANDROID_NDK="%ANDROID_NDK_ROOT%" ^
    -DANDROID_SDK_ROOT="%ANDROID_SDK_ROOT%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    ..

if %errorlevel% neq 0 (
    echo.
    echo Error: CMake configuration failed.
    cd ..
    exit /b 1
)

REM Build project
echo.
echo Building project for Android...
echo.

cmake --build . --config Release --parallel

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
echo Output directory: %CD%
echo.

cd ..

exit /b 0
