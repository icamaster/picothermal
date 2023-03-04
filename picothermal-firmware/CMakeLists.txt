cmake_minimum_required(VERSION 3.21)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

# Enable compile commands for SonarLint
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(PICO_BOARD none CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(cmake/pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.5.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

project(picothermal C CXX ASM)

# Add executable. Default name is the project name, version 0.1
add_executable(${PROJECT_NAME} src/main.c)

pico_set_program_name(${PROJECT_NAME} "picothermal")
pico_set_program_version(${PROJECT_NAME} "0.1")

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_semihosting(${PROJECT_NAME} 1)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
  pico_stdlib
)

# Add DEBUG definition based on build type
target_compile_definitions(${PROJECT_NAME} PRIVATE "DEBUG=$<IF:$<CONFIG:Debug>,1,0>")

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
  src
)

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME}
  hardware_spi
)

# Additonal liker options
target_link_options(${PROJECT_NAME} PRIVATE
  -Wl,--print-memory-usage 
)

# Generate UF2 file
pico_add_extra_outputs(${PROJECT_NAME})

