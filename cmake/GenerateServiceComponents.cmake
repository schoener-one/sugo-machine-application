function(generate_service_components)
    set(options "")
    set(oneValueArgs TARGET CONFIG_FILE)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
                        
    message("Using configuration file:   ${ARG_CONFIG_FILE}")

    if(${ARG_TARGET} STREQUAL "")
        message(FATAL_ERROR "Missing target name")
    endif()

    set(TARGET_LIB ${ARG_TARGET}-GenLib)

    # Create a list of generated output files
    execute_process(
        COMMAND python ${CMAKE_SOURCE_DIR}/ServiceComponents/scripts/GenerateServiceComponents.py -i ${ARG_CONFIG_FILE} -o gen --generate-service-components --list-only
        OUTPUT_VARIABLE GENERATED_OUTPUT_FILES
    )
    string(STRIP ${GENERATED_OUTPUT_FILES} GENERATED_OUTPUT_FILES)
    string(REGEX REPLACE "[ \n\t\r]+" ";" GENERATED_OUTPUT_FILE_LIST ${GENERATED_OUTPUT_FILES})

    # Generate the output files
    add_custom_command(
        OUTPUT ${GENERATED_OUTPUT_FILE_LIST}
        COMMAND python ${CMAKE_SOURCE_DIR}/ServiceComponents/scripts/GenerateServiceComponents.py -i ${ARG_CONFIG_FILE} -o gen --generate-service-components
        DEPENDS ${ARG_CONFIG_FILE}
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
        ServiceComponents
        )
endfunction()

function(generate_service_components_exec_group)
    set(options "")
    set(oneValueArgs TARGET CONFIG_FILE)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
                        
    message("Using configuration file:   ${ARG_CONFIG_FILE}")

    if(${ARG_TARGET} STREQUAL "")
        message(FATAL_ERROR "Missing target name")
    endif()

    set(TARGET_LIB ${ARG_TARGET}-ExecGroup-GenLib)

    # Create a list of generated output files
    execute_process(
        COMMAND python ${CMAKE_SOURCE_DIR}/ServiceComponents/scripts/GenerateServiceComponents.py -i ${ARG_CONFIG_FILE} -o gen --generate-exec-group --list-only
        OUTPUT_VARIABLE GENERATED_OUTPUT_FILES
    )
    string(STRIP ${GENERATED_OUTPUT_FILES} GENERATED_OUTPUT_FILES)
    string(REGEX REPLACE "[ \n\t\r]+" ";" GENERATED_OUTPUT_FILE_LIST ${GENERATED_OUTPUT_FILES})

    # Generate the output files
    add_custom_command(
        OUTPUT ${GENERATED_OUTPUT_FILE_LIST}
        COMMAND python ${CMAKE_SOURCE_DIR}/ServiceComponents/scripts/GenerateServiceComponents.py -i ${ARG_CONFIG_FILE} -o gen --generate-exec-group
        DEPENDS ${ARG_CONFIG_FILE}
    )

    # Filter generated source files
    list(FILTER GENERATED_OUTPUT_FILE_LIST INCLUDE REGEX ".*cpp")
    
    add_library (${TARGET_LIB} STATIC 
        ${GENERATED_OUTPUT_FILE_LIST}
    )
    target_include_directories (${TARGET_LIB} PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/gen/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(${TARGET_LIB} PUBLIC
        Common
        MessageBroker
        ServiceComponents
        ${ARG_TARGET}-GenLib
        HardwareAbstractionLayer # FIXME this dependency should not be add here!
        MachineRemoteControl     # FIXME this dependency should not be add here!
        )
endfunction()
