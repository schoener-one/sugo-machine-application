# CMake configuration file
# Module: CoffeeAutomatSmokeTest

# Google Unittests
if (NOT NO_UNITTEST)
    include(GoogleTest)
    add_executable(CoffeeAutomatSmokeTest
        CoffeeAutomatSmokeTest.cpp
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
        gtest
        gtest_main
        gmock)
    gtest_discover_tests(CoffeeAutomatSmokeTest "" AUTO TEST_LIST moco)
endif()