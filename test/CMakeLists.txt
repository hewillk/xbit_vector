cmake_minimum_required(VERSION 3.14)

project(xbit_vector_tests CXX)

# ---- Dependencies ----

set(xbit_vector_INCLUDE_WITHOUT_SYSTEM YES CACHE INTERNAL "")

include(FetchContent)
FetchContent_Declare(xbit_vector SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
FetchContent_MakeAvailable(xbit_vector)

find_package(Catch2 2.13.4 CONFIG REQUIRED)
include(Catch)

# ---- Test ----

enable_testing()

add_executable(xbit_vector_test test.cpp)
target_compile_features(xbit_vector_test PRIVATE cxx_std_20)
target_link_libraries(
    xbit_vector_test
    PRIVATE
    Catch2::Catch2WithMain
    xbit_vector::xbit_vector
)
target_compile_options(
    xbit_vector_test
    PRIVATE
    # TODO: maybe abstract these into a CMake module with cross platform flags?
    -Wall
    -Wextra
)
catch_discover_tests(xbit_vector_test)
