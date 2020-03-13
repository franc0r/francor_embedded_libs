#
# FRANCOR e.V. - Embedded Library Tests
#
# Author: Martin Bauernschmitt

cmake_minimum_required(VERSION 3.0.0)

#
# Project description
#

project(francor_embedded_unit_tests)
set(PROJECT_DESCRIPTION "FRANCOR e.V. Embedded Library Unit Test Binary")

#
# Project configuration
#

set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)

#
# Unit test executable paths and settings
#

set(TEST_SRCS 
    ${CMAKE_SOURCE_DIR}/src/tests.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/qvariable_tests.cpp
)

add_executable(${TARGET_NAME} ${TEST_SRCS})

target_compile_definitions(
  ${TARGET_NAME}
  PUBLIC
  -DBUILD_TESTS
)

target_include_directories(
  ${TARGET_NAME} PUBLIC
  ${CMAKE_SOURCE_DIR}/../include
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
  ${TARGET_NAME}
  pthread
  CppUTest
  CppUTestExt
)

#
# Post build execution of binary
#
add_custom_command(
  TARGET ${TARGET_NAME}
  POST_BUILD
  COMMAND ${TARGET_NAME} -v -c
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)