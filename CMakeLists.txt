cmake_minimum_required(VERSION 3.30)

project(rune)

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
