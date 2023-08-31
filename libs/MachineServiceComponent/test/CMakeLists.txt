###################################################################################################
# 
# @file
#  
# @author: Denis Schoener (denis@schoener-one.de)
# @date: 22.09.2023
#  
# @license: Copyright (C) 2020 by Denis Schoener
# 
# This program is free software: you can redistribute it andor modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <https:www.gnu.orglicenses>.
#
###################################################################################################

# Integration Tests
set(MODULE_TEST_APP ${MODULE_NAME}Test)
find_package(GTest REQUIRED)
enable_testing()
# Helper lib
add_library(${MODULE_TEST_APP}Helper
    src/MachineConfiguration.cpp
)
target_include_directories(${MODULE_TEST_APP}Helper
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${MODULE_TEST_APP}Helper
    PRIVATE
        ${MODULE_NAME}
    PUBLIC
        Common
        CommonMocks
)
# Module test application
add_executable(${MODULE_TEST_APP}
    ${MODULE_NAME}Test.cpp
)
target_compile_options(${MODULE_TEST_APP} PUBLIC "-DUNIT_TEST")
target_link_libraries(${MODULE_TEST_APP}
    Common
    CommonMocks
    MessageBrokerMocks
    HardwareAbstractionLayerMocks
    ${MODULE_NAME}
    ${MODULE_TEST_APP}Helper
    ${Boost_LIBRARIES}
    pthread
    GTest::GTest
    GTest::Main
    gmock
)
gtest_add_tests(${MODULE_TEST_APP} "" AUTO)