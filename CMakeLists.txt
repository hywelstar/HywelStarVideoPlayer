# ============================================================================
# HywelStar Video Player - CMake Build Configuration
# ============================================================================
#
# Prerequisites:
#   - Qt 6.6+ (https://www.qt.io/download)
#   - GStreamer 1.20+ (https://gstreamer.freedesktop.org/download/)
#
# Build Instructions:
#   Option 1: Use Qt Creator (Recommended)
#     - Open CMakeLists.txt in Qt Creator
#     - Configure and build
#
#   Option 2: Command Line
#     cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/msvc2022_64
#     cmake --build build --config Release
#
# ============================================================================

cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Project Configuration
# ============================================================================
project(HywelStarVideoPlayer
    VERSION 1.0.0
    DESCRIPTION "GStreamer-based Video Player"
    HOMEPAGE_URL "https://github.com/hywelstar/HywelStarVideoPlayer"
    LANGUAGES CXX
)

# ============================================================================
# CMake Module Paths
# ============================================================================
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    "${CMAKE_SOURCE_DIR}/cmake/find-modules"
)

# ============================================================================
# Build Configuration
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

message(STATUS "")
message(STATUS "=== HywelStar Video Player Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")

# ============================================================================
# Output Directories (Qt Creator style)
# ============================================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

# ============================================================================
# Qt6 Configuration
# ============================================================================
find_package(Qt6 6.6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

if(NOT Qt6_FOUND)
    message(FATAL_ERROR
        "Qt6 6.6+ not found.\n"
        "Set CMAKE_PREFIX_PATH to your Qt installation, e.g.:\n"
        "  cmake -DCMAKE_PREFIX_PATH=C:/Qt/6.8.0/msvc2022_64 ..\n"
        "Or use qt-cmake which sets this automatically."
    )
endif()

message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "Qt6 Path: ${Qt6_DIR}")

# ============================================================================
# GStreamer Configuration
# ============================================================================
find_package(GStreamer 1.20 REQUIRED)

message(STATUS "GStreamer Version: ${GStreamer_VERSION}")
message(STATUS "GStreamer Path: ${GStreamer_ROOT_DIR}")

# ============================================================================
# Source Files
# ============================================================================
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/core/GStreamerEngine.cpp
    src/core/RecordingManager.cpp
    src/core/ConfigManager.cpp
    src/ui/VideoDisplayWidget.cpp
    src/ui/ControlBar.cpp
    src/ui/StatusBar.cpp
    src/ui/QuickConnectBar.cpp
    src/ui/SettingsDialog.cpp
    src/ui/AboutDialog.cpp
    src/utils/Logger.cpp
    src/utils/StreamParser.cpp
)

set(HEADERS
    src/MainWindow.h
    src/core/GStreamerEngine.h
    src/core/RecordingManager.h
    src/core/ConfigManager.h
    src/ui/VideoDisplayWidget.h
    src/ui/ControlBar.h
    src/ui/StatusBar.h
    src/ui/QuickConnectBar.h
    src/ui/SettingsDialog.h
    src/ui/AboutDialog.h
    src/utils/Logger.h
    src/utils/StreamParser.h
)

set(RESOURCES
    resources/resources.qrc
)

# ============================================================================
# Create Executable
# ============================================================================
qt_add_executable(${PROJECT_NAME}
    WIN32
    MACOSX_BUNDLE
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${GStreamer_INCLUDE_DIRS}
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${GStreamer_LIBRARIES}
)

# ============================================================================
# Link Directories
# ============================================================================
if(GStreamer_LIBRARY_DIRS)
    target_link_directories(${PROJECT_NAME}
        PRIVATE
            ${GStreamer_LIBRARY_DIRS}
    )
endif()

# ============================================================================
# Compiler Flags
# ============================================================================
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /Zc:__cplusplus)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ============================================================================
# Installation
# ============================================================================
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ============================================================================
# Build Summary
# ============================================================================
message(STATUS "")
message(STATUS "=== Build Summary ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")
