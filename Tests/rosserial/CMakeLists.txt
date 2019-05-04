#
# CMakeFile for FRANCOR Embedded Libs
#
# Author: Martin Bauernschmitt
# Date: 04.05.2019

# Minimum required cmake version
cmake_minimum_required(VERSION 3.0.0)

# Project description
project(rosserial_stm32f4_test VERSION 0.1)

# Enable testing
enable_testing()

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add include directories
include_directories(
  ${CMAKE_SOURCE_DIR}/../Device/STM32F446RE/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
  ${CMAKE_SOURCE_DIR}/../Device/STM32F446RE/Drivers/STM32F4xx_HAL_Driver/Inc
  ${CMAKE_SOURCE_DIR}/../Device/STM32F446RE/Sim
  ${CMAKE_SOURCE_DIR}/../Device/STM32F446RE/Inc
)

# Add source files
AUX_SOURCE_DIRECTORY(
  ${CMAKE_SOURCE_DIR}/../Device/STM32F446RE/Drivers/STM32F4xx_HAL_Driver/Src
  device_srcs
)

AUX_SOURCE_DIRECTORY(
  ${CMAKE_SOURCE_DIR}/../Device/STM32F446RE/Src
  device_srcs
)

AUX_SOURCE_DIRECTORY(
  ${CMAKE_SOURCE_DIR}/../Device/STM32F446RE/Sim
  device_srcs
)

# Add test files
AUX_SOURCE_DIRECTORY(
  ${CMAKE_SOURCE_DIR}/src
  test_srcs
)

# Add definitions
add_definitions(
  -DSTM32F446xx
  -DUSE_HAL_DRIVER
  -DBUILD_TESTS
)

# Disable warnings
set(CMAKE_C_FLAGS "-Wno-pointer-to-int-cast -Wno-pointer-compare -Wno-int-to-pointer-cast")

# Create target name
set(TARGET_NAME ${PROJECT_NAME}_V${PROJECT_VERSION})

# Create executable
add_executable(${TARGET_NAME} ${device_srcs} ${test_srcs})

# Link libraries
target_link_libraries(${TARGET_NAME} pthread)
target_link_libraries(${TARGET_NAME} CppUTest)
target_link_libraries(${TARGET_NAME} CppUTestExt)

# Add test
add_test(NAME rosserial_stm32f4_test COMMAND ${TARGET_NAME})