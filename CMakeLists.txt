# MIT Licence
#
# KeyChattering
#
# Copyright © 2021 Erwan Saclier de la Bâtie (Erwan28250)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
# to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.16.0)
project(KeyChattering CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)

# Check if cxxopt is installed.
if (EXISTS "${CMAKE_SOURCE_DIR}/dependencies/cxxopts/include/cxxopts.hpp")
    include_directories("${CMAKE_SOURCE_DIR}/dependencies/cxxopts/include/")
    message(STATUS "cxxopts found.")
else()
    message(FATAL_ERROR "\ncxxopts not found.")
endif()

set(KEY_CHATTERING_INCLUDE
    "include/KeyboardHook.h"
    "include/KeyPressData.h"
    "include/Application.h"
    "include/CommandLineParsing.h")

set(KEY_CHATTERING_SCR
    "src/main.cpp"
    "src/KeyboardHook.cpp"
    "src/KeyPressData.cpp"
    "src/Application.cpp"
    "src/CommandLineParsing.cpp")

add_executable(KeyChattering
    ${KEY_CHATTERING_INCLUDE}
    ${KEY_CHATTERING_SCR})
set_target_properties(KeyChattering PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)