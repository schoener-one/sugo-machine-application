function(generate_service_component)
    set(options "")
    set(oneValueArgs TARGET CONFIG_FILE)
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN} )
                        
    message("Generating code for target: ${ARG_TARGET}")
    message("Using configuration file:   ${ARG_CONFIG_FILE}")

    if(${ARG_TARGET} STREQUAL "")
        message(FATAL_ERROR "Missing target name")
    endif()

    set(TARGET_LIB ${ARG_TARGET}-GenLib)

    # Create a list of generated output files
    execute_process(
        COMMAND python ${CMAKE_SOURCE_DIR}/ServiceComponents/scripts/GenerateServiceComponent.py -i ${ARG_CONFIG_FILE} --list-only
        OUTPUT_VARIABLE GENERATED_OUTPUT_FILES
    )
    string(STRIP ${GENERATED_OUTPUT_FILES} GENERATED_OUTPUT_FILES)
    string(REGEX REPLACE "[ \n\t\r]+" ";" GENERATED_OUTPUT_FILE_LIST ${GENERATED_OUTPUT_FILES})
    list(LENGTH GENERATED_OUTPUT_FILE_LIST GENERATED_OUTPUT_FILES_len)

    # Generate the output files
    add_custom_command(
        OUTPUT ${GENERATED_OUTPUT_FILE_LIST}
        COMMAND python ${CMAKE_SOURCE_DIR}/ServiceComponents/scripts/GenerateServiceComponent.py -i ${ARG_CONFIG_FILE} -o generated
        DEPENDS ${ARG_CONFIG_FILE}
    )

    # Create a static library from generated files
    list(FILTER GENERATED_OUTPUT_FILE_LIST INCLUDE REGEX ".*cpp")
    
    add_library (${TARGET_LIB} STATIC 
        ${GENERATED_OUTPUT_FILE_LIST}
    )
    target_include_directories (${TARGET_LIB} PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/generated/include
    )
    target_link_libraries(${TARGET_LIB} PUBLIC
        Common
        MessageBroker
        ServiceComponents
        )
endfunction()