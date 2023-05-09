# Google Unittests
# Test helper
add_library(${MODULE_NAME}TestHelper 
    src/MachineServiceComponentsTestHelper.cpp
    )
target_include_directories(${MODULE_NAME}TestHelper PUBLIC include)
target_link_libraries(${MODULE_NAME}TestHelper 
    PUBLIC
    CommonMocks
    PRIVATE
    ${MODULE_NAME}
    )

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
    gmock
    ${MODULE_NAME}TestHelper
    )

