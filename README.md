# HywelStar Video Player

A GStreamer-based video player built with Qt 6, supporting RTSP, UDP, HTTP streams and local video files.

## Features

- Stream playback (RTSP, UDP, HTTP, RTP, TCP MPEG-TS)
- Local video file playback
- Video recording to MKV format
- Screenshot capture (PNG format)
- Grid overlay for video analysis
- Fullscreen mode (keyboard or double-click)
- Volume control
- Settings persistence

## Prerequisites

### Windows

1. **Qt 6.6+**
   - Download from [Qt Online Installer](https://www.qt.io/download-qt-installer)
   - Install MSVC 2022 64-bit kit

2. **GStreamer 1.20+**
   - Download from [GStreamer Downloads](https://gstreamer.freedesktop.org/download/)
   - Install both **Runtime** and **Development** packages (MSVC 64-bit)
   - The installer will set environment variables automatically

3. **Visual Studio 2022** (or Build Tools)
   - Required for MSVC compiler

### Linux (Ubuntu/Debian)

```bash
# Qt 6
sudo apt install qt6-base-dev qt6-tools-dev cmake

# GStreamer
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-good1.0-dev libgstreamer-plugins-bad1.0-dev \
    gstreamer1.0-plugins-ugly gstreamer1.0-libav
```

### macOS

```bash
# Using Homebrew
brew install qt@6 gstreamer gst-plugins-base gst-plugins-good gst-plugins-bad
```

## Build Instructions

### Option 1: Qt Creator (Recommended)

1. Open Qt Creator
2. File → Open File or Project → Select `CMakeLists.txt`
3. Configure the project with your Qt kit
4. Build → Build Project

### Option 2: Command Line

```bash
# Windows (from Developer Command Prompt)
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH=C:/Qt/6.8.0/msvc2022_64
cmake --build build --config Release

# Linux/macOS
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
cmake --build build --config Release
```

### Deployment (Windows)

```bash
# Deploy Qt dependencies
windeployqt --release build/Release/HywelStarVideoPlayer.exe

# Copy GStreamer DLLs to the same directory
# Copy GStreamer plugins to lib/gstreamer-1.0/
```

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| Space | Play/Pause |
| R | Start/Stop Recording |
| S | Screenshot |
| G | Toggle Grid |
| F | Toggle Fullscreen |
| Esc | Exit Fullscreen |
| Q | Quit |

## Mouse Controls

| Action | Function |
|--------|----------|
| Double-click | Toggle fullscreen |

## Project Structure

```
HywelStarPlayer/
├── CMakeLists.txt          # Build configuration
├── cmake/
│   └── find-modules/       # CMake find modules
├── src/
│   ├── main.cpp
│   ├── MainWindow.cpp/h
│   ├── core/               # GStreamer engine, recording
│   ├── ui/                 # UI components
│   └── utils/              # Logger, stream parser
├── resources/
│   ├── resources.qrc
│   └── icons/
└── android/                # Android build files (planned)
```

## Technology Stack

- **UI Framework**: Qt 6.6+
- **Video Engine**: GStreamer 1.20+
- **Language**: C++17
- **Build System**: CMake 3.16+

## License

MIT License

## Author

hywelstar (hywelstar@126.com)
