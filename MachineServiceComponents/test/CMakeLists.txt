# Google Unittests
add_executable(${MODULE_NAME}Test
    ${MODULE_NAME}Test.cpp
    )
target_compile_options(${MODULE_NAME}Test PUBLIC "-DUNIT_TEST")
target_link_libraries(${MODULE_NAME}Test
    ${MODULE_NAME}
    CommonMocks
    MessageBrokerMocks
    HardwareAbstractionLayerInterfaceMocks
    ${Protobuf_LIBRARIES}
    ${Boost_LIBRARIES}
    GTest::GTest
    GTest::Main
    gmock)
#gtest_add_tests(${MODULE_NAME}Test "" AUTO TEST_LIST sugo)