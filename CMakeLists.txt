cmake_minimum_required(VERSION 3.16.0)
project(KeyChattering CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)

set(KEY_CHATTERING_INCLUDE
    "include/KeyboardHook.h"
    "include/KeyPressData.h"
    "include/Application.h")

set(KEY_CHATTERING_SCR
    "src/main.cpp"
    "src/KeyboardHook.cpp"
    "src/KeyPressData.cpp"
    "src/Application.cpp")

add_executable(KeyChattering
    ${KEY_CHATTERING_INCLUDE}
    ${KEY_CHATTERING_SCR})
set_target_properties(KeyChattering PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)