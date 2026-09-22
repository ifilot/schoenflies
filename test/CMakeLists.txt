file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(schoenflies_test ${TEST_SOURCES} ../resources.qrc)

target_link_libraries(schoenflies_test
    PRIVATE
        schoenflies_core
        Boost::unit_test_framework
        Qt5::Core
)

target_compile_definitions(schoenflies_test
    PRIVATE
        BOOST_TEST_NO_DLL
)

set_property(TARGET schoenflies_test PROPERTY CXX_SCAN_FOR_MODULES OFF)

add_test(
    NAME schoenflies_tests
    COMMAND schoenflies_test
)
