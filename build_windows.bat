@echo off
echo Setting up Visual Studio environment...
call "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 (
    echo Failed to setup VS environment
    pause
    exit /b 1
)

echo Building HywelStarVideoPlayer...
cd /d "E:\ai\hywelPlayer\HywelStarPlayer\build\windows_x64"
cmake --build . --config Release

echo.
echo Build completed!
pause
