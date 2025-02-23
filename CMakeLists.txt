cmake_minimum_required(VERSION 3.30)

project(dove)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("src")

enable_testing()

add_executable(test_message_passing tests/test_message_passing.cpp)
add_test(NAME test_message_passing COMMAND $<TARGET_FILE:test_message_passing>)

add_executable(test_remove_listener tests/test_remove_listener.cpp)
add_test(NAME test_remove_listener COMMAND $<TARGET_FILE:test_remove_listener>)

add_executable(test_register_many_listeners tests/test_register_many_listeners.cpp)
add_test(NAME test_register_many_listeners COMMAND $<TARGET_FILE:test_register_many_listeners>)

add_executable(test_custom_whoptr tests/test_custom_whoptr.cpp)
add_test(NAME test_custom_whoptr COMMAND $<TARGET_FILE:test_custom_whoptr>)
