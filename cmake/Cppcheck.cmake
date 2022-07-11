## **********************************
## Adds cppcheck static code analysis
##
## **********************************

# FIXME remove globbing!
file(GLOB_RECURSE SRC_ALL 
    ${PROJECT_SOURCE_DIR}/MachineController/*.cpp
    ${PROJECT_SOURCE_DIR}/MachineApplication/*.cpp
    ${PROJECT_SOURCE_DIR}/MachineService/*.cpp
    ${PROJECT_SOURCE_DIR}/Common/*.cpp
    ${PROJECT_SOURCE_DIR}/ServiceComponents/*.cpp
    ${PROJECT_SOURCE_DIR}/CupRotationTray/*.cpp
    ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer/*.cpp
    ${PROJECT_SOURCE_DIR}/HardwareServiceComponents/*.cpp
    ${PROJECT_SOURCE_DIR}/MessageBroker/*.cpp
    )
file(GLOB_RECURSE HDR_ALL 
    ${PROJECT_SOURCE_DIR}/MachineController/*.hpp
    ${PROJECT_SOURCE_DIR}/MachineApplication/*.hpp
    ${PROJECT_SOURCE_DIR}/MachineService/*.hpp
    ${PROJECT_SOURCE_DIR}/Common/*.hpp
    ${PROJECT_SOURCE_DIR}/ServiceComponents/*.hpp
    ${PROJECT_SOURCE_DIR}/CupRotationTray/*.hpp
    ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer/*.hpp
    ${PROJECT_SOURCE_DIR}/HardwareServiceComponents/*.hpp
    ${PROJECT_SOURCE_DIR}/MessageBroker/*.hpp
    )

find_program(CPPCHECK_EXEC NAMES cppcheck)
if (CPPCHECK_EXEC)
    message("Static code analysis enabled for '${CPPCHECK_EXEC}'.")
    add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK_EXEC}
        --quiet
        --enable=all
        --std=c++11
        --inline-suppr 
        --suppress=missingIncludeSystem
        -I ${PROJECT_SOURCE_DIR}/MachineController/include
        -I ${PROJECT_SOURCE_DIR}/MachineApplication/include
        -I ${PROJECT_SOURCE_DIR}/MachineService/include
        -I ${PROJECT_SOURCE_DIR}/Common/include
        -I ${PROJECT_SOURCE_DIR}/ServiceComponents/include
        -I ${PROJECT_SOURCE_DIR}/CupRotationTray/include
        -I ${PROJECT_SOURCE_DIR}/HardwareAbstractionLayer/include
        -I ${PROJECT_SOURCE_DIR}/HardwareServiceComponents/include
        -I ${PROJECT_SOURCE_DIR}/MessageBroker/include
        ${SRC_ALL}
    )
    add_dependencies(MachineApplication cppcheck)
endif()
