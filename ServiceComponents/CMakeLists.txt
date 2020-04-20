# CMake configuration file
# Module: ServiceComponents

add_library (ServiceComponents STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CommandId.cpp
    )
target_include_directories (ServiceComponents PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ServiceComponents Common MessageBroker)

# Google Unittests
if (NOT NO_UNITTEST)    
    add_executable(ServiceComponentsTest
        test/ServiceComponentTest.cpp
        )
    target_compile_options(ServiceComponentsTest PUBLIC "-DUNIT_TEST")
    target_link_libraries(ServiceComponentsTest
        Common
        CommonMocks
        MessageBrokerMocks
        ServiceComponents
        ${Boost_LIBRARIES}
        pthread
        GTest::GTest
        GTest::Main
        gmock)
    gtest_add_tests(ServiceComponentsTest "" AUTO)
endif()