cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME pre10-patient-stm32-v1.0.0)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Src/app_task_default.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Src/app_task_com.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Src/app_task_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Src/app_task_stop_ups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Src/app_task_torqueMove.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Src/app_task_arm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Src/app_task_park.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Src/bsp_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Src/bsp_usart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Src/bsp_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Src/bsp_torque_com.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Src/bsp_servomotor_com.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Src/bsp_adc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Src/bsp_crc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Src/bsp_ws2812.c





)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# 生成 map 文件，用于 VSCode 左侧的 Build Analyzer 内存分析
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map,--cref
)



# 生成 hex 文件
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
    $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex)

# 生成 bin 文件
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
    $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin)