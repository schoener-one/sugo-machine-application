#!/bin/bash
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
BUILD_DIR=${1:-build}
shift 1
CHECK_DIR_LIST=$@
CHECK_LIST=( \
       'clang-analyzer-*' \
       'clang-analyzer-cplusplus*' \
       'concurrency-*' \
       'cppcoreguidelines-*' \
       '-cppcoreguidelines-special-member-functions' \
       '-cppcoreguidelines-macro-usage' \
       '-cppcoreguidelines-pro-bounds-array-to-pointer-decay' \
       'modernize-*' \
       '-modernize-use-nodiscard' \
       '-modernize-use-trailing-return-type' \
       'performance-*' \
)
CHECK_LIST_JOINED=$(printf "%s," "${CHECK_LIST[@]}")
find ${CHECK_DIR_LIST} | \
    grep -E "\\.hpp|\\.cpp" | \
    grep -vi test | \
    xargs clang-tidy -p ${BUILD_DIR} --checks="-*,${CHECK_LIST_JOINED}"