# Android Build Guide

This document reflects the current working Android build flow (validated on Windows).

## Current status

- Windows build: stable.
- Android build: `arm64-v8a` compile + APK packaging is working.
- Current Android runtime is built in stub mode (GStreamer disabled on Android in CMake for now).

## Prerequisites

- Qt: `6.10.0`
  - Android kit: `F:\QT\6.10.0\android_arm64_v8a` (or `android_x86_64`)
  - Host kit: `F:\QT\6.10.0\msvc2022_64`
  - Tools: `F:\Qt\Tools\CMake_64\bin\cmake.exe`, `F:\Qt\Tools\Ninja\ninja.exe`
- Android SDK with Build Tools installed (example: `36.1.0`)
- Android NDK: `27.2.12479018`

## 1) Set environment (PowerShell)

Recommended: copy `env.local.example.bat` to `env.local.bat` in project root and edit paths once.
`build_android.bat` / `build_windows.bat` / `deploy_windows.bat` auto-load `env.local.bat`.

```bat
copy env.local.example.bat env.local.bat
```

If your local paths differ from script defaults, set these before build:

```powershell
$env:ANDROID_NDK_ROOT="D:\Software\android_sdk\ndk\27.2.12479018"
$env:ANDROID_SDK_ROOT="D:\Software\android_sdk"
$env:QT_HOST_PATH="F:\QT\6.10.0\msvc2022_64"
```

Optional platform override (default is `android-35`):

```powershell
$env:ANDROID_PLATFORM="android-35"
```

## 2) Build

```bat
build_android.bat arm64-v8a
```

or:

```bat
build_android.bat x86_64
```

Default ABI is `arm64-v8a`, so this is also valid:

```bat
build_android.bat
```

## 3) Output

After success, unsigned APK is generated at:

```text
build\android_arm64-v8a\android-build\build\outputs\apk\release\android-build-release-unsigned.apk
```

## 4) Common errors

- `ANDROID_NDK_ROOT not set`
  - Set `ANDROID_NDK_ROOT` in terminal before running the script.
- `ninja not found`
  - Install Ninja or ensure `F:\Qt\Tools\Ninja\ninja.exe` exists.
- `compileSdk ... currently compiled against android-33-ext4`
  - Use the latest script + current `CMakeLists.txt` (already fixed to compileSdk 35).
- `android-36 is above the maximum supported version 35`
  - With NDK `27.2.12479018`, use `ANDROID_PLATFORM=android-35`.

## 5) Notes for teammates

- The script has machine-local default paths for this repo owner's environment.
- If sharing to other machines, set env vars (section 1) before build.
- Commit shared files only (`build_android.bat`, `CMakeLists.txt`, docs).
