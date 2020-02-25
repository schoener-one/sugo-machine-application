# CMake configuration file
# Module: CoffeeAutomat

add_library (CoffeeAutomat STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CoffeeAutomat.cpp
    )
target_include_directories (CoffeeAutomat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CoffeeAutomat
    Common
    Protocol
    MessageBroker
    CupRotationTray
    )