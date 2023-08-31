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

target_sources(${MODULE_NAME}
    PRIVATE
        src/GpioControl.cpp
        src/TemperatureSensorControl.cpp
        src/StepperMotorControl.cpp
        src/GpioPin.cpp
        src/TemperatureSensor.cpp
        src/StepperMotor.cpp
        src/Max31865.cpp
        src/SpiControl.cpp
        src/I2cControl.cpp
        src/TicController.cpp
        src/StepperMotor.cpp
        src/HardwareAbstractionLayer.cpp
    )
target_include_directories (${MODULE_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
target_link_libraries(${MODULE_NAME}
    PRIVATE
        gpiod
        gpiodcxx
    )

