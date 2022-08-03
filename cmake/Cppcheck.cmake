## **********************************
## Adds cppcheck static code analysis
##
## **********************************

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
        -I ${PROJECT_SOURCE_DIR}/MachineServiceGateway/include
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
