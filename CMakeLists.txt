cmake_minimum_required(VERSION 3.12)
project(xbit_vector CXX)

add_library(xbit_vector INTERFACE)
target_include_directories(xbit_vector INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

option(XBIT_VECTOR_TESTS "Build the tests" ON)
if(XBIT_VECTOR_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()