# HywelStar Video Player

A GStreamer-based video player built with Qt 6, supporting RTSP, UDP, HTTP streams and local video files.

## Development Progress

### Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Windows x64 | ✅ Done | Qt 6.10 + GStreamer 1.22 |
| Linux x64 | 🔲 Planned | - |
| macOS | 🔲 Planned | - |
| Android arm64-v8a | 🚧 In Progress | GStreamer integration pending |
| Android x86_64 | 🔲 Planned | For emulator |

### Features

| Feature | Status | Notes |
|---------|--------|-------|
| RTSP Stream Playback | ✅ Done | H.264/H.265 |
| UDP Stream Playback | ✅ Done | H.264/H.265 |
| HTTP/HTTPS Playback | ✅ Done | - |
| Local File Playback | ✅ Done | MP4, MKV, AVI, etc. |
| Video Recording | ✅ Done | MKV format |
| Screenshot | ✅ Done | PNG format |
| Grid Overlay | ✅ Done | For positioning |
| Fullscreen Mode | ✅ Done | F key / double-click |
| Volume Control | ✅ Done | Slider + mute |
| Settings Persistence | ✅ Done | Window size, URI, volume |
| Recording Timer | ✅ Done | Real-time display |
| Menu Bar | ✅ Done | Help > About |
| Settings Dialog | ✅ Done | Recording/Screenshot paths |
| About Dialog | ✅ Done | App info |
| Local Playlist | 🔲 Planned | File/folder selection |
| Playback Speed Control | 🔲 Planned | 0.5x - 2x |
| Subtitle Support | 🔲 Planned | SRT, ASS |
| Audio Track Selection | 🔲 Planned | Multi-track support |
| Hardware Acceleration | 🚧 Partial | D3D11 on Windows |
| Multi-language UI | 🔲 Planned | i18n support |

### Known Issues

- [ ] Android GStreamer integration not complete
- [ ] Linux/macOS builds not tested
- [ ] Hardware decoding may not work on all GPUs

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

### Option 1: Build Script (Windows)

```bash
build_windows.bat
```

### Option 2: Qt Creator (Recommended)

1. Open Qt Creator
2. File → Open File or Project → Select `CMakeLists.txt`
3. Configure the project with your Qt kit
4. Build → Build Project

### Option 3: Command Line

```bash
# Windows (from Developer Command Prompt)
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH=C:/Qt/6.8.0/msvc2022_64
cmake --build build --config Release

# Linux/macOS
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64
cmake --build build --config Release
```

### Android Build

```bash
build_android.bat arm64-v8a
```

### Deployment (Windows)

```bash
# Deploy Qt dependencies
windeployqt --release build/windows_x64/Release/HywelStarVideoPlayer.exe

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
├── build_windows.bat       # Windows build script
├── build_android.bat       # Android build script
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
└── android/                # Android build files
```

## Technology Stack

- **UI Framework**: Qt 6.6+
- **Video Engine**: GStreamer 1.20+
- **Language**: C++17
- **Build System**: CMake 3.16+

## Changelog

### v1.0.0 (In Development)
- Basic stream playback (RTSP, UDP, HTTP)
- Video recording and screenshot
- Fullscreen mode with keyboard/mouse support
- Grid overlay
- Settings persistence
- Simplified UI with combined Play/Pause button

## License

MIT License

## Author

hywelstar (hywelstar@126.com)
