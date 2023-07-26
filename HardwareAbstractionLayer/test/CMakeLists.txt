# CMake configuration file
# Module: HardwareAbstractionLayerSmokeTest

# Google Unittests
if (NOT NO_UNITTEST)    
    add_executable(HardwareAbstractionLayerSmokeTest
        HardwareAbstractionLayerSmokeTest.cpp
        )
    target_compile_options(HardwareAbstractionLayerSmokeTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(HardwareAbstractionLayerSmokeTest
        HardwareAbstractionLayer
        Common
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(HardwareAbstractionLayerSmokeTest "" AUTO)
    configure_file("HardwareAbstractionLayerSmokeTestConfig.json"
        "HardwareAbstractionLayerSmokeTestConfig.json" COPYONLY)
endif()