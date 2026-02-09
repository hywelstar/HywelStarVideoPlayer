# HywelStarPlayer Build Script
Write-Host "Setting up Visual Studio environment..." -ForegroundColor Cyan

# Load VS Developer PowerShell
& "D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1" -Arch amd64

Write-Host "Building HywelStarPlayer..." -ForegroundColor Cyan
Set-Location "E:\ai\hywelPlayer\HywelStarPlayer\build\windows_x64"
cmake --build . --config Release

Write-Host ""
Write-Host "Build completed!" -ForegroundColor Green
Write-Host "Executable: E:\ai\hywelPlayer\HywelStarPlayer\build\windows_x64\Release\bin\HywelStarPlayer.exe"
Write-Host ""
Read-Host "Press Enter to exit"
