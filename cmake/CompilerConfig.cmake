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

add_compile_options(
    -pipe
    -g
    -Wall -Wextra -Wpedantic -Wno-psabi
    $<$<CONFIG:DEBUG>:-ggdb>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:RELEASE>:-O2>
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    # FIXME Remove NO_TEST_INTERFACE
    add_compile_options(-Werror)
endif()
