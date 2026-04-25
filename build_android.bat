@echo off
setlocal EnableExtensions

REM ============================================================================
REM HywelStar Video Player - Android Build Script
REM Usage: build_android.bat [arm64-v8a|x86_64]
REM ============================================================================

set "SCRIPT_DIR=%~dp0"
if exist "%SCRIPT_DIR%env.local.bat" (
    call "%SCRIPT_DIR%env.local.bat"
)

set "ABI=%~1"
if "%ABI%"=="" set "ABI=arm64-v8a"
if /I not "%ABI%"=="arm64-v8a" if /I not "%ABI%"=="x86_64" (
    echo Error: Unsupported ABI "%ABI%"
    echo Usage: build_android.bat [arm64-v8a^|x86_64]
    exit /b 1
)

set "BUILD_DIR=build\android_%ABI%"
if not defined ANDROID_PLATFORM set "ANDROID_PLATFORM=android-35"

if not defined CMAKE_EXE set "CMAKE_EXE=cmake"
if /I "%CMAKE_EXE%"=="cmake" (
    where cmake >nul 2>nul
    if errorlevel 1 (
        echo Error: cmake not found in PATH
        echo Hint: set CMAKE_EXE in env.local.bat or add cmake to PATH
        exit /b 1
    )
) else (
    if not exist "%CMAKE_EXE%" (
        echo Error: Invalid CMAKE_EXE: %CMAKE_EXE%
        exit /b 1
    )
)

if not defined NINJA_EXE (
    for /f "delims=" %%I in ('where ninja 2^>nul') do (
        set "NINJA_EXE=%%I"
        goto :ninja_found
    )
)
:ninja_found
if not defined NINJA_EXE (
    echo Error: ninja not found.
    echo Hint: set NINJA_EXE in env.local.bat or add ninja to PATH
    exit /b 1
)
if not exist "%NINJA_EXE%" (
    echo Error: Invalid NINJA_EXE: %NINJA_EXE%
    exit /b 1
)

if not defined ANDROID_SDK_ROOT (
    if defined ANDROID_HOME set "ANDROID_SDK_ROOT=%ANDROID_HOME%"
)
if not defined ANDROID_SDK_ROOT (
    if exist "%LOCALAPPDATA%\Android\Sdk\build-tools" set "ANDROID_SDK_ROOT=%LOCALAPPDATA%\Android\Sdk"
)

if not defined ANDROID_NDK_ROOT (
    if defined ANDROID_NDK_HOME set "ANDROID_NDK_ROOT=%ANDROID_NDK_HOME%"
)
if not defined ANDROID_NDK_ROOT (
    if defined ANDROID_SDK_ROOT call :pick_latest_ndk "%ANDROID_SDK_ROOT%\ndk"
)

if not defined QT_ANDROID_PATH (
    if /I "%ABI%"=="arm64-v8a" if defined QT_ANDROID_ARM64 set "QT_ANDROID_PATH=%QT_ANDROID_ARM64%"
    if /I "%ABI%"=="x86_64" if defined QT_ANDROID_X86_64 set "QT_ANDROID_PATH=%QT_ANDROID_X86_64%"
)
if not defined QT_HOST_PATH (
    if defined QT_PATH set "QT_HOST_PATH=%QT_PATH%"
)
if not defined QT_HOST_PATH (
    if defined QTDIR set "QT_HOST_PATH=%QTDIR%"
)

if not defined ANDROID_NDK_ROOT (
    echo Error: ANDROID_NDK_ROOT not set.
    echo Hint: set ANDROID_NDK_ROOT in env.local.bat
    exit /b 1
)
if not exist "%ANDROID_NDK_ROOT%\build\cmake\android.toolchain.cmake" (
    echo Error: Invalid ANDROID_NDK_ROOT: %ANDROID_NDK_ROOT%
    echo Missing: %ANDROID_NDK_ROOT%\build\cmake\android.toolchain.cmake
    exit /b 1
)

if not defined ANDROID_SDK_ROOT (
    echo Error: ANDROID_SDK_ROOT not set.
    echo Hint: set ANDROID_SDK_ROOT in env.local.bat
    exit /b 1
)
if not exist "%ANDROID_SDK_ROOT%\build-tools" (
    echo Error: Invalid ANDROID_SDK_ROOT: %ANDROID_SDK_ROOT%
    echo Missing: %ANDROID_SDK_ROOT%\build-tools
    exit /b 1
)

if not defined QT_ANDROID_PATH (
    echo Error: QT_ANDROID_PATH is not set for ABI %ABI%.
    echo Hint: set QT_ANDROID_ARM64 / QT_ANDROID_X86_64 in env.local.bat
    exit /b 1
)
if not exist "%QT_ANDROID_PATH%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo Error: Invalid QT_ANDROID_PATH: %QT_ANDROID_PATH%
    echo Missing: %QT_ANDROID_PATH%\lib\cmake\Qt6\Qt6Config.cmake
    exit /b 1
)

if not defined QT_HOST_PATH (
    echo Error: QT_HOST_PATH is not set.
    echo Hint: set QT_HOST_PATH in env.local.bat
    exit /b 1
)
if not exist "%QT_HOST_PATH%\lib\cmake\Qt6HostInfo\Qt6HostInfoConfig.cmake" (
    echo Error: Invalid QT_HOST_PATH: %QT_HOST_PATH%
    echo Missing: %QT_HOST_PATH%\lib\cmake\Qt6HostInfo\Qt6HostInfoConfig.cmake
    exit /b 1
)

echo.
echo === HywelStar Video Player Android Build ===
echo ABI: %ABI%
echo NDK: %ANDROID_NDK_ROOT%
echo SDK: %ANDROID_SDK_ROOT%
echo Qt: %QT_ANDROID_PATH%
echo Qt Host: %QT_HOST_PATH%
echo Android Platform: %ANDROID_PLATFORM%
echo CMake: %CMAKE_EXE%
echo Ninja: %NINJA_EXE%
echo.

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if exist "%BUILD_DIR%\CMakeCache.txt" del /f /q "%BUILD_DIR%\CMakeCache.txt"
if exist "%BUILD_DIR%\CMakeFiles" rmdir /s /q "%BUILD_DIR%\CMakeFiles"

echo Configuring...
"%CMAKE_EXE%" -B "%BUILD_DIR%" -G Ninja ^
    -DCMAKE_MAKE_PROGRAM="%NINJA_EXE%" ^
    -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK_ROOT%\build\cmake\android.toolchain.cmake" ^
    -DANDROID_ABI=%ABI% ^
    -DANDROID_PLATFORM=%ANDROID_PLATFORM% ^
    -DANDROID_SDK_ROOT="%ANDROID_SDK_ROOT%" ^
    -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY ^
    -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH ^
    -DQT_HOST_PATH="%QT_HOST_PATH%" ^
    -DQt6HostInfo_DIR="%QT_HOST_PATH%\lib\cmake\Qt6HostInfo" ^
    -DCMAKE_PREFIX_PATH="%QT_ANDROID_PATH%" ^
    -DQt6_DIR="%QT_ANDROID_PATH%\lib\cmake\Qt6"

if errorlevel 1 (
    echo Configuration failed!
    exit /b 1
)

echo.
echo Building...
"%CMAKE_EXE%" --build "%BUILD_DIR%"
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo === Build Complete ===
echo.

endlocal
exit /b 0

:pick_latest_ndk
set "NDK_BASE=%~1"
if not exist "%NDK_BASE%" goto :eof
for /f "delims=" %%D in ('dir /b /ad "%NDK_BASE%" 2^>nul ^| sort /R') do (
    if exist "%NDK_BASE%\%%D\build\cmake\android.toolchain.cmake" (
        set "ANDROID_NDK_ROOT=%NDK_BASE%\%%D"
        goto :eof
    )
)
goto :eof
