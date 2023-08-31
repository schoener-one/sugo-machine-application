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

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(SRC_Protobuf HDR_Protobuf Message.proto)
get_filename_component(${HDR_Protobuf} DIR_Protobuf DIRECTORY)
set(MODULE "MessageBrokerProto")
add_library(${MODULE} SHARED ${SRC_Protobuf} ${HDR_Protobuf})
target_link_libraries(${MODULE} INTERFACE ${Protobuf_LIBRARIES})
target_include_directories (${MODULE} PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories (${MODULE} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
