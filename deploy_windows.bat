@echo off
setlocal

REM ============================================================================
REM HywelStar Video Player - Windows Deployment Script
REM Usage: deploy_windows.bat [Release|Debug]
REM ============================================================================

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

REM Find the exe
if exist "build\windows_x64\%BUILD_TYPE%\HywelStarVideoPlayer.exe" (
    set EXE_PATH=build\windows_x64\%BUILD_TYPE%
) else if exist "build\Desktop_Qt_6_10_0_MSVC2022_64bit-Debug\Debug\HywelStarVideoPlayer.exe" (
    set EXE_PATH=build\Desktop_Qt_6_10_0_MSVC2022_64bit-Debug\Debug
    set BUILD_TYPE=Debug
) else if exist "build\Desktop_Qt_6_10_0_MSVC2022_64bit-Release\Release\HywelStarVideoPlayer.exe" (
    set EXE_PATH=build\Desktop_Qt_6_10_0_MSVC2022_64bit-Release\Release
    set BUILD_TYPE=Release
) else (
    echo Error: HywelStarVideoPlayer.exe not found. Build the project first.
    exit /b 1
)

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

REM Check for GStreamer path
if not defined GSTREAMER_PATH (
    if defined GSTREAMER_1_0_ROOT_MSVC_X86_64 (
        set GSTREAMER_PATH=%GSTREAMER_1_0_ROOT_MSVC_X86_64%
    ) else if exist "E:\gstreamer\1.0\msvc_x86_64" (
        set GSTREAMER_PATH=E:\gstreamer\1.0\msvc_x86_64
    ) else if exist "C:\gstreamer\1.0\msvc_x86_64" (
        set GSTREAMER_PATH=C:\gstreamer\1.0\msvc_x86_64
    ) else (
        echo Error: GStreamer not found. Set GSTREAMER_PATH environment variable.
        exit /b 1
    )
)

echo.
echo === HywelStar Video Player Deployment ===
echo Build Type: %BUILD_TYPE%
echo EXE Path: %EXE_PATH%
echo Qt Path: %QT_PATH%
echo GStreamer Path: %GSTREAMER_PATH%
echo.

REM Deploy Qt dependencies
echo Deploying Qt dependencies...
"%QT_PATH%\bin\windeployqt.exe" --no-translations "%EXE_PATH%\HywelStarVideoPlayer.exe"
if errorlevel 1 (
    echo Qt deployment failed!
    exit /b 1
)

REM Copy GStreamer DLLs
echo.
echo Copying GStreamer DLLs...
copy "%GSTREAMER_PATH%\bin\*.dll" "%EXE_PATH%\" >nul 2>&1

REM Create GStreamer plugins directory
if not exist "%EXE_PATH%\lib\gstreamer-1.0" mkdir "%EXE_PATH%\lib\gstreamer-1.0"

REM Copy essential GStreamer plugins
echo Copying GStreamer plugins...
set GST_PLUGINS=%GSTREAMER_PATH%\lib\gstreamer-1.0

REM Core plugins
copy "%GST_PLUGINS%\gstcoreelements.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstplayback.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gsttypefindfunctions.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstapp.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstautodetect.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1

REM Video plugins
copy "%GST_PLUGINS%\gstvideoconvertscale.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstvideoparsersbad.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstvideorate.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstd3d11.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1

REM Codec plugins
copy "%GST_PLUGINS%\gstlibav.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstopenh264.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstx264.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstx265.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1

REM Container plugins
copy "%GST_PLUGINS%\gstisomp4.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstmatroska.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstavi.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1

REM Network plugins
copy "%GST_PLUGINS%\gstrtsp.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstrtp.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstrtpmanager.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstudp.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gsttcp.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstsdpelem.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1

REM Audio plugins
copy "%GST_PLUGINS%\gstaudioconvert.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstaudioresample.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstwasapi2.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1
copy "%GST_PLUGINS%\gstvolume.dll" "%EXE_PATH%\lib\gstreamer-1.0\" >nul 2>&1

echo.
echo === Deployment Complete ===
echo Output: %EXE_PATH%
echo.
echo You can now run: %EXE_PATH%\HywelStarVideoPlayer.exe
echo.

endlocal
