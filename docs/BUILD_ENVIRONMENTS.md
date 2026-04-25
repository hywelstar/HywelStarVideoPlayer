# Build Environments

This page clarifies where builds are verified vs. only documented.

## Verified Environments

### Windows x64 (Verified)

- OS: Windows 11
- Compiler: Visual Studio 2022 (MSVC)
- Qt: 6.10.0 (`msvc2022_64`)
- GStreamer: 1.22.12 (`msvc_x86_64`)
- CMake: 3.16+
- Status: Build and runtime verified in current project iteration

### Android (Partially Verified)

- NDK: 27.2.12479018 (recommended in sample config)
- Qt Android kits:
  - `android_arm64_v8a`
  - `android_x86_64`
- Generator: Ninja
- Status:
  - Build script and local-config workflow are prepared
  - Runtime integration still in progress (not feature-complete)

## Not Yet Verified in Repository CI

### Linux x64

- Current status: not fully verified by maintainers in this repo snapshot
- Recommended baseline for contributors:
  - Ubuntu 22.04/24.04
  - Qt 6.6+
  - GStreamer 1.20+
  - CMake 3.16+

### macOS

- Current status: not verified

## For Contributors on Linux/macOS

If you build on Linux/macOS, please include the following in your PR/issue:

- OS + version
- Qt version and install path
- GStreamer version and plugin packages
- CMake version and compiler version
- Full configure command and first error line (if failed)

This helps us update platform docs quickly and keep build instructions reliable.
