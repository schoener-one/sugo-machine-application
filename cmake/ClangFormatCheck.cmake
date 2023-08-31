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

# FIXME remove globbing!
file(GLOB_RECURSE SOURCE_ALL 
    ${PROJECT_SOURCE_DIR}/*.cpp
)
list(FILTER SOURCE_ALL EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*" )
file(GLOB_RECURSE HEADER_ALL 
    ${PROJECT_SOURCE_DIR}/*.hpp
)
list(FILTER HEADER_ALL EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*" )

set(CLANG_FORMAT_LOG ${CMAKE_CURRENT_BINARY_DIR}/clang-format.log)
add_custom_command(
    OUTPUT            ${CLANG_FORMAT_LOG}
    COMMENT           "Checking clang-format"
    COMMAND           clang-format --dry-run --Werror ${SOURCE_ALL} ${HEADER_ALL} 2> ${CLANG_FORMAT_LOG}
    )

add_custom_target(clang_format_check
    ALL
    DEPENDS ${CLANG_FORMAT_LOG})