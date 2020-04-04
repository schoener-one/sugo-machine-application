# CMake configuration file
# Module: CoffeeAutomatApplication

add_executable(CoffeeAutomatApplication
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CoffeeAutomatApplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    )
target_include_directories (CoffeeAutomatApplication PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CoffeeAutomatApplication
    Common
    Protocol
    MessageBroker
    CoffeeAutomatService
    CupRotationTray
    CoffeeAutomat
    HardwareComponents
    HardwareAbstractionLayer
    ${HardwareAbstractionLayerImpl}
    )
    
 # Google Unittests
if (NOT NO_UNITTEST)
    add_executable(CoffeeAutomatSmokeTest
        test/CoffeeAutomatSmokeTest.cpp
        )
    target_compile_options(CoffeeAutomatSmokeTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(CoffeeAutomatSmokeTest
        Common
        Protocol
        MessageBroker
        CoffeeAutomatService
        CupRotationTray
        CoffeeAutomat
        ${Protobuf_LIBRARIES}
        ${Boost_LIBRARIES}
        pthread
        zmq
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(CoffeeAutomatSmokeTest "" AUTO TEST_LIST moco)
endif()