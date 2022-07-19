# CMake configuration file
# Module: ${MODULE_NAME}
set(MODULE_NAME "MachineApplication")
generate_service_component(
    TARGET ${MODULE_NAME}
    CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/MachineServiceComponents.yaml"
    )
add_executable(${MODULE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FilamentCoilControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FilamentCoilMotor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FilamentFeederMotor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FilamentMergerControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FilamentMergerHeater.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FilamentPreHeater.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FilamentTensionSensor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MachineApplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MachineControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UserInterfaceControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    )
target_include_directories (${MODULE_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
target_link_libraries(${MODULE_NAME}
    Common
    Protocol
    MessageBroker
    MachineServiceGateway
    ${MODULE_NAME}-GenLib
    HardwareAbstractionLayer
    )
    
 # Google Unittests
if (NOT NO_UNITTEST)
    add_executable(MachineSmokeTest
        test/MachineSmokeTest.cpp
        )
    target_compile_options(MachineSmokeTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(MachineSmokeTest
        Common
        CommonMocks
        Protocol
        MessageBroker
        MessageBrokerMocks
        MachineServiceGateway
        ${Protobuf_LIBRARIES}
        ${Boost_LIBRARIES}
        pthread
        zmq
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(MachineSmokeTest "" AUTO TEST_LIST sugo)
endif()
