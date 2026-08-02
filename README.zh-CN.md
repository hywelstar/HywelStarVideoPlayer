# HywelStar Player

[English](README.md) | [简体中文](README.zh-CN.md)

HywelStar Player 是一个基于 Qt 6 和 GStreamer 的视频播放器，支持 RTSP、UDP、TCP、RTMP、SRT、HLS、DASH、HTTP 网络流和本地视频文件播放。

![image-20260802142054603](image/image-20260802142054603.png)

## 开发进度

### 平台支持

| 平台 | 状态 | 说明 |
|------|------|------|
| Windows x64 | 已完成 | Qt 6.10 + GStreamer 1.22 |
| Linux x64 | 计划中 | - |
| macOS | 计划中 | - |
| Android arm64-v8a | 进行中 | GStreamer 集成待完成 |
| Android x86_64 | 计划中 | 用于模拟器 |

### 功能

| 功能 | 状态 | 说明 |
|------|------|------|
| RTSP 流播放 | 已完成 | H.264/H.265 |
| UDP 流播放 | 已完成 | H.264/H.265 |
| TCP 流播放 | 已完成 | 基础 URI 支持 |
| RTMP/RTMPS 播放 | 已完成 | 已使用公网 RTMP 流测试 |
| SRT 播放 | 已完成 | URI 识别和 playbin3 播放；实际源待验证 |
| HLS 播放 | 已完成 | 已测试 HTTPS `.m3u8` |
| MPEG-DASH 播放 | 已完成 | 已测试 HTTPS `.mpd` |
| HTTP/HTTPS 播放 | 已完成 | 渐进式流和 MJPEG URL |
| 本地文件播放 | 已完成 | MP4、MKV、AVI 等 |
| Stream/Local 模式切换 | 已完成 | 网络流和本地播放流程分离 |
| 本地播放列表 | 已完成 | 文件/文件夹选择，播放列表持久化 |
| 本地播放结束模式 | 已完成 | Play Once、Loop One、Loop All |
| 播放进度浮层 | 已完成 | 视频画面内进度条、拖动跳转、自动隐藏 |
| 播放倍速控制 | 已完成 | 0.5x - 2x |
| 适应/拉伸显示 | 已完成 | 保持比例或拉伸显示 |
| 码率显示 | 已完成 | 本地平均码率和 RTSP RTP 视频码率 |
| 视频录制 | 已完成 | MKV 格式，支持本地和网络流播放录制 |
| 截图 | 已完成 | PNG 格式 |
| 网格叠加 | 已完成 | 用于画面定位 |
| 全屏模式 | 已完成 | F 键 / 双击 |
| 音量控制 | 已完成 | 滑块 + 静音 |
| 主题选择 | 已完成 | System、Light、Dark |
| 设置持久化 | 已完成 | 窗口大小、流地址、音量、模式、循环设置 |
| 录制定时器 | 已完成 | 实时显示 |
| 菜单栏 | 已完成 | 顶部菜单 |
| 设置窗口 | 已完成 | 录制/截图路径 |
| 关于窗口 | 已完成 | 应用信息 |
| 字幕支持 | 计划中 | SRT、ASS |
| 音轨选择 | 计划中 | 多音轨支持 |
| 硬件加速 | 部分完成 | Windows 使用 D3D11 |
| 多语言界面 | 计划中 | i18n 支持 |

### 已知问题

- [ ] Android GStreamer 集成未完成
- [ ] Linux/macOS 构建未测试
- [ ] 硬件解码可能不适用于所有 GPU

## 环境要求

### Windows

1. **Qt 6.6+**
   - 从 [Qt Online Installer](https://www.qt.io/download-qt-installer) 下载
   - 安装 MSVC 2022 64-bit kit

2. **GStreamer 1.20+**
   - 从 [GStreamer Downloads](https://gstreamer.freedesktop.org/download/) 下载
   - 安装 **Runtime** 和 **Development** 两个包，选择 MSVC 64-bit

3. **Visual Studio 2022** 或 Build Tools
   - 用于 MSVC 编译

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

## 构建说明

### 1) 本地环境配置（推荐）

首次复制本地环境模板，然后按自己的机器路径修改：

```bat
copy env.local.example.bat env.local.bat
```

构建脚本会自动加载 `env.local.bat`。

### 2) Windows 构建

```bat
build_windows.bat Release
```

查看脚本帮助：

```bat
build_windows.bat --help
```

如果输出 exe 正在运行，脚本会尝试关闭 `HywelStarVideoPlayer.exe` 并自动重试一次构建。

### 3) Android 构建

```bat
build_android.bat arm64-v8a
```

### 4) Windows 部署

```bat
deploy_windows.bat Release
```

### 5) Qt Creator（可选）

1. 打开 Qt Creator
2. 打开 `CMakeLists.txt`
3. 选择 Qt kit
4. 构建并运行

## 快捷键

| 按键 | 操作 |
|------|------|
| Space | 播放/暂停 |
| R | 开始/停止录制 |
| S | 截图 |
| G | 显示/隐藏网格 |
| F | 切换全屏 |
| Esc | 退出全屏 |
| Q | 退出 |

## 鼠标操作

| 操作 | 功能 |
|------|------|
| 鼠标移入视频区域 | 显示播放进度浮层 |
| 点击视频画面 | 播放/暂停 |
| 拖动进度条 | 跳转本地/媒体播放位置 |
| 双击 | 切换全屏 |

## 播放模式和显示

- **Stream 模式**：在顶部输入框输入 RTSP/UDP/TCP/RTMP/SRT/HLS/DASH/HTTP/HTTPS URL。
- **Local 模式**：从本地播放列表选择文件或文件夹。
- **本地播放结束行为**：可选择 Play Once、Loop One、Loop All。
- **码率显示**：本地文件显示平均媒体码率；RTSP 流在可用时显示测量到的 RTP 视频码率。
- **主题**：可在 Settings 中选择 System、Light、Dark。

## 已测试流地址

| 协议 | 状态 | URL |
|------|------|-----|
| RTSP | 通过 | `rtsp://192.168.1.39:8554/video/slamtv60.264` |
| RTMP | 通过 | `rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid` |
| HLS | 通过 | `https://test-streams.mux.dev/x36xhzz/x36xhzz.m3u8` |
| MPEG-DASH | 通过 | `https://dash.akamaized.net/akamai/bbb_30fps/bbb_30fps.mpd` |
| SRT | 待验证 | 需要可访问的 SRT 源或服务器 |
| MJPEG | 待验证 | 建议使用可信摄像头或本地 MJPEG 测试服务 |

## 项目结构

```text
HywelStarPlayer/
|-- CMakeLists.txt          # 构建配置
|-- build_windows.bat       # Windows 构建脚本
|-- build_android.bat       # Android 构建脚本
|-- deploy_windows.bat      # Windows 部署脚本
|-- package_portable.bat    # 便携版打包脚本
|-- env.local.example.bat   # 本地环境模板
|-- cmake/
|   `-- find-modules/       # CMake 查找模块
|-- src/
|   |-- main.cpp
|   |-- MainWindow.cpp/h
|   |-- core/               # GStreamer 引擎和录制
|   |-- ui/                 # UI 组件和主题
|   |   |-- LocalFileListWidget.cpp/h
|   |   `-- ThemeManager.cpp/h
|   `-- utils/              # Logger、stream parser
|-- resources/
|   |-- resources.qrc
|   `-- icons/
`-- docs/
```

## 技术栈

- **UI 框架**：Qt 6.6+
- **视频引擎**：GStreamer 1.20+
- **语言**：C++17
- **构建系统**：CMake 3.16+

## 更新日志

### v1.1.2 (Released 2026-08-02)
- 增加 RTSP、RTMP、HLS、MPEG-DASH 已测试流地址说明。
- 改进 RTMP、SRT、HLS、DASH、HTTP/MJPEG 和 HTTPS TLS 运行时依赖的协议诊断。
- 修复 Windows 便携版 HTTPS HLS 播放问题，打包 GIO TLS 模块并设置 `GIO_MODULE_DIR`。
- 修复本地和网络流录制启动时由 D3D11 内存协商导致的失败。
- 改进录制稳定性，避免录制分支反压影响播放预览。

### v1.1.1 (Released 2026-08-02)
- 修复播放音量控制和静音/取消静音行为。
- 修复本地文件和 RTSP 视频流码率上报。
- 增加 System/Light/Dark 主题支持并改进暗色主题可读性。
- 修复全屏播放时菜单栏未隐藏的问题。
- 增加 RTMP、SRT、HLS、DASH 和 MJPEG URL 的第一阶段协议识别。

### v1.1.0 (Released 2026-05-08)
- 增加顶部 Stream/Local 模式切换。
- 增加本地文件列表，支持文件/文件夹导入和播放列表持久化。
- 增加本地播放模式：Play Once、Loop One、Loop All。
- 增加视频画面内进度浮层，支持拖动跳转和自动隐藏。
- 增加播放倍速和适应/拉伸显示控制。
- 改进全屏和视频画面刷新行为。
- 修复播放音量控制，支持静音/取消静音和更大的默认音量。
- 更新 Windows 构建脚本，支持 `--help`，并在 exe 正在运行时自动重试。

### v1.0.0 (Released 2026-04-26)
- 基础流播放（RTSP、UDP、HTTP）
- 视频录制和截图
- 支持键盘/鼠标全屏
- 网格叠加
- 设置持久化
- 简化 UI，使用合并的播放/暂停按钮

## License

MIT License

## 作者

hywelstar (hywelstar@163.com)

## 文档

- 项目文档索引：docs/README.md
- Windows 构建指南：docs/WINDOWS_BUILD.md
- 构建环境说明：docs/BUILD_ENVIRONMENTS.md
- Android 构建指南：docs/ANDROID_BUILD.md
