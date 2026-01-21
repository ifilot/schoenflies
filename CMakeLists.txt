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

# Extra safety: tell GCC not to use modules TS
add_compile_options(-fno-modules-ts)
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
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
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
        Qt5::Widgets
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

add_executable(schoenflies ${APP_SOURCES})

if (WIN32)
    set_target_properties(schoenflies PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

target_link_libraries(schoenflies PRIVATE schoenflies_core Qt5::Widgets)
set_property(TARGET schoenflies PROPERTY CXX_SCAN_FOR_MODULES OFF)

# =========================
# Tests
# =========================
include(CTest)
enable_testing()
add_subdirectory(test)
