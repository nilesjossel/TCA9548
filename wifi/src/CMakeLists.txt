add_executable(wifi
    main.cpp
    fsm.cpp
    wrappers.cpp
)

target_include_directories(wifi PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(wifi
    pico_stdlib 
    FreeRTOS-Kernel-Heap4
    )

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(wifi
        pico_cyw43_arch_none 
        pico_stdlib 
        FreeRTOS-Kernel-Heap4
    )
endif()

pico_add_extra_outputs(wifi)