@echo off
setlocal

REM ============================================================================
REM HywelStar Video Player - Windows Build Script
REM ============================================================================

set BUILD_DIR=build\windows_x64
set BUILD_TYPE=Release

REM Check for Qt path
if not defined QT_PATH (
    if exist "F:\QT\6.10.0\msvc2022_64" (
        set QT_PATH=F:\QT\6.10.0\msvc2022_64
    ) else if exist "C:\Qt\6.8.0\msvc2022_64" (
        set QT_PATH=C:\Qt\6.8.0\msvc2022_64
    ) else (
        echo Error: Qt not found. Set QT_PATH environment variable.
        exit /b 1
    )
)

echo.
echo === HywelStar Video Player Build ===
echo Qt Path: %QT_PATH%
echo Build Type: %BUILD_TYPE%
echo.

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Configure
echo Configuring...
cmake -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%"
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
