cmake_minimum_required(VERSION 3.1.0)
project(schoenflies)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Use Qt5
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Set include folders
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR})

# Add sources
file(GLOB SOURCES "src/*.cpp")
add_executable(schoenflies ${SOURCES})

# Set C++17
add_definitions(-std=c++17 -march=native)

# Libraries
target_link_libraries(schoenflies Qt5::Widgets)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
