# CMake configuration file
# Module: HardwareAbstractionLayerTest

# Google Unittests
if (NOT NO_UNITTEST)    
    add_executable(HardwareAbstractionLayerTest
        HardwareAbstractionLayerTest.cpp
        )
    target_compile_options(HardwareAbstractionLayerTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(HardwareAbstractionLayerTest
        HardwareAbstractionLayer
        HardwareAbstractionLayerCommon
        Common
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(HardwareAbstractionLayerTest "" AUTO)
endif()