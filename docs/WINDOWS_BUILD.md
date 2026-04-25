# Windows Build Guide

This document describes the current Windows build/deploy flow for this project.

## Prerequisites

- Visual Studio 2022 (MSVC x64 toolchain)
- Qt 6.10.0 MSVC kit
- GStreamer 1.22 MSVC x86_64 (Runtime + Development)

## 1) Local environment config (recommended)

Copy local template once and edit paths for your machine:

```bat
copy env.local.example.bat env.local.bat
```

`build_windows.bat` / `deploy_windows.bat` / `build_android.bat` auto-load `env.local.bat`.

Recommended variables for Windows build/deploy:

```bat
set "QT_PATH=C:\Qt\6.10.0\msvc2022_64"
set "GSTREAMER_ROOT_DIR=E:\gstreamer\1.0\msvc_x86_64"
set "GSTREAMER_PATH=E:\gstreamer\1.0\msvc_x86_64"
set "GSTREAMER_1_0_ROOT_MSVC_X86_64=E:\gstreamer\1.0\msvc_x86_64"
```

## 2) Build

Release:

```bat
build_windows.bat Release
```

Debug:

```bat
build_windows.bat Debug
```

Clean rebuild:

```bat
build_windows.bat Release clean
```

## 3) Deploy runtime dependencies

```bat
deploy_windows.bat Release
```

This step runs `windeployqt` and copies required GStreamer DLLs/plugins.

## 4) Portable package (optional)

```bat
package_portable.bat Release
```

Common options:

```bat
package_portable.bat Release nozip
package_portable.bat Release lite
package_portable.bat Release nozip lite clean
```

## 5) Common errors

- `Qt6 not found` / `Invalid QT_PATH`
  - Verify `QT_PATH` points to a valid MSVC Qt kit root.
- `GStreamer not found` / `Invalid GStreamer path`
  - Verify `GSTREAMER_ROOT_DIR` or `GSTREAMER_1_0_ROOT_MSVC_X86_64`.
- App starts but no plugins loaded
  - Run `deploy_windows.bat Release` after building.

## 6) Output

- Built exe:
  - `build\windows_x64\Release\HywelStarVideoPlayer.exe`
- Portable package:
  - `dist\portable\HywelStarPlayer_Portable_Release\`
