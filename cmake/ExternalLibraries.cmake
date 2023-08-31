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

include(FindGit)
find_package(Git)

if (NOT Git_FOUND)
    message(FATAL_ERROR "Git not found!")
endif ()

include(ExternalProject)

# azmq
ExternalProject_Add(azmq_external
    GIT_REPOSITORY    https://github.com/zeromq/azmq.git
    GIT_TAG           v1.0.3
    GIT_SHALLOW       ON
    PREFIX            azmq
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -B <BINARY_DIR> -S <SOURCE_DIR> -GNinja -DAZMQ_NO_TESTS=1 -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    BUILD_COMMAND     ${CMAKE_COMMAND} --build <BINARY_DIR>
    BUILD_ALWAYS      OFF
)
ExternalProject_Get_Property(azmq_external INSTALL_DIR)
message("azmq_external install dir: ${INSTALL_DIR}")
include_directories(${INSTALL_DIR}/include)

# jsonrpcpp
ExternalProject_Add(jsonrpcpp_external
    GIT_REPOSITORY    https://github.com/badaix/jsonrpcpp.git
    GIT_TAG           v1.3.2
    GIT_SHALLOW       ON
    PREFIX            jsonrpcpp
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -B <BINARY_DIR> -S <SOURCE_DIR> -GNinja -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    BUILD_COMMAND     ${CMAKE_COMMAND} --build <BINARY_DIR>
    BUILD_ALWAYS      OFF
)
ExternalProject_Get_Property(jsonrpcpp_external INSTALL_DIR)
message("jsonrpcpp_external install dir: ${INSTALL_DIR}")
include_directories(${INSTALL_DIR}/include)

# mongoose
find_program(MAKE_COMMAND NAMES make REQUIRED)
set(MONGOOSE_CFLAGS "${CMAKE_CPP_FLAGS} -DMG_ENABLE_PACKED_FS=0 -fPIC")
ExternalProject_Add(mongoose_external
    GIT_REPOSITORY    https://github.com/cesanta/mongoose.git
    GIT_TAG           7.8
    GIT_SHALLOW       ON
    PREFIX            mongoose
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} ${MAKE_COMMAND} uninstall install PREFIX= CFLAGS=${MONGOOSE_CFLAGS} SSL=OPENSSL -C <SOURCE_DIR> DESTDIR=<INSTALL_DIR>
    BUILD_ALWAYS      OFF
    BUILD_IN_SOURCE   ON
    PATCH_COMMAND     mkdir -p <INSTALL_DIR>/lib
)
ExternalProject_Get_Property(mongoose_external INSTALL_DIR)
message("mongoose_external install dir: ${INSTALL_DIR}")
include_directories(${INSTALL_DIR}/include)
link_directories(${INSTALL_DIR}/lib)

add_custom_target(ExternalLibraries
    DEPENDS mongoose_external jsonrpcpp_external azmq_external)
