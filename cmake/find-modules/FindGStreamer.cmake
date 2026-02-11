# ============================================================================
# FindGStreamer.cmake
# CMake find module for GStreamer multimedia framework
#
# This module finds GStreamer and sets the following variables:
#   GStreamer_FOUND        - True if GStreamer was found
#   GStreamer_VERSION      - GStreamer version
#   GStreamer_INCLUDE_DIRS - Include directories
#   GStreamer_LIBRARIES    - Libraries to link
#   GStreamer_LIBRARY_DIRS - Library directories
#   GStreamer_ROOT_DIR     - GStreamer installation root
#
# Supported platforms: Windows, Linux, macOS
# ============================================================================

# ----------------------------------------------------------------------------
# Default Version
# ----------------------------------------------------------------------------
if(NOT DEFINED GStreamer_FIND_VERSION)
    set(GStreamer_FIND_VERSION 1.20)
endif()

# ----------------------------------------------------------------------------
# Find GStreamer Root Directory
# ----------------------------------------------------------------------------
if(NOT DEFINED GStreamer_ROOT_DIR)
    if(WIN32)
        # Windows: Check environment variables and common paths
        if(DEFINED ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64} AND EXISTS "$ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64}")
            set(GStreamer_ROOT_DIR "$ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64}")
        elseif(DEFINED ENV{GSTREAMER_1_0_ROOT_X86_64} AND EXISTS "$ENV{GSTREAMER_1_0_ROOT_X86_64}")
            set(GStreamer_ROOT_DIR "$ENV{GSTREAMER_1_0_ROOT_X86_64}")
        elseif(DEFINED ENV{GSTREAMER_1_0_ROOT_MINGW_X86_64} AND EXISTS "$ENV{GSTREAMER_1_0_ROOT_MINGW_X86_64}")
            set(GStreamer_ROOT_DIR "$ENV{GSTREAMER_1_0_ROOT_MINGW_X86_64}")
        elseif(EXISTS "C:/gstreamer/1.0/msvc_x86_64")
            set(GStreamer_ROOT_DIR "C:/gstreamer/1.0/msvc_x86_64")
        elseif(EXISTS "D:/gstreamer/1.0/msvc_x86_64")
            set(GStreamer_ROOT_DIR "D:/gstreamer/1.0/msvc_x86_64")
        endif()
    elseif(APPLE)
        # macOS: Check framework and Homebrew paths
        if(EXISTS "/Library/Frameworks/GStreamer.framework/Versions/1.0")
            set(GStreamer_ROOT_DIR "/Library/Frameworks/GStreamer.framework/Versions/1.0")
        elseif(EXISTS "/opt/homebrew/opt/gstreamer")
            set(GStreamer_ROOT_DIR "/opt/homebrew/opt/gstreamer")
        elseif(EXISTS "/usr/local/opt/gstreamer")
            set(GStreamer_ROOT_DIR "/usr/local/opt/gstreamer")
        endif()
    else()
        # Linux: Use system paths
        if(EXISTS "/usr")
            set(GStreamer_ROOT_DIR "/usr")
        endif()
    endif()
endif()

# Normalize path
if(GStreamer_ROOT_DIR)
    cmake_path(CONVERT "${GStreamer_ROOT_DIR}" TO_CMAKE_PATH_LIST GStreamer_ROOT_DIR NORMALIZE)
endif()

# ----------------------------------------------------------------------------
# Validate Installation
# ----------------------------------------------------------------------------
if(NOT EXISTS "${GStreamer_ROOT_DIR}")
    if(GStreamer_FIND_REQUIRED)
        message(FATAL_ERROR
            "GStreamer not found.\n"
            "Please install GStreamer from https://gstreamer.freedesktop.org/download/\n"
            "Windows: Install MSVC 64-bit runtime + development packages\n"
            "Linux: sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev\n"
            "macOS: brew install gstreamer"
        )
    endif()
    return()
endif()

# ----------------------------------------------------------------------------
# Set Paths
# ----------------------------------------------------------------------------
if(WIN32)
    set(GSTREAMER_LIB_PATH "${GStreamer_ROOT_DIR}/lib")
    set(GSTREAMER_INCLUDE_PATH "${GStreamer_ROOT_DIR}/include")
    set(GSTREAMER_BIN_PATH "${GStreamer_ROOT_DIR}/bin")
elseif(APPLE)
    set(GSTREAMER_LIB_PATH "${GStreamer_ROOT_DIR}/lib")
    set(GSTREAMER_INCLUDE_PATH "${GStreamer_ROOT_DIR}/include")
else()
    # Linux: Handle multiarch
    if(EXISTS "${GStreamer_ROOT_DIR}/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu")
        set(GSTREAMER_LIB_PATH "${GStreamer_ROOT_DIR}/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu")
    else()
        set(GSTREAMER_LIB_PATH "${GStreamer_ROOT_DIR}/lib")
    endif()
    set(GSTREAMER_INCLUDE_PATH "${GStreamer_ROOT_DIR}/include")
endif()

# ----------------------------------------------------------------------------
# Use pkg-config for detailed discovery
# ----------------------------------------------------------------------------
find_package(PkgConfig QUIET)

if(PKG_CONFIG_FOUND)
    # Set up pkg-config environment
    if(WIN32)
        set(PKG_CONFIG_EXECUTABLE "${GStreamer_ROOT_DIR}/bin/pkg-config.exe")
        set(ENV{PKG_CONFIG} "${PKG_CONFIG_EXECUTABLE}")
        set(ENV{PKG_CONFIG_PATH} "${GSTREAMER_LIB_PATH}/pkgconfig")
    else()
        set(ENV{PKG_CONFIG_PATH} "${GSTREAMER_LIB_PATH}/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    endif()

    pkg_check_modules(PC_GSTREAMER gstreamer-1.0>=${GStreamer_FIND_VERSION})
    pkg_check_modules(PC_GSTREAMER_VIDEO gstreamer-video-1.0)
    pkg_check_modules(PC_GSTREAMER_APP gstreamer-app-1.0)

    if(PC_GSTREAMER_FOUND)
        set(GStreamer_VERSION "${PC_GSTREAMER_VERSION}")
    endif()
endif()

# ----------------------------------------------------------------------------
# Find Include Directories
# ----------------------------------------------------------------------------
find_path(GStreamer_INCLUDE_DIR
    NAMES gst/gst.h
    PATHS
        "${GSTREAMER_INCLUDE_PATH}/gstreamer-1.0"
        "${GSTREAMER_INCLUDE_PATH}"
    PATH_SUFFIXES gstreamer-1.0
)

find_path(GLib_INCLUDE_DIR
    NAMES glib.h
    PATHS
        "${GSTREAMER_INCLUDE_PATH}/glib-2.0"
        "${GSTREAMER_INCLUDE_PATH}"
    PATH_SUFFIXES glib-2.0
)

find_path(GLib_Config_INCLUDE_DIR
    NAMES glibconfig.h
    PATHS
        "${GSTREAMER_LIB_PATH}/glib-2.0/include"
        "${GSTREAMER_INCLUDE_PATH}"
)

# ----------------------------------------------------------------------------
# Find Libraries
# ----------------------------------------------------------------------------
find_library(GStreamer_LIBRARY
    NAMES gstreamer-1.0
    PATHS "${GSTREAMER_LIB_PATH}"
)

find_library(GStreamer_Base_LIBRARY
    NAMES gstbase-1.0
    PATHS "${GSTREAMER_LIB_PATH}"
)

find_library(GStreamer_Video_LIBRARY
    NAMES gstvideo-1.0
    PATHS "${GSTREAMER_LIB_PATH}"
)

find_library(GStreamer_App_LIBRARY
    NAMES gstapp-1.0
    PATHS "${GSTREAMER_LIB_PATH}"
)

find_library(GObject_LIBRARY
    NAMES gobject-2.0
    PATHS "${GSTREAMER_LIB_PATH}"
)

find_library(GLib_LIBRARY
    NAMES glib-2.0
    PATHS "${GSTREAMER_LIB_PATH}"
)

# ----------------------------------------------------------------------------
# Set Output Variables
# ----------------------------------------------------------------------------
set(GStreamer_INCLUDE_DIRS
    ${GStreamer_INCLUDE_DIR}
    ${GLib_INCLUDE_DIR}
    ${GLib_Config_INCLUDE_DIR}
)

set(GStreamer_LIBRARIES
    ${GStreamer_LIBRARY}
    ${GStreamer_Base_LIBRARY}
    ${GStreamer_Video_LIBRARY}
    ${GStreamer_App_LIBRARY}
    ${GObject_LIBRARY}
    ${GLib_LIBRARY}
)

set(GStreamer_LIBRARY_DIRS
    ${GSTREAMER_LIB_PATH}
)

# Get version if not already set
if(NOT GStreamer_VERSION AND EXISTS "${GStreamer_INCLUDE_DIR}/gst/gstversion.h")
    file(STRINGS "${GStreamer_INCLUDE_DIR}/gst/gstversion.h" _gst_version_major
        REGEX "^#define GST_VERSION_MAJOR \\([0-9]+\\)")
    file(STRINGS "${GStreamer_INCLUDE_DIR}/gst/gstversion.h" _gst_version_minor
        REGEX "^#define GST_VERSION_MINOR \\([0-9]+\\)")
    file(STRINGS "${GStreamer_INCLUDE_DIR}/gst/gstversion.h" _gst_version_micro
        REGEX "^#define GST_VERSION_MICRO \\([0-9]+\\)")

    string(REGEX REPLACE ".*\\(([0-9]+)\\).*" "\\1" _major "${_gst_version_major}")
    string(REGEX REPLACE ".*\\(([0-9]+)\\).*" "\\1" _minor "${_gst_version_minor}")
    string(REGEX REPLACE ".*\\(([0-9]+)\\).*" "\\1" _micro "${_gst_version_micro}")

    set(GStreamer_VERSION "${_major}.${_minor}.${_micro}")
endif()

# ----------------------------------------------------------------------------
# Handle Standard Args
# ----------------------------------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GStreamer
    REQUIRED_VARS
        GStreamer_LIBRARY
        GStreamer_INCLUDE_DIR
    VERSION_VAR GStreamer_VERSION
)

mark_as_advanced(
    GStreamer_INCLUDE_DIR
    GStreamer_LIBRARY
    GStreamer_Base_LIBRARY
    GStreamer_Video_LIBRARY
    GStreamer_App_LIBRARY
    GLib_INCLUDE_DIR
    GLib_Config_INCLUDE_DIR
    GObject_LIBRARY
    GLib_LIBRARY
)
