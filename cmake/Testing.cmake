## **********************************
## Adds testing
##
## **********************************

if (NOT NO_UNITTEST)
    enable_testing()
    find_package(GTest REQUIRED)
endif()