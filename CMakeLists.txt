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

cmake_minimum_required(VERSION 3.16)
project(SugoMachine VERSION 0.1.0) # TODO Create version automatically from git tag

set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(PROJECT_BINARY_DIR "bin")
message("Source path: ${PROJECT_SOURCE_DIR}")
message("Binary path: ${PROJECT_BINARY_DIR}")

include(cmake/CompilerConfig.cmake)
include(cmake/Coverage.cmake)
include(cmake/LibBoost.cmake)
include(cmake/ExternalLibraries.cmake)

# Enable cppcheck
include(cmake/Cppcheck.cmake)
# Enable clang-format check
include(cmake/ClangFormatCheck.cmake)

# Enable google mock testing
if (NOT NO_UNITTEST)
  enable_testing()
  find_package(GTest REQUIRED)
endif()

## **********************************
## Build the Sugo machine project!
##
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/ServiceComponent/cmake;${CMAKE_MODULE_PATH}") # FIXME remove!
add_subdirectory(libs/Common)
add_subdirectory(libs/MessageBroker)
add_subdirectory(libs/ServiceComponent)
add_subdirectory(libs/ServiceGateway)
add_subdirectory(libs/HardwareAbstractionLayer)
add_subdirectory(libs/HardwareAbstractionLayerImpl)
add_subdirectory(libs/MachineServiceComponent)
add_subdirectory(libs/RemoteControl)

include(cmake/MachineApplication.cmake)
