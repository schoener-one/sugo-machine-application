# Google Unittests

# Integration Tests
add_executable(MessageBrokerIntegrationTest
    CommandMessageBrokerIntegrationTest.cpp
    ClientServerIntegrationTest.cpp
    PublisherSubscriberIntegrationTest.cpp
    )
target_compile_options(MessageBrokerIntegrationTest PUBLIC "-DUNIT_TEST")
target_link_libraries(MessageBrokerIntegrationTest
    Common
    MessageBroker
    ${Protobuf_LIBRARIES}
    ${Boost_LIBRARIES}
    pthread
    GTest::GTest
    GTest::Main
    gmock)
gtest_add_tests(MessageBrokerIntegrationTest "" AUTO)
