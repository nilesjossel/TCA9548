add_executable(rp2350-freertos-template
    main.cpp
    fsm.cpp
    wrappers.cpp
)

target_include_directories(rp2350-freertos-template PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(rp2350-freertos-template 
    pico_stdlib 
    FreeRTOS-Kernel-Heap4
    )

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(rp2350-freertos-template 
        pico_cyw43_arch_none 
        pico_stdlib 
        FreeRTOS-Kernel-Heap4
    )
endif()

pico_add_extra_outputs(rp2350-freertos-template)