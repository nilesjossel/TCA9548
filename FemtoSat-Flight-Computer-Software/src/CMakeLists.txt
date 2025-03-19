add_executable(rp2350-freertos-template
    main.cpp
)

target_include_directories(rp2350-freertos-template PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(rp2350-freertos-template 
    pico_stdlib 
    hardware_i2c
    hardware_spi
    hardware_dma
    hardware_pio
    hardware_interp
    hardware_timer
    hardware_watchdog
    hardware_clocks
    hardware_pwm
    pico_multicore

    FreeRTOS-Kernel-Heap4
    BNO08x_Pico_Library
    Pico_BME280_Driver
    RadioLib
    )

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(rp2350-freertos-template 
        pico_cyw43_arch_none 
    )
endif()

pico_add_extra_outputs(rp2350-freertos-template)

# Modify the below lines to enable/disable console output over UART/USB
pico_enable_stdio_uart(rp2350-freertos-template 0)
pico_enable_stdio_usb(rp2350-freertos-template 1)