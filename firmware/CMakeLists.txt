# SPDX-License-Identifier: Apache-2.0

set(BOARD_ROOT ${CMAKE_SOURCE_DIR}/..)

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(zephyr_cpp_fw)

set(FW_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(FW_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/source)

target_sources(
    app
    PRIVATE
        ${FW_SOURCE_DIR}/main.cpp
        ${FW_SOURCE_DIR}/gpio.cpp
        ${FW_SOURCE_DIR}/led.cpp
        ${FW_SOURCE_DIR}/leds_controller.cpp
)

target_include_directories(
    app
    PRIVATE
        ${FW_INCLUDE_DIR}
)

target_compile_options(
    app
    PRIVATE
        -fdata-sections
        -ffunction-sections
        --specs=nano.specs
        -Wl,--gc-sections

        -fno-rtti
        -fno-exceptions
        -fno-threadsafe-statics
)