file(GLOB_RECURSE DRIVER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Drivers/*.c"
)

target_sources(${TARGET_NAME} PRIVATE ${DRIVER_SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/Drivers/Ethernet"
    "${CMAKE_SOURCE_DIR}/Drivers/GPIO"
    "${CMAKE_SOURCE_DIR}/Drivers/UART"
)
