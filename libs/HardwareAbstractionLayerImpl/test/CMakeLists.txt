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

set(MODULE_TEST_APP ${MODULE_NAME}Test)
find_package(GTest REQUIRED)
enable_testing()
add_executable(${MODULE_TEST_APP}
    HardwareAbstractionLayerTest.cpp
    HardwareAbstractionLayerSmokeTest.cpp
    )
target_compile_options(${MODULE_TEST_APP} PUBLIC "-DUNIT_TEST")
target_link_libraries(${MODULE_TEST_APP}
    ${MODULE_NAME}
    pthread
    GTest::GTest
    GTest::Main
    gmock)
gtest_add_tests(${MODULE_TEST_APP} "" AUTO)

configure_file("HardwareAbstractionLayerSmokeTestConfig.json"
    "HardwareAbstractionLayerSmokeTestConfig.json" COPYONLY)
