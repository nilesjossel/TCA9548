add_executable(rp2350-freertos-template
    main.cpp
)

target_include_directories(rp2350-freertos-template PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(rp2350-freertos-template 
    pico_stdlib 
    FreeRTOS-Kernel-Heap4
    BNO08x_Pico_Library
    TCA9548
    Honeywell_ABP
    )

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(rp2350-freertos-template 
        pico_cyw43_arch_none 
        pico_stdlib 
        FreeRTOS-Kernel-Heap4
        BNO08x_Pico_Library
        TCA9548
        Honeywell_ABP
    )
endif()

pico_add_extra_outputs(rp2350-freertos-template)