@echo off
setlocal

REM ============================================================================
REM HywelStar Video Player - Android Build Script
REM Usage: build_android.bat [arm64-v8a|x86_64]
REM ============================================================================

set ABI=%1
if "%ABI%"=="" set ABI=arm64-v8a

set BUILD_DIR=build\android_%ABI%

REM Check for required paths
if not defined ANDROID_NDK_ROOT (
    if exist "D:\Software\android_sdk\ndk\27.2.12479018" (
        set ANDROID_NDK_ROOT=D:\Software\android_sdk\ndk\27.2.12479018
    ) else (
        echo Error: ANDROID_NDK_ROOT not set
        exit /b 1
    )
)

if not defined QT_ANDROID_PATH (
    if "%ABI%"=="arm64-v8a" (
        set QT_ANDROID_PATH=F:\QT\6.10.0\android_arm64_v8a
    ) else if "%ABI%"=="x86_64" (
        set QT_ANDROID_PATH=F:\QT\6.10.0\android_x86_64
    )
)

echo.
echo === HywelStar Video Player Android Build ===
echo ABI: %ABI%
echo NDK: %ANDROID_NDK_ROOT%
echo Qt: %QT_ANDROID_PATH%
echo.

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Configure
echo Configuring...
cmake -B "%BUILD_DIR%" -G Ninja ^
    -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK_ROOT%\build\cmake\android.toolchain.cmake" ^
    -DANDROID_ABI=%ABI% ^
    -DANDROID_PLATFORM=android-30 ^
    -DCMAKE_PREFIX_PATH="%QT_ANDROID_PATH%"

if errorlevel 1 (
    echo Configuration failed!
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build "%BUILD_DIR%"
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo === Build Complete ===
echo.

endlocal
