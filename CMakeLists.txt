cmake_minimum_required(VERSION 3.22)

# IMPORTANT: must be set before project()
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

project(schoenflies LANGUAGES CXX)

# =========================
# C++ setup
# =========================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Extra safety: tell GCC not to use modules TS. AppleClang does not support
# this GCC-specific option.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fno-modules-ts)
endif()
add_compile_definitions(
    EIGEN_NO_DEBUG
    EIGEN_DONT_VECTORIZE
    EIGEN_MAX_ALIGN_BYTES=0
)

# Qt automoc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# =========================
# Dependencies
# =========================
find_package(Boost CONFIG REQUIRED filesystem program_options unit_test_framework)
find_package(glm REQUIRED)
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Svg
    SvgWidgets
    OpenGL
    OpenGLWidgets
)
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(nlohmann_json 3.9.0 REQUIRED)
find_package(Freetype REQUIRED)

# =========================
# Core library (ALL project code except main)
# =========================
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(schoenflies_core STATIC
    ${CORE_SOURCES}
    resources.qrc
)

target_include_directories(schoenflies_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(schoenflies_core
    PUBLIC
        Boost::filesystem
        Boost::program_options
        glm::glm
        Qt6::Widgets
        Qt6::Svg
        Qt6::SvgWidgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        Eigen3::Eigen
        nlohmann_json::nlohmann_json
        Freetype::Freetype
)

set_property(TARGET schoenflies_core PROPERTY CXX_SCAN_FOR_MODULES OFF)

# =========================
# Main executable
# =========================

set(APP_SOURCES
    src/main.cpp
    resources.qrc
)

if (WIN32)
    list(APPEND APP_SOURCES resources.rc)
endif()

if(APPLE)
    add_executable(schoenflies MACOSX_BUNDLE ${APP_SOURCES})
else()
    add_executable(schoenflies ${APP_SOURCES})
endif()

if (WIN32)
    set_target_properties(schoenflies PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(APPLE)
    set_target_properties(schoenflies PROPERTIES
        OUTPUT_NAME "Schoenflies"
        MACOSX_BUNDLE_BUNDLE_NAME "Schoenflies"
        MACOSX_BUNDLE_GUI_IDENTIFIER "nl.tue.schoenflies"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.3.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.3.0"
    )
endif()

target_link_libraries(schoenflies PRIVATE schoenflies_core Qt6::Widgets)
set_property(TARGET schoenflies PROPERTY CXX_SCAN_FOR_MODULES OFF)

# =========================
# Tests
# =========================
include(CTest)
enable_testing()
add_subdirectory(test)
