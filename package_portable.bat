@echo off
setlocal EnableExtensions

REM ============================================================================
REM HywelStar Video Player - Portable Package Script
REM Usage:
REM   package_portable.bat [Release|Debug] [nozip] [lite] [clean]
REM Examples:
REM   package_portable.bat Release
REM   package_portable.bat Release nozip
REM   package_portable.bat Release lite
REM   package_portable.bat Release nozip lite
REM   package_portable.bat Release clean
REM   package_portable.bat Release nozip lite clean
REM ============================================================================

set "BUILD_TYPE=Release"
set "CREATE_ZIP=1"
set "LITE_MODE=0"
set "CLEAN_MODE=0"

for %%A in (%*) do (
    if /I "%%~A"=="Release" set "BUILD_TYPE=Release"
    if /I "%%~A"=="Debug" set "BUILD_TYPE=Debug"
    if /I "%%~A"=="nozip" set "CREATE_ZIP=0"
    if /I "%%~A"=="lite" set "LITE_MODE=1"
    if /I "%%~A"=="clean" set "CLEAN_MODE=1"
)

REM Validate unknown args
for %%A in (%*) do (
    if /I not "%%~A"=="Release" if /I not "%%~A"=="Debug" if /I not "%%~A"=="nozip" if /I not "%%~A"=="lite" if /I not "%%~A"=="clean" (
        echo Error: Unsupported argument "%%~A"
        echo Usage: package_portable.bat [Release^|Debug] [nozip] [lite] [clean]
        exit /b 1
    )
)

echo.
echo === HywelStar Portable Packaging ===
echo Build Type: %BUILD_TYPE%
if "%LITE_MODE%"=="1" (
    echo Package Mode: Lite ^(smaller, reduced codec coverage^)
) else (
    echo Package Mode: Full
)
if "%CLEAN_MODE%"=="1" echo Clean Mode: Enabled
if "%CREATE_ZIP%"=="0" echo Zip Output: Disabled
if "%CREATE_ZIP%"=="1" echo Zip Output: Enabled
echo.

REM 0) Optional clean rebuild before deployment
if "%CLEAN_MODE%"=="1" (
    echo Running clean rebuild...
    call build_windows.bat %BUILD_TYPE% clean
    if errorlevel 1 (
        echo Clean build failed. Packaging aborted.
        exit /b 1
    )
)

REM 1) Deploy runtime dependencies into build output
call deploy_windows.bat %BUILD_TYPE%
if errorlevel 1 (
    echo Deployment failed. Packaging aborted.
    exit /b 1
)

REM 2) Resolve source output directory
set "SOURCE_DIR="
if exist "build\windows_x64\%BUILD_TYPE%\HywelStarVideoPlayer.exe" (
    set "SOURCE_DIR=build\windows_x64\%BUILD_TYPE%"
) else if exist "build\Desktop_Qt_6_10_0_MSVC2022_64bit-Debug\Debug\HywelStarVideoPlayer.exe" (
    set "SOURCE_DIR=build\Desktop_Qt_6_10_0_MSVC2022_64bit-Debug\Debug"
) else if exist "build\Desktop_Qt_6_10_0_MSVC2022_64bit-Release\Release\HywelStarVideoPlayer.exe" (
    set "SOURCE_DIR=build\Desktop_Qt_6_10_0_MSVC2022_64bit-Release\Release"
)

if not defined SOURCE_DIR (
    echo Error: Cannot locate deployed build output.
    exit /b 1
)

REM 3) Prepare portable output directory
set "DIST_ROOT=dist\portable"
set "PORTABLE_NAME=HywelStarPlayer_Portable_%BUILD_TYPE%"
if "%LITE_MODE%"=="1" set "PORTABLE_NAME=%PORTABLE_NAME%_Lite"
set "PORTABLE_DIR=%DIST_ROOT%\%PORTABLE_NAME%"
set "ZIP_PATH=dist\%PORTABLE_NAME%.zip"

if exist "%PORTABLE_DIR%" rmdir /s /q "%PORTABLE_DIR%"
if not exist "%DIST_ROOT%" mkdir "%DIST_ROOT%"

REM Copy all runtime files except transient runtime data and historical runtime output
robocopy "%SOURCE_DIR%" "%PORTABLE_DIR%" /E /R:1 /W:1 /NFL /NDL /NJH /NJS /NP /XD picture video bin /XF app.log
if errorlevel 8 (
    echo Error: Failed to copy portable files.
    exit /b 1
)

REM Recreate runtime data directories as empty folders
if not exist "%PORTABLE_DIR%\picture" mkdir "%PORTABLE_DIR%\picture"
if not exist "%PORTABLE_DIR%\video" mkdir "%PORTABLE_DIR%\video"

REM 4) Lite-mode pruning (optional)
if "%LITE_MODE%"=="1" (
    echo Applying lite pruning...

    REM Remove test/validation/editing/webrtc families not used by this player runtime
    for %%F in (
        gstcheck-1.0-0.dll
        gstvalidate-1.0-0.dll
        ges-1.0-0.dll
        gsttranscoder-1.0-0.dll
        gstwebrtc-1.0-0.dll
        gstwebrtcnice-1.0-0.dll
        gstrtspserver-1.0-0.dll
        orc-test-0.4-0.dll
    ) do if exist "%PORTABLE_DIR%\%%F" del /f /q "%PORTABLE_DIR%\%%F"

    REM Remove libav plugin + heavy ffmpeg DLL family to reduce package size.
    REM Tradeoff: reduced compatibility for some local file codecs/containers.
    if exist "%PORTABLE_DIR%\lib\gstreamer-1.0\gstlibav.dll" del /f /q "%PORTABLE_DIR%\lib\gstreamer-1.0\gstlibav.dll"
    for %%F in (
        avcodec-59.dll
        avfilter-8.dll
        avformat-59.dll
        avutil-57.dll
        swresample-4.dll
    ) do if exist "%PORTABLE_DIR%\%%F" del /f /q "%PORTABLE_DIR%\%%F"
)

(
    echo HywelStar Video Player - Portable Package
    echo.
    echo 1. Run HywelStarVideoPlayer.exe directly.
    echo 2. Keep all files/folders in this directory together.
    echo 3. Screenshots default to .\picture and recordings to .\video.
    if "%LITE_MODE%"=="1" (
        echo 4. Lite mode removes some codec/runtime components to reduce size.
    )
    echo.
    echo Build type: %BUILD_TYPE%
    if "%LITE_MODE%"=="1" (
        echo Package mode: Lite
    ) else (
        echo Package mode: Full
    )
    if "%CLEAN_MODE%"=="1" (
        echo Clean mode: Enabled
    ) else (
        echo Clean mode: Disabled
    )
    echo Source: %SOURCE_DIR%
) > "%PORTABLE_DIR%\README_PORTABLE.txt"

REM 5) Optional zip archive
if "%CREATE_ZIP%"=="1" (
    if exist "%ZIP_PATH%" del /f /q "%ZIP_PATH%"
    powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%PORTABLE_DIR%' -DestinationPath '%ZIP_PATH%' -Force"
    if errorlevel 1 (
        echo Error: Failed to create zip archive.
        exit /b 1
    )
)

for /f %%S in ('powershell -NoProfile -Command "[math]::Round(((Get-ChildItem -Recurse '%PORTABLE_DIR%' | Measure-Object Length -Sum).Sum / 1MB),2)"') do set "SIZE_MB=%%S"

echo.
echo === Portable Package Complete ===
echo Folder: %PORTABLE_DIR%
echo Size : %SIZE_MB% MB
if "%CREATE_ZIP%"=="1" echo Zip:    %ZIP_PATH%
echo.

endlocal
