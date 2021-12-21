## **********************************
## Adds additional dependent libraries
##
## **********************************

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
    include(FindGit)
    find_package(Git)

    if (NOT Git_FOUND)
        message(FATAL_ERROR "Git not found!")
    endif ()

    include(ExternalProject)
    ExternalProject_Add(azmq
        GIT_REPOSITORY    https://github.com/zeromq/azmq.git
        GIT_SHALLOW       ON
        PREFIX            azmq
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -B <BINARY_DIR> -S <SOURCE_DIR> -GNinja -DAZMQ_NO_TESTS=1 -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        BUILD_COMMAND     ${CMAKE_COMMAND} --build <BINARY_DIR>
        BUILD_ALWAYS      OFF
    )
    ExternalProject_Get_Property(azmq INSTALL_DIR)
    message("azmq install dir: ${INSTALL_DIR}")
    include_directories(${INSTALL_DIR}/include)

    ExternalProject_Add(jsonrpcpp
        GIT_REPOSITORY    https://github.com/badaix/jsonrpcpp.git
        GIT_TAG           v1.3.2
        GIT_SHALLOW       ON
        PREFIX            jsonrpcpp
        CONFIGURE_COMMAND ""
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -B <BINARY_DIR> -S <SOURCE_DIR> -GNinja -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        BUILD_COMMAND     ${CMAKE_COMMAND} --build <BINARY_DIR>
        BUILD_ALWAYS      OFF
    )
    ExternalProject_Get_Property(jsonrpcpp INSTALL_DIR)
    message("jsonrpcpp install dir: ${INSTALL_DIR}")
    include_directories(${INSTALL_DIR}/include)
endif()