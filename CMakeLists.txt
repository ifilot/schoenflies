cmake_minimum_required(VERSION 3.1.0)
project(schoenflies)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Use Boost
set(Boost_NO_SYSTEM_PATHS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

find_package(Boost 1.67.0 COMPONENTS filesystem program_options unit_test_framework REQUIRED)

# Use GLM
find_package(glm REQUIRED)

# Use Qt5
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Use Eigen
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# Set include folders
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${Boost_INCLUDE_DIRS})

# Add test directory
add_subdirectory(test)

# Add sources
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp" "resources.qrc")
add_executable(schoenflies ${SOURCES})

# Libraries
target_link_libraries(schoenflies ${Boost_LIBRARIES} glm Qt5::Widgets Eigen3::Eigen)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
