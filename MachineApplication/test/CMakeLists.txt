# Google Unittests

# Integration test
add_executable(${MODULE_NAME}IntegrationTest
    ${MODULE_NAME}IntegrationTest.cpp
    )
target_compile_options(${MODULE_NAME}IntegrationTest PUBLIC "-DUNIT_TEST")
target_link_libraries(${MODULE_NAME}IntegrationTest
    Common
    MessageBroker
    MachineServiceComponents-ExecGroup-GenLib
    MachineServiceComponents
    HardwareAbstractionLayerInterfaceMocks
    ${Protobuf_LIBRARIES}
    ${Boost_LIBRARIES}
    GTest::GTest
    GTest::Main
    gmock)
gtest_add_tests(${MODULE_NAME}IntegrationTest "" AUTO TEST_LIST sugo)