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
set(MODULE_NAME HardwareAbstractionLayerImpl)

# Build library
add_library (${MODULE_NAME}
    src/HardwareAbstractionLayer.cpp
    src/Configuration.cpp
)
target_include_directories (${MODULE_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${MODULE_NAME}
    PUBLIC
        HardwareAbstractionLayer
        Common
)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    add_compile_definitions(HAL_SIMULATION_ENABLE)
    message("Using HardwareAbstractionLayerStub (x86)")
    add_subdirectory(Stub)
else()
    message("Using HardwareAbstractionLayerRpi")
    add_subdirectory(Rpi)
endif()

# Build tests
if (NOT NO_TESTS)
    add_subdirectory(test)
endif()

# Install configuration
file(GLOB HEADER_FILES include/${MODULE_NAME}/*.hpp)
set_target_properties(${MODULE_NAME} PROPERTIES
    PUBLIC_HEADER "${HEADER_FILES}"
)
include(GNUInstallDirs)
install(TARGETS ${MODULE_NAME}
    EXPORT ${MODULE_NAME}Config
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${MODULE_NAME}
)
# TODO
# install(EXPORT ${MODULE_NAME}Config
#     # NAMESPACE Sugo::
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${MODULE_NAME}
# )
