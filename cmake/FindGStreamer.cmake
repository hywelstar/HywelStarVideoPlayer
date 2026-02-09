# ============================================================================
# FindGStreamer.cmake
# CMake find module for GStreamer multimedia framework
# Supports Windows, Linux, macOS
# ============================================================================

# Set default version
if(NOT DEFINED GStreamer_FIND_VERSION)
    set(GStreamer_FIND_VERSION 1.22.12)
endif()

# Determine GStreamer root directory
if(NOT DEFINED GStreamer_ROOT_DIR)
    if(DEFINED GSTREAMER_ROOT)
        set(GStreamer_ROOT_DIR ${GSTREAMER_ROOT})
    elseif(DEFINED GStreamer_ROOT)
        set(GStreamer_ROOT_DIR ${GStreamer_Root})
    endif()
endif()

# ============================================================================
# Windows Platform
# ============================================================================
if(WIN32)
    if(NOT DEFINED GStreamer_ROOT_DIR)
        if(DEFINED ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64} AND EXISTS "$ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64}")
            set(GStreamer_ROOT_DIR "$ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64}")
        elseif(DEFINED ENV{GSTREAMER_1_0_ROOT_X86_64} AND EXISTS "$ENV{GSTREAMER_1_0_ROOT_X86_64}")
            set(GStreamer_ROOT_DIR "$ENV{GSTREAMER_1_0_ROOT_X86_64}")
        elseif(EXISTS "C:/Program Files/gstreamer/1.0/msvc_x86_64")
            set(GStreamer_ROOT_DIR "C:/Program Files/gstreamer/1.0/msvc_x86_64")
        elseif(EXISTS "C:/gstreamer/1.0/msvc_x86_64")
            set(GStreamer_ROOT_DIR "C:/gstreamer/1.0/msvc_x86_64")
        else()
            message(FATAL_ERROR "GStreamer not found. Please set GSTREAMER_1_0_ROOT_MSVC_X86_64 environment variable or install GStreamer to C:/gstreamer/1.0/msvc_x86_64")
        endif()
    endif()

    cmake_path(CONVERT "${GStreamer_ROOT_DIR}" TO_CMAKE_PATH_LIST GStreamer_ROOT_DIR NORMALIZE)

    if(NOT EXISTS "${GStreamer_ROOT_DIR}")
        message(FATAL_ERROR "GStreamer root directory not found: ${GStreamer_ROOT_DIR}")
    endif()

    set(GSTREAMER_LIB_PATH "${GStreamer_ROOT_DIR}/lib")
    set(GSTREAMER_INCLUDE_PATH "${GStreamer_ROOT_DIR}/include")
    set(GSTREAMER_BIN_PATH "${GStreamer_ROOT_DIR}/bin")

    message(STATUS "GStreamer root: ${GStreamer_ROOT_DIR}")
    message(STATUS "GStreamer lib path: ${GSTREAMER_LIB_PATH}")
    message(STATUS "GStreamer include path: ${GSTREAMER_INCLUDE_PATH}")

    # Find GStreamer libraries
    find_library(GSTREAMER_LIBRARY
        NAMES gstreamer-1.0
        PATHS ${GSTREAMER_LIB_PATH}
        NO_DEFAULT_PATH
    )

    find_library(GSTREAMER_BASE_LIBRARY
        NAMES gstbase-1.0
        PATHS ${GSTREAMER_LIB_PATH}
        NO_DEFAULT_PATH
    )

    find_library(GSTREAMER_VIDEO_LIBRARY
        NAMES gstvideo-1.0
        PATHS ${GSTREAMER_LIB_PATH}
        NO_DEFAULT_PATH
    )

    find_library(GSTREAMER_APP_LIBRARY
        NAMES gstapp-1.0
        PATHS ${GSTREAMER_LIB_PATH}
        NO_DEFAULT_PATH
    )

    find_library(GLIB_LIBRARY
        NAMES glib-2.0
        PATHS ${GSTREAMER_LIB_PATH}
        NO_DEFAULT_PATH
    )

    find_library(GOBJECT_LIBRARY
        NAMES gobject-2.0
        PATHS ${GSTREAMER_LIB_PATH}
        NO_DEFAULT_PATH
    )

    # Set output variables
    set(GStreamer_INCLUDE_DIRS
        ${GSTREAMER_INCLUDE_PATH}
        ${GSTREAMER_INCLUDE_PATH}/gstreamer-1.0
        ${GSTREAMER_LIB_PATH}/gstreamer-1.0/include
        ${GSTREAMER_INCLUDE_PATH}/glib-2.0
        ${GSTREAMER_LIB_PATH}/glib-2.0/include
    )

    set(GStreamer_LIBRARIES
        ${GSTREAMER_LIBRARY}
        ${GSTREAMER_BASE_LIBRARY}
        ${GSTREAMER_VIDEO_LIBRARY}
        ${GSTREAMER_APP_LIBRARY}
        ${GLIB_LIBRARY}
        ${GOBJECT_LIBRARY}
    )

    set(GStreamer_LIBRARY_DIRS ${GSTREAMER_LIB_PATH})

# ============================================================================
# Linux Platform
# ============================================================================
elseif(LINUX)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
    pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-1.0)
    pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)

    set(GStreamer_INCLUDE_DIRS
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_VIDEO_INCLUDE_DIRS}
        ${GSTREAMER_APP_INCLUDE_DIRS}
    )

    set(GStreamer_LIBRARIES
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_VIDEO_LIBRARIES}
        ${GSTREAMER_APP_LIBRARIES}
    )

    set(GStreamer_LIBRARY_DIRS
        ${GSTREAMER_LIBRARY_DIRS}
        ${GSTREAMER_VIDEO_LIBRARY_DIRS}
        ${GSTREAMER_APP_LIBRARY_DIRS}
    )

# ============================================================================
# macOS Platform
# ============================================================================
elseif(APPLE)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
    pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-1.0)
    pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)

    set(GStreamer_INCLUDE_DIRS
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_VIDEO_INCLUDE_DIRS}
        ${GSTREAMER_APP_INCLUDE_DIRS}
    )

    set(GStreamer_LIBRARIES
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_VIDEO_LIBRARIES}
        ${GSTREAMER_APP_LIBRARIES}
    )

    set(GStreamer_LIBRARY_DIRS
        ${GSTREAMER_LIBRARY_DIRS}
        ${GSTREAMER_VIDEO_LIBRARY_DIRS}
        ${GSTREAMER_APP_LIBRARY_DIRS}
    )
endif()

# ============================================================================
# Handle find_package arguments
# ============================================================================
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GStreamer
    FOUND_VAR GStreamer_FOUND
    REQUIRED_VARS GStreamer_INCLUDE_DIRS GStreamer_LIBRARIES
    VERSION_VAR GStreamer_FIND_VERSION
)

if(GStreamer_FOUND)
    message(STATUS "GStreamer found successfully")
    message(STATUS "  Include dirs: ${GStreamer_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${GStreamer_LIBRARIES}")
else()
    message(FATAL_ERROR "GStreamer not found")
endif()

mark_as_advanced(GStreamer_INCLUDE_DIRS GStreamer_LIBRARIES GStreamer_LIBRARY_DIRS)
