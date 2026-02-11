# ============================================================================
# HywelStar Video Player - CMake Build Configuration
# Author: hywelstar
# Email: hywelstar@126.com
# License: MIT
# ============================================================================

cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Project Configuration
# ============================================================================
project(HywelStarVideoPlayer
    VERSION 1.0.0
    DESCRIPTION "GStreamer-based Video Player"
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
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug or Release)" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")

# Android specific settings
if(ANDROID)
    message(STATUS "Android ABI: ${ANDROID_ABI}")
    message(STATUS "Android Platform: ${ANDROID_PLATFORM}")
    set(CMAKE_ANDROID_API_MIN 21)
    set(CMAKE_ANDROID_STL_TYPE c++_shared)

    # Qt for Android package source directory
    set(QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    message(STATUS "Qt Android package source: ${QT_ANDROID_PACKAGE_SOURCE_DIR}")

    # Set Android SDK path for Qt CMake macros
    set(ANDROID_SDK_ROOT "D:/Software/android_sdk" CACHE PATH "Android SDK root")
    set(QT_ANDROID_SDK_ROOT "${ANDROID_SDK_ROOT}")
endif()

# ============================================================================
# CPM (CMake Package Manager) Configuration
# ============================================================================
include(CPM)
if(NOT CPM_SOURCE_CACHE)
    set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/cpm_modules")
endif()

message(STATUS "CPM cache directory: ${CPM_SOURCE_CACHE}")

# ============================================================================
# Output Directories
# ============================================================================
if(ANDROID)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/bin")
endif()

# ============================================================================
# Qt6 Configuration
# ============================================================================
# For Android, explicitly set Qt6_DIR and related paths
if(ANDROID)
    # Determine Qt path based on ABI
    if(ANDROID_ABI STREQUAL "arm64-v8a")
        set(QT_ANDROID_PATH "F:/QT/6.10.0/android_arm64_v8a")
    elseif(ANDROID_ABI STREQUAL "x86_64")
        set(QT_ANDROID_PATH "F:/QT/6.10.0/android_x86_64")
    elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
        set(QT_ANDROID_PATH "F:/QT/6.10.0/android_armv7")
    elseif(ANDROID_ABI STREQUAL "x86")
        set(QT_ANDROID_PATH "F:/QT/6.10.0/android_x86")
    else()
        set(QT_ANDROID_PATH "F:/QT/6.10.0/android_arm64_v8a")
    endif()

    set(Qt6_DIR "${QT_ANDROID_PATH}/lib/cmake/Qt6" CACHE PATH "Qt6 CMake config directory")
    set(Qt6Core_DIR "${QT_ANDROID_PATH}/lib/cmake/Qt6Core" CACHE PATH "Qt6Core CMake config directory")
    set(Qt6Gui_DIR "${QT_ANDROID_PATH}/lib/cmake/Qt6Gui" CACHE PATH "Qt6Gui CMake config directory")
    set(Qt6Widgets_DIR "${QT_ANDROID_PATH}/lib/cmake/Qt6Widgets" CACHE PATH "Qt6Widgets CMake config directory")
    message(STATUS "Android Qt6_DIR: ${Qt6_DIR}")

    # Qt for Android specific settings
    set(QT_ANDROID_ABIS "${ANDROID_ABI}")
    set(QT_ANDROID_MIN_SDK_VERSION 30)
    set(QT_ANDROID_TARGET_SDK_VERSION 35)
    set(QT_ANDROID_COMPILE_SDK_VERSION 35)
endif()

find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    REQUIRED
)

message(STATUS "Qt6 found: ${Qt6_DIR}")

# ============================================================================
# GStreamer Configuration
# ============================================================================
# For Android, GStreamer support is optional or requires special handling
if(NOT ANDROID)
    find_package(GStreamer 1.22.12 REQUIRED)
    message(STATUS "GStreamer found")
    message(STATUS "  Include dirs: ${GStreamer_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${GStreamer_LIBRARIES}")
else()
    message(STATUS "Android platform: GStreamer support to be added")
    # TODO: Add GStreamer Android support
    set(GStreamer_INCLUDE_DIRS "")
    set(GStreamer_LIBRARIES "")
    set(GStreamer_LIBRARY_DIRS "")
endif()

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

# ============================================================================
# Resources
# ============================================================================
set(RESOURCES
    resources/resources.qrc
)

# ============================================================================
# Create Executable
# ============================================================================
if(ANDROID)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${HEADERS}
        ${RESOURCES}
    )
endif()

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

# Android specific libraries
if(ANDROID)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            android
            log
    )
endif()

# ============================================================================
# Link Directories
# ============================================================================
target_link_directories(${PROJECT_NAME}
    PRIVATE
        ${GStreamer_LIBRARY_DIRS}
)

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
if(ANDROID)
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
else()
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )
endif()

# ============================================================================
# Finalize Android Executable
# ============================================================================
if(ANDROID)
    qt_finalize_executable(${PROJECT_NAME})
endif()

# ============================================================================
# Summary
# ============================================================================
message(STATUS "")
message(STATUS "=== ${PROJECT_NAME} Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "GStreamer Version: 1.22.12")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")
