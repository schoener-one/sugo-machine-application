# CMake configuration file
# Module: MachineApplication

add_executable(MachineApplication
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MachineApplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    )
target_include_directories (MachineApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MachineApplication
    Common
    Protocol
    MessageBroker
    MachineService
    CupRotationTray
    MachineController
    HardwareComponents
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
        MachineService
        CupRotationTray
        MachineController
        ${Protobuf_LIBRARIES}
        ${Boost_LIBRARIES}
        pthread
        zmq
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(MachineSmokeTest "" AUTO TEST_LIST sugo)
endif()
