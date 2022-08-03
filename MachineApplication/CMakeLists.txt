# CMake configuration file
# Module: ${MODULE_NAME}
set(MODULE_NAME "MachineApplication")
add_executable(${MODULE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MachineApplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    )
target_include_directories (${MODULE_NAME}
    PUBLIC
    include
    )
target_link_libraries(${MODULE_NAME}
    MachineServiceComponents-ExecGroup-GenLib
    MachineServiceComponents
    )
 
if (NOT NO_UNITTEST)
    add_subdirectory(test)
endif()
