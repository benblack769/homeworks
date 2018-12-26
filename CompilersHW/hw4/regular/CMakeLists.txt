cmake_minimum_required (VERSION 3.8)

set(CPP_SOURCE_DIR ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -g")

add_executable(encode ${CPP_TEST_DIR}encode.cc ${CPP_TEST_DIR}huffman.hh)
add_executable(decode ${CPP_TEST_DIR}decode.cc ${CPP_TEST_DIR}huffman.hh)

set_property(TARGET encode PROPERTY CXX_STANDARD 11)
set_property(TARGET encode PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET decode PROPERTY CXX_STANDARD 11)
set_property(TARGET decode PROPERTY CXX_STANDARD_REQUIRED ON)
