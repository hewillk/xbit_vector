project(xbit_vector_test)

add_executable(xbit_vector_test test.cpp)
target_link_libraries(xbit_vector_test xbit_vector)

add_test(NAME xbit_vector COMMAND xbit_vector_test)