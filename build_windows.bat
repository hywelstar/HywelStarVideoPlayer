@echo off
setlocal

REM ============================================================================
REM HywelStar Video Player - Windows Build Script
REM ============================================================================

set BUILD_DIR=build\windows_x64
set BUILD_TYPE=Release
set CLEAN_BUILD=0

if /I "%~1"=="Debug" set BUILD_TYPE=Debug
if /I "%~1"=="Release" set BUILD_TYPE=Release
if /I "%~2"=="clean" set CLEAN_BUILD=1
if /I "%~1"=="clean" set CLEAN_BUILD=1

REM Check for Qt path
if not defined QT_PATH (
    if defined QTDIR if exist "%QTDIR%\lib\cmake\Qt6\Qt6Config.cmake" set QT_PATH=%QTDIR%
    if not defined QT_PATH if defined Qt6_DIR if exist "%Qt6_DIR%\Qt6Config.cmake" for %%I in ("%Qt6_DIR%\..\..\..") do set QT_PATH=%%~fI
    if not defined QT_PATH if exist "F:\QT\6.10.0\msvc2022_64\lib\cmake\Qt6\Qt6Config.cmake" set QT_PATH=F:\QT\6.10.0\msvc2022_64
    if not defined QT_PATH if exist "C:\Qt\6.10.0\msvc2022_64\lib\cmake\Qt6\Qt6Config.cmake" set QT_PATH=C:\Qt\6.10.0\msvc2022_64
    if not defined QT_PATH if exist "C:\Qt\6.8.0\msvc2022_64\lib\cmake\Qt6\Qt6Config.cmake" set QT_PATH=C:\Qt\6.8.0\msvc2022_64
)

if not defined GSTREAMER_ROOT_DIR (
    if defined GSTREAMER_1_0_ROOT_MSVC_X86_64 if exist "%GSTREAMER_1_0_ROOT_MSVC_X86_64%\include\gstreamer-1.0\gst\gst.h" set GSTREAMER_ROOT_DIR=%GSTREAMER_1_0_ROOT_MSVC_X86_64%
    if not defined GSTREAMER_ROOT_DIR if exist "E:\gstreamer\1.0\msvc_x86_64\include\gstreamer-1.0\gst\gst.h" set GSTREAMER_ROOT_DIR=E:\gstreamer\1.0\msvc_x86_64
    if not defined GSTREAMER_ROOT_DIR if exist "C:\gstreamer\1.0\msvc_x86_64\include\gstreamer-1.0\gst\gst.h" set GSTREAMER_ROOT_DIR=C:\gstreamer\1.0\msvc_x86_64
    if not defined GSTREAMER_ROOT_DIR if exist "D:\gstreamer\1.0\msvc_x86_64\include\gstreamer-1.0\gst\gst.h" set GSTREAMER_ROOT_DIR=D:\gstreamer\1.0\msvc_x86_64
)

if not defined QT_PATH (
    echo Error: Qt6 not found.
    echo Hint: set QT_PATH to Qt msvc kit root, e.g.:
    echo   set QT_PATH=C:\Qt\6.10.0\msvc2022_64
    exit /b 1
)

if not exist "%QT_PATH%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo Error: Invalid QT_PATH: %QT_PATH%
    echo Missing: %QT_PATH%\lib\cmake\Qt6\Qt6Config.cmake
    exit /b 1
)

if not defined GSTREAMER_ROOT_DIR (
    echo Error: GStreamer not found.
    echo Hint: install GStreamer MSVC x86_64 runtime + development and set:
    echo   set GSTREAMER_1_0_ROOT_MSVC_X86_64=E:\gstreamer\1.0\msvc_x86_64
    exit /b 1
)

if not exist "%GSTREAMER_ROOT_DIR%\include\gstreamer-1.0\gst\gst.h" (
    echo Error: Invalid GStreamer path: %GSTREAMER_ROOT_DIR%
    echo Missing: %GSTREAMER_ROOT_DIR%\include\gstreamer-1.0\gst\gst.h
    exit /b 1
)

if %CLEAN_BUILD%==1 (
    if exist "%BUILD_DIR%" (
        echo Cleaning build directory: %BUILD_DIR%
        rmdir /s /q "%BUILD_DIR%"
    )
)

echo.
echo === HywelStar Video Player Build ===
echo Qt Path: %QT_PATH%
echo GStreamer Path: %GSTREAMER_ROOT_DIR%
echo Build Type: %BUILD_TYPE%
echo.

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Configure
echo Configuring...
cmake -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%" -DGStreamer_ROOT_DIR="%GSTREAMER_ROOT_DIR%"
if errorlevel 1 (
    echo Configuration failed!
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE%
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo === Build Complete ===
echo Output: %BUILD_DIR%\%BUILD_TYPE%\HywelStarVideoPlayer.exe
echo.

endlocal
