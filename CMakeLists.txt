cmake_minimum_required(VERSION 3.21)

if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
    set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
    set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
    set(CMAKE_SIZE arm-none-eabi-size)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

project(nucleo_h753zi LANGUAGES C ASM)

set(TARGET_NAME nucleo_h753zi)
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Linker/STM32H753ZITX_FLASH.ld")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS NO)

set(CPU_FLAGS
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)

file(GLOB_RECURSE ASM_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Startup/*.s"
)

add_executable(${TARGET_NAME} ${ASM_SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LINK_DEPENDS "${LINKER_SCRIPT}"
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    STM32H753xx
    USE_PWR_LDO_SUPPLY
)
target_compile_options(${TARGET_NAME} PRIVATE
    ${CPU_FLAGS}
    -O0
    -g3
    -Wall
    -Wextra
    -ffunction-sections
    -fdata-sections
)

add_subdirectory(CMSIS)
add_subdirectory(App)
add_subdirectory(Drivers)

target_link_options(${TARGET_NAME} PRIVATE
    ${CPU_FLAGS}
    "-T${LINKER_SCRIPT}"
    "-Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map"
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,--fatal-warnings
    -nostartfiles
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND "${CMAKE_OBJCOPY}" -O binary
        "$<TARGET_FILE:${TARGET_NAME}>"
        "${CMAKE_BINARY_DIR}/${TARGET_NAME}.bin"
    COMMAND "${CMAKE_SIZE}" "$<TARGET_FILE:${TARGET_NAME}>"
    VERBATIM
)
