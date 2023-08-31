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

function(generate_service_components)
    set(options "")
    set(oneValueArgs TARGET CONFIG_FILE)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
                        
    message("Using configuration file:   ${ARG_CONFIG_FILE}")

    if(${ARG_TARGET} STREQUAL "")
        message(FATAL_ERROR "Missing target name")
    endif()

    set(TARGET_LIB ${ARG_TARGET}Generated)

    # Create a list of generated output files
    set(GENERATOR_SCRIPT_PATH "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../scripts/GenerateServiceComponents.py")
    find_package(Python COMPONENTS Interpreter)
    execute_process(
        COMMAND ${Python_EXECUTABLE} ${GENERATOR_SCRIPT_PATH} -i ${ARG_CONFIG_FILE} -o gen --generate-service-components --list-only
        OUTPUT_VARIABLE GENERATED_OUTPUT_FILES
        ERROR_VARIABLE ERROR_OUTPUT
    )

    if(NOT("${ERROR_OUTPUT}" STREQUAL ""))
        message(FATAL_ERROR "${ERROR_OUTPUT}")
    endif()

    string(STRIP ${GENERATED_OUTPUT_FILES} GENERATED_OUTPUT_FILES)
    string(REGEX REPLACE "[ \n\t\r]+" ";" GENERATED_OUTPUT_FILE_LIST ${GENERATED_OUTPUT_FILES})

    # Generate the output files
    add_custom_command(
        OUTPUT ${GENERATED_OUTPUT_FILE_LIST}
        COMMAND ${Python_EXECUTABLE} ${GENERATOR_SCRIPT_PATH} -i ${ARG_CONFIG_FILE} -o gen --generate-service-components
        DEPENDS ${ARG_CONFIG_FILE} ${GENERATOR_SCRIPT_PATH}
    )

    # Filter generated source files
    list(FILTER GENERATED_OUTPUT_FILE_LIST INCLUDE REGEX ".*cpp")

    add_library (${TARGET_LIB} STATIC 
        ${GENERATED_OUTPUT_FILE_LIST}
    )
    target_include_directories (${TARGET_LIB} PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/gen/include
    )
    target_link_libraries(${TARGET_LIB} PUBLIC
        Common
        MessageBroker
        ServiceComponent
        )
endfunction()

