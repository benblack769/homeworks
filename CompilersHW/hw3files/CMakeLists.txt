cmake_minimum_required (VERSION 3.8)

set(CPP_SOURCE_DIR ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable(test_tree_exec ${CPP_TEST_DIR}test_tree.cc ${CPP_TEST_DIR}tree.hh ${CPP_TEST_DIR}vectree.cc ${CPP_TEST_DIR}vectree.hh ${CPP_TEST_DIR}ptrtree.cc ${CPP_TEST_DIR}ptrtree.hh ${CPP_TEST_DIR}catch.cc)

set_property(TARGET test_tree_exec PROPERTY CXX_STANDARD 11)
set_property(TARGET test_tree_exec PROPERTY CXX_STANDARD_REQUIRED ON)
