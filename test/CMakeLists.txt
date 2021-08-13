# Add sources
file(GLOB TEST_SOURCES "*.cpp" "resources.qrc")
file(GLOB SRC_SOURCES "../src/*.cpp" "../src/*/*.cpp" "../src/*/*/*.cpp" "../src/resources.qrc")
list(FILTER SRC_SOURCES EXCLUDE REGEX ".*/src/main.cpp$")
add_executable(schoenflies_test ${TEST_SOURCES} ${SRC_SOURCES})

# Libraries
target_link_libraries(schoenflies_test ${Boost_LIBRARIES} glm Qt5::Widgets Eigen3::Eigen)
