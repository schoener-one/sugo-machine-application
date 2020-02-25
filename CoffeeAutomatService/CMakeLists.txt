# CMake configuration file
# Module: CoffeeAutomatService

add_library (CoffeeAutomatService STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CoffeeAutomatService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CommunicationService.cpp
    )
target_include_directories (CoffeeAutomatService PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CoffeeAutomatService
    Common
    Protocol
    MessageBroker
    zmq
    )