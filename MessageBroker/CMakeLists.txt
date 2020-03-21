# CMake configuration file
# Module: MessageBroker

add_library (MessageBroker INTERFACE)
target_include_directories (MessageBroker INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MessageBroker INTERFACE Common Protocol)

# Google Unittests
if (NOT NO_UNITTEST)    
    add_executable(MessageBrokerTest
        test/MessageBrokerTest.cpp
        )
    target_compile_options(MessageBrokerTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(MessageBrokerTest
        Common
        Protocol
        MessageBroker
        ${Protobuf_LIBRARIES}
        ${Boost_LIBRARIES}
        pthread
        zmq
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(MessageBrokerTest "" AUTO)
endif()