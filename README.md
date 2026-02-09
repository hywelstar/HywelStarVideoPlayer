# HywelStar Video Player

A cross-platform video player based on GStreamer and Qt 6.10, supporting Windows and Android platforms.

## Features

### Supported Stream Formats
- RTSP (H.264/H.265)
- UDP H.264/H.265
- TCP MPEG-TS
- HTTP/HTTPS streaming
- Local video files

### Core Features
- Real-time video playback and recording
- Screenshot capture
- Fullscreen/windowed mode toggle
- Grid overlay for positioning assistance
- Volume control
- Connection history

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Windows x64 | Supported | Qt 6.10 + GStreamer 1.22.12 |
| Android arm64-v8a | Supported | Qt 6.10 for Android |
| Android x86_64 | Supported | For emulator testing |

## Build Requirements

### Common Requirements
- CMake 3.16+
- Qt 6.10.0
- C++17 compiler

### Windows
- Visual Studio 2022
- GStreamer 1.22.12 (MSVC 64-bit)

### Android
- Android NDK 27+
- Android SDK

## Build Instructions

### Windows Build

```bash
# Configure
cd build/windows_x64
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="F:/QT/6.10.0/msvc2022_64" ../..

# Build
cmake --build . --config Release

# Deploy Qt dependencies
windeployqt --release Release/bin/HywelStarVideoPlayer.exe

# Copy GStreamer DLLs to Release/bin/
# Copy GStreamer plugins to Release/bin/lib/gstreamer-1.0/
```

Or use the build script:
```bash
build_windows.bat
```

### Android Build

```bash
# Using build script
build_android.bat arm64-v8a

# Or manual configuration
cd build/android_arm64_v8a
cmake -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK_ROOT%/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-30 \
    -DCMAKE_PREFIX_PATH="F:/QT/6.10.0/android_arm64_v8a" \
    ../..

cmake --build .
```

## Keyboard Shortcuts

| Key | Function |
|-----|----------|
| F | Toggle fullscreen |
| Space | Play/Pause |
| R | Start/Stop recording |
| S | Screenshot |
| G | Show/Hide grid |
| Q | Quit application |

## Project Structure

```
HywelStarVideoPlayer/
├── src/
│   ├── main.cpp                    # Application entry point
│   ├── MainWindow.h/.cpp           # Main window
│   ├── core/
│   │   ├── GStreamerEngine.h/.cpp  # GStreamer engine
│   │   ├── RecordingManager.h/.cpp # Recording management
│   │   └── ConfigManager.h/.cpp    # Configuration management
│   ├── ui/
│   │   ├── VideoDisplayWidget.h/.cpp  # Video display
│   │   ├── ControlBar.h/.cpp          # Control bar
│   │   ├── StatusBar.h/.cpp           # Status bar
│   │   └── QuickConnectBar.h/.cpp     # Quick connect bar
│   └── utils/
│       ├── Logger.h/.cpp           # Logging system
│       └── StreamParser.h/.cpp     # Stream URI parser
├── android/                        # Android configuration
├── resources/                      # Resource files
├── cmake/                          # CMake modules
├── CMakeLists.txt
└── README.md
```

## Technology Stack

- **UI Framework**: Qt 6.10
- **Video Engine**: GStreamer 1.22.12
- **Language**: C++17
- **Build System**: CMake 3.16+

## Author

- **Author**: hywelstar
- **Email**: hywelstar@126.com

## License

MIT License
